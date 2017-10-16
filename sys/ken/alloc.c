#
/*
 */

#include &quot;../param.h&quot;
#include &quot;../systm.h&quot;
#include &quot;../filsys.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../buf.h&quot;
#include &quot;../inode.h&quot;
#include &quot;../user.h&quot;

/*
 * iinit is called once (from main)
 * very early in initialization.
 * It reads the root&#39;s super block
 * and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super
 * block. Usually because of an IO error.
 */
iinit()
{
	register *cp, *bp;

	(*bdevsw[rootdev.d_major].d_open)(rootdev, 1);
	bp = bread(rootdev, 1);
	cp = getblk(NODEV);
	if(u.u_error)
		panic(&quot;iinit&quot;);
	bcopy(bp-&gt;b_addr, cp-&gt;b_addr, 256);
	brelse(bp);
	mount[0].m_bufp = cp;
	mount[0].m_dev = rootdev;
	cp = cp-&gt;b_addr;
	cp-&gt;s_flock = 0;
	cp-&gt;s_ilock = 0;
	cp-&gt;s_ronly = 0;
	time[0] = cp-&gt;s_time[0];
	time[1] = cp-&gt;s_time[1];
}

/*
 * alloc will obtain the next available
 * free disk block from the free list of
 * the specified device.
 * The super block has up to 100 remembered
 * free blocks; the last of these is read to
 * obtain 100 more . . .
 *
 * no space on dev x/y -- when
 * the free list is exhausted.
 */
alloc(dev)
{
	int bno;
	register *bp, *ip, *fp;

	fp = getfs(dev);
	while(fp-&gt;s_flock)
		sleep(&amp;fp-&gt;s_flock, PINOD);
	do {
		if(fp-&gt;s_nfree &lt;= 0)
			goto nospace;
		bno = fp-&gt;s_free[--fp-&gt;s_nfree];
		if(bno == 0)
			goto nospace;
	} while (badblock(fp, bno, dev));
	if(fp-&gt;s_nfree &lt;= 0) {
		fp-&gt;s_flock++;
		bp = bread(dev, bno);
		ip = bp-&gt;b_addr;
		fp-&gt;s_nfree = *ip++;
		bcopy(ip, fp-&gt;s_free, 100);
		brelse(bp);
		fp-&gt;s_flock = 0;
		wakeup(&amp;fp-&gt;s_flock);
	}
	bp = getblk(dev, bno);
	clrbuf(bp);
	fp-&gt;s_fmod = 1;
	return(bp);

nospace:
	fp-&gt;s_nfree = 0;
	prdev(&quot;no space&quot;, dev);
	u.u_error = ENOSPC;
	return(NULL);
}

/*
 * place the specified disk block
 * back on the free list of the
 * specified device.
 */
free(dev, bno)
{
	register *fp, *bp, *ip;

	fp = getfs(dev);
	fp-&gt;s_fmod = 1;
	while(fp-&gt;s_flock)
		sleep(&amp;fp-&gt;s_flock, PINOD);
	if (badblock(fp, bno, dev))
		return;
	if(fp-&gt;s_nfree &lt;= 0) {
		fp-&gt;s_nfree = 1;
		fp-&gt;s_free[0] = 0;
	}
	if(fp-&gt;s_nfree &gt;= 100) {
		fp-&gt;s_flock++;
		bp = getblk(dev, bno);
		ip = bp-&gt;b_addr;
		*ip++ = fp-&gt;s_nfree;
		bcopy(fp-&gt;s_free, ip, 100);
		fp-&gt;s_nfree = 0;
		bwrite(bp);
		fp-&gt;s_flock = 0;
		wakeup(&amp;fp-&gt;s_flock);
	}
	fp-&gt;s_free[fp-&gt;s_nfree++] = bno;
	fp-&gt;s_fmod = 1;
}

/*
 * Check that a block number is in the
 * range between the I list and the size
 * of the device.
 * This is used mainly to check that a
 * garbage file system has not been mounted.
 *
 * bad block on dev x/y -- not in range
 */
badblock(afp, abn, dev)
{
	register struct filsys *fp;
	register char *bn;

	fp = afp;
	bn = abn;
	if (bn &lt; fp-&gt;s_isize+2 || bn &gt;= fp-&gt;s_fsize) {
		prdev(&quot;bad block&quot;, dev);
		return(1);
	}
	return(0);
}

/*
 * Allocate an unused I node
 * on the specified device.
 * Used with file creation.
 * The algorithm keeps up to
 * 100 spare I nodes in the
 * super block. When this runs out,
 * a linear search through the
 * I list is instituted to pick
 * up 100 more.
 */
ialloc(dev)
{
	register *fp, *bp, *ip;
	int i, j, k, ino;

	fp = getfs(dev);
	while(fp-&gt;s_ilock)
		sleep(&amp;fp-&gt;s_ilock, PINOD);
loop:
	if(fp-&gt;s_ninode &gt; 0) {
		ino = fp-&gt;s_inode[--fp-&gt;s_ninode];
		ip = iget(dev, ino);
		if (ip==NULL)
			return(NULL);
		if(ip-&gt;i_mode == 0) {
			for(bp = &amp;ip-&gt;i_mode; bp &lt; &amp;ip-&gt;i_addr[8];)
				*bp++ = 0;
			fp-&gt;s_fmod = 1;
			return(ip);
		}
		/*
		 * Inode was allocated after all.
		 * Look some more.
		 */
		iput(ip);
		goto loop;
	}
	fp-&gt;s_ilock++;
	ino = 0;
	for(i=0; i&lt;fp-&gt;s_isize; i++) {
		bp = bread(dev, i+2);
		ip = bp-&gt;b_addr;
		for(j=0; j&lt;256; j=+16) {
			ino++;
			if(ip[j] != 0)
				continue;
			for(k=0; k&lt;NINODE; k++)
			if(dev==inode[k].i_dev &amp;&amp; ino==inode[k].i_number)
				goto cont;
			fp-&gt;s_inode[fp-&gt;s_ninode++] = ino;
			if(fp-&gt;s_ninode &gt;= 100)
				break;
		cont:;
		}
		brelse(bp);
		if(fp-&gt;s_ninode &gt;= 100)
			break;
	}
	fp-&gt;s_ilock = 0;
	wakeup(&amp;fp-&gt;s_ilock);
	if (fp-&gt;s_ninode &gt; 0)
		goto loop;
	prdev(&quot;Out of inodes&quot;, dev);
	u.u_error = ENOSPC;
	return(NULL);
}

/*
 * Free the specified I node
 * on the specified device.
 * The algorithm stores up
 * to 100 I nodes in the super
 * block and throws away any more.
 */
ifree(dev, ino)
{
	register *fp;

	fp = getfs(dev);
	if(fp-&gt;s_ilock)
		return;
	if(fp-&gt;s_ninode &gt;= 100)
		return;
	fp-&gt;s_inode[fp-&gt;s_ninode++] = ino;
	fp-&gt;s_fmod = 1;
}

/*
 * getfs maps a device number into
 * a pointer to the incore super
 * block.
 * The algorithm is a linear
 * search through the mount table.
 * A consistency check of the
 * in core free-block and i-node
 * counts.
 *
 * bad count on dev x/y -- the count
 *	check failed. At this point, all
 *	the counts are zeroed which will
 *	almost certainly lead to &quot;no space&quot;
 *	diagnostic
 * panic: no fs -- the device is not mounted.
 *	this &quot;cannot happen&quot;
 */
getfs(dev)
{
	register struct mount *p;
	register char *n1, *n2;

	for(p = &amp;mount[0]; p &lt; &amp;mount[NMOUNT]; p++)
	if(p-&gt;m_bufp != NULL &amp;&amp; p-&gt;m_dev == dev) {
		p = p-&gt;m_bufp-&gt;b_addr;
		n1 = p-&gt;s_nfree;
		n2 = p-&gt;s_ninode;
		if(n1 &gt; 100 || n2 &gt; 100) {
			prdev(&quot;bad count&quot;, dev);
			p-&gt;s_nfree = 0;
			p-&gt;s_ninode = 0;
		}
		return(p);
	}
	panic(&quot;no fs&quot;);
}

/*
 * update is the internal name of
 * &#39;sync&#39;. It goes through the disk
 * queues to initiate sandbagged IO;
 * goes through the I nodes to write
 * modified nodes; and it goes through
 * the mount table to initiate modified
 * super blocks.
 */
update()
{
	register struct inode *ip;
	register struct mount *mp;
	register *bp;

	if(updlock)
		return;
	updlock++;
	for(mp = &amp;mount[0]; mp &lt; &amp;mount[NMOUNT]; mp++)
		if(mp-&gt;m_bufp != NULL) {
			ip = mp-&gt;m_bufp-&gt;b_addr;
			if(ip-&gt;s_fmod==0 || ip-&gt;s_ilock!=0 ||
			   ip-&gt;s_flock!=0 || ip-&gt;s_ronly!=0)
				continue;
			bp = getblk(mp-&gt;m_dev, 1);
			ip-&gt;s_fmod = 0;
			ip-&gt;s_time[0] = time[0];
			ip-&gt;s_time[1] = time[1];
			bcopy(ip, bp-&gt;b_addr, 256);
			bwrite(bp);
		}
	for(ip = &amp;inode[0]; ip &lt; &amp;inode[NINODE]; ip++)
		if((ip-&gt;i_flag&amp;ILOCK) == 0) {
			ip-&gt;i_flag =| ILOCK;
			iupdat(ip, time);
			prele(ip);
		}
	updlock = 0;
	bflush(NODEV);
}
