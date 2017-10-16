#
/*
 */

#include &quot;../param.h&quot;
#include &quot;../user.h&quot;
#include &quot;../filsys.h&quot;
#include &quot;../file.h&quot;
#include &quot;../conf.h&quot;
#include &quot;../inode.h&quot;
#include &quot;../reg.h&quot;

/*
 * Convert a user supplied
 * file descriptor into a pointer
 * to a file structure.
 * Only task is to check range
 * of the descriptor.
 */
getf(f)
{
	register *fp, rf;

	rf = f;
	if(rf&lt;0 || rf&gt;=NOFILE)
		goto bad;
	fp = u.u_ofile[rf];
	if(fp != NULL)
		return(fp);
bad:
	u.u_error = EBADF;
	return(NULL);
}

/*
 * Internal form of close.
 * Decrement reference count on
 * file structure and call closei
 * on last closef.
 * Also make sure the pipe protocol
 * does not constipate.
 */
closef(fp)
int *fp;
{
	register *rfp, *ip;

	rfp = fp;
	if(rfp-&gt;f_flag&amp;FPIPE) {
		ip = rfp-&gt;f_inode;
		ip-&gt;i_mode =&amp; ~(IREAD|IWRITE);
		wakeup(ip+1);
		wakeup(ip+2);
	}
	if(rfp-&gt;f_count &lt;= 1)
		closei(rfp-&gt;f_inode, rfp-&gt;f_flag&amp;FWRITE);
	rfp-&gt;f_count--;
}

/*
 * Decrement reference count on an
 * inode due to the removal of a
 * referencing file structure.
 * On the last closei, switchout
 * to the close entry point of special
 * device handler.
 * Note that the handler gets called
 * on every open and only on the last
 * close.
 */
closei(ip, rw)
int *ip;
{
	register *rip;
	register dev, maj;

	rip = ip;
	dev = rip-&gt;i_addr[0];
	maj = rip-&gt;i_addr[0].d_major;
	if(rip-&gt;i_count &lt;= 1)
	switch(rip-&gt;i_mode&amp;IFMT) {

	case IFCHR:
		(*cdevsw[maj].d_close)(dev, rw);
		break;

	case IFBLK:
		(*bdevsw[maj].d_close)(dev, rw);
	}
	iput(rip);
}

/*
 * openi called to allow handler
 * of special files to initialize and
 * validate before actual IO.
 * Called on all sorts of opens
 * and also on mount.
 */
openi(ip, rw)
int *ip;
{
	register *rip;
	register dev, maj;

	rip = ip;
	dev = rip-&gt;i_addr[0];
	maj = rip-&gt;i_addr[0].d_major;
	switch(rip-&gt;i_mode&amp;IFMT) {

	case IFCHR:
		if(maj &gt;= nchrdev)
			goto bad;
		(*cdevsw[maj].d_open)(dev, rw);
		break;

	case IFBLK:
		if(maj &gt;= nblkdev)
			goto bad;
		(*bdevsw[maj].d_open)(dev, rw);
	}
	return;

bad:
	u.u_error = ENXIO;
}

/*
 * Check mode permission on inode pointer.
 * Mode is READ, WRITE or EXEC.
 * In the case of WRITE, the
 * read-only status of the file
 * system is checked.
 * Also in WRITE, prototype text
 * segments cannot be written.
 * The mode is shifted to select
 * the owner/group/other fields.
 * The super user is granted all
 * permissions except for EXEC where
 * at least one of the EXEC bits must
 * be on.
 */
access(aip, mode)
int *aip;
{
	register *ip, m;

	ip = aip;
	m = mode;
	if(m == IWRITE) {
		if(getfs(ip-&gt;i_dev)-&gt;s_ronly != 0) {
			u.u_error = EROFS;
			return(1);
		}
		if(ip-&gt;i_flag &amp; ITEXT) {
			u.u_error = ETXTBSY;
			return(1);
		}
	}
	if(u.u_uid == 0) {
		if(m == IEXEC &amp;&amp; (ip-&gt;i_mode &amp; 
			(IEXEC | (IEXEC&gt;&gt;3) | (IEXEC&gt;&gt;6))) == 0)
				goto bad;
		return(0);
	}
	if(u.u_uid != ip-&gt;i_uid) {
		m =&gt;&gt; 3;
		if(u.u_gid != ip-&gt;i_gid)
			m =&gt;&gt; 3;
	}
	if((ip-&gt;i_mode&amp;m) != 0)
		return(0);

bad:
	u.u_error = EACCES;
	return(1);
}

/*
 * Look up a pathname and test if
 * the resultant inode is owned by the
 * current user.
 * If not, try for super-user.
 * If permission is granted,
 * return inode pointer.
 */
owner()
{
	register struct inode *ip;
	extern uchar();

	if ((ip = namei(uchar, 0)) == NULL)
		return(NULL);
	if(u.u_uid == ip-&gt;i_uid)
		return(ip);
	if (suser())
		return(ip);
	iput(ip);
	return(NULL);
}

/*
 * Test if the current user is the
 * super user.
 */
suser()
{

	if(u.u_uid == 0)
		return(1);
	u.u_error = EPERM;
	return(0);
}

/*
 * Allocate a user file descriptor.
 */
ufalloc()
{
	register i;

	for (i=0; i&lt;NOFILE; i++)
		if (u.u_ofile[i] == NULL) {
			u.u_ar0[R0] = i;
			return(i);
		}
	u.u_error = EMFILE;
	return(-1);
}

/*
 * Allocate a user file descriptor
 * and a file structure.
 * Initialize the descriptor
 * to point at the file structure.
 *
 * no file -- if there are no available
 * 	file structures.
 */
falloc()
{
	register struct file *fp;
	register i;

	if ((i = ufalloc()) &lt; 0)
		return(NULL);
	for (fp = &amp;file[0]; fp &lt; &amp;file[NFILE]; fp++)
		if (fp-&gt;f_count==0) {
			u.u_ofile[i] = fp;
			fp-&gt;f_count++;
			fp-&gt;f_offset[0] = 0;
			fp-&gt;f_offset[1] = 0;
			return(fp);
		}
	printf(&quot;no file\n&quot;);
	u.u_error = ENFILE;
	return(NULL);
}
