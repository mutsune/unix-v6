#
#include &quot;../param.h&quot;
#include &quot;../inode.h&quot;
#include &quot;../user.h&quot;
#include &quot;../systm.h&quot;
#include &quot;../buf.h&quot;

/*
 * Convert a pathname into a pointer to
 * an inode. Note that the inode is locked.
 *
 * func = function called to get next char of name
 *	&amp;uchar if name is in user space
 *	&amp;schar if name is in system space
 * flag = 0 if name is sought
 *	1 if name is to be created
 *	2 if name is to be deleted
 */
namei(func, flag)
int (*func)();
{
	register struct inode *dp;
	register c;
	register char *cp;
	int eo, *bp;

	/*
	 * If name starts with &#39;/&#39; start from
	 * root; otherwise start from current dir.
	 */

	dp = u.u_cdir;
	if((c=(*func)()) == &#39;/&#39;)
		dp = rootdir;
	iget(dp-&gt;i_dev, dp-&gt;i_number);
	while(c == &#39;/&#39;)
		c = (*func)();
	if(c == &#39;\0&#39; &amp;&amp; flag != 0) {
		u.u_error = ENOENT;
		goto out;
	}

cloop:
	/*
	 * Here dp contains pointer
	 * to last component matched.
	 */

	if(u.u_error)
		goto out;
	if(c == &#39;\0&#39;)
		return(dp);

	/*
	 * If there is another component,
	 * dp must be a directory and
	 * must have x permission.
	 */

	if((dp-&gt;i_mode&amp;IFMT) != IFDIR) {
		u.u_error = ENOTDIR;
		goto out;
	}
	if(access(dp, IEXEC))
		goto out;

	/*
	 * Gather up name into
	 * users&#39; dir buffer.
	 */

	cp = &amp;u.u_dbuf[0];
	while(c!=&#39;/&#39; &amp;&amp; c!=&#39;\0&#39; &amp;&amp; u.u_error==0) {
		if(cp &lt; &amp;u.u_dbuf[DIRSIZ])
			*cp++ = c;
		c = (*func)();
	}
	while(cp &lt; &amp;u.u_dbuf[DIRSIZ])
		*cp++ = &#39;\0&#39;;
	while(c == &#39;/&#39;)
		c = (*func)();
	if(u.u_error)
		goto out;

	/*
	 * Set up to search a directory.
	 */

	u.u_offset[1] = 0;
	u.u_offset[0] = 0;
	u.u_segflg = 1;
	eo = 0;
	u.u_count = ldiv(dp-&gt;i_size1, DIRSIZ+2);
	bp = NULL;

eloop:

	/*
	 * If at the end of the directory,
	 * the search failed. Report what
	 * is appropriate as per flag.
	 */

	if(u.u_count == 0) {
		if(bp != NULL)
			brelse(bp);
		if(flag==1 &amp;&amp; c==&#39;\0&#39;) {
			if(access(dp, IWRITE))
				goto out;
			u.u_pdir = dp;
			if(eo)
				u.u_offset[1] = eo-DIRSIZ-2; else
				dp-&gt;i_flag =| IUPD;
			return(NULL);
		}
		u.u_error = ENOENT;
		goto out;
	}

	/*
	 * If offset is on a block boundary,
	 * read the next directory block.
	 * Release previous if it exists.
	 */

	if((u.u_offset[1]&amp;0777) == 0) {
		if(bp != NULL)
			brelse(bp);
		bp = bread(dp-&gt;i_dev,
			bmap(dp, ldiv(u.u_offset[1], 512)));
	}

	/*
	 * Note first empty directory slot
	 * in eo for possible creat.
	 * String compare the directory entry
	 * and the current component.
	 * If they do not match, go back to eloop.
	 */

	bcopy(bp-&gt;b_addr+(u.u_offset[1]&amp;0777), &amp;u.u_dent, (DIRSIZ+2)/2);
	u.u_offset[1] =+ DIRSIZ+2;
	u.u_count--;
	if(u.u_dent.u_ino == 0) {
		if(eo == 0)
			eo = u.u_offset[1];
		goto eloop;
	}
	for(cp = &amp;u.u_dbuf[0]; cp &lt; &amp;u.u_dbuf[DIRSIZ]; cp++)
		if(*cp != cp[u.u_dent.u_name - u.u_dbuf])
			goto eloop;

	/*
	 * Here a component matched in a directory.
	 * If there is more pathname, go back to
	 * cloop, otherwise return.
	 */

	if(bp != NULL)
		brelse(bp);
	if(flag==2 &amp;&amp; c==&#39;\0&#39;) {
		if(access(dp, IWRITE))
			goto out;
		return(dp);
	}
	bp = dp-&gt;i_dev;
	iput(dp);
	dp = iget(bp, u.u_dent.u_ino);
	if(dp == NULL)
		return(NULL);
	goto cloop;

out:
	iput(dp);
	return(NULL);
}

/*
 * Return the next character from the
 * kernel string pointed at by dirp.
 */
schar()
{

	return(*u.u_dirp++ &amp; 0377);
}

/*
 * Return the next character from the
 * user string pointed at by dirp.
 */
uchar()
{
	register c;

	c = fubyte(u.u_dirp++);
	if(c == -1)
		u.u_error = EFAULT;
	return(c);
}
