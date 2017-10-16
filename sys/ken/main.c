#
#include &quot;../param.h&quot;
#include &quot;../user.h&quot;
#include &quot;../systm.h&quot;
#include &quot;../proc.h&quot;
#include &quot;../text.h&quot;
#include &quot;../inode.h&quot;
#include &quot;../seg.h&quot;

#define	CLOCK1	0177546
#define	CLOCK2	0172540
/*
 * Icode is the octal bootstrap
 * program executed in user mode
 * to bring up the system.
 */
int	icode[]
{
	0104413,	/* sys exec; init; initp */
	0000014,
	0000010,
	0000777,	/* br . */
	0000014,	/* initp: init; 0 */
	0000000,
	0062457,	/* init: &lt;/etc/init\0&gt; */
	0061564,
	0064457,
	0064556,
	0000164,
};

/*
 * Initialization code.
 * Called from m40.s or m45.s as
 * soon as a stack and segmentation
 * have been established.
 * Functions:
 *	clear and free user core
 *	find which clock is configured
 *	hand craft 0th process
 *	call all initialization routines
 *	fork - process 0 to schedule
 *	     - process 1 execute bootstrap
 *
 * panic: no clock -- neither clock responds
 * loop at loc 6 in user mode -- /etc/init
 *	cannot be executed.
 */
main()
{
	extern schar;
	register i, *p;

	/*
	 * zero and free all of core
	 */

	updlock = 0;
	i = *ka6 + USIZE;
	UISD-&gt;r[0] = 077406;
	for(;;) {
		UISA-&gt;r[0] = i;
		if(fuibyte(0) &lt; 0)
			break;
		clearseg(i);
		maxmem++;
		mfree(coremap, 1, i);
		i++;
	}
	if(cputype == 70)
	for(i=0; i&lt;62; i=+2) {
		UBMAP-&gt;r[i] = i&lt;&lt;12;
		UBMAP-&gt;r[i+1] = 0;
	}
	printf(&quot;mem = %l\n&quot;, maxmem*5/16);
	maxmem = min(maxmem, MAXMEM);
	mfree(swapmap, nswap, swplo);

	/*
	 * determine clock
	 */

	UISA-&gt;r[7] = ka6[1]; /* io segment */
	UISD-&gt;r[7] = 077406;
	lks = CLOCK1;
	if(fuiword(lks) == -1) {
		lks = CLOCK2;
		if(fuiword(lks) == -1)
			panic(&quot;no clock&quot;);
	}

	/*
	 * set up system process
	 */

	proc[0].p_addr = *ka6;
	proc[0].p_size = USIZE;
	proc[0].p_stat = SRUN;
	proc[0].p_flag =| SLOAD|SSYS;
	u.u_procp = &amp;proc[0];

	/*
	 * set up &#39;known&#39; i-nodes
	 */

	*lks = 0115;
	cinit();
	binit();
	iinit();
	rootdir = iget(rootdev, ROOTINO);
	rootdir-&gt;i_flag =&amp; ~ILOCK;
	u.u_cdir = iget(rootdev, ROOTINO);
	u.u_cdir-&gt;i_flag =&amp; ~ILOCK;

	/*
	 * make init process
	 * enter scheduling loop
	 * with system process
	 */

	if(newproc()) {
		expand(USIZE+1);
		estabur(0, 1, 0, 0);
		copyout(icode, 0, sizeof icode);
		/*
		 * Return goes to loc. 0 of user init
		 * code just copied out.
		 */
		return;
	}
	sched();
}

/*
 * Load the user hardware segmentation
 * registers from the software prototype.
 * The software registers must have
 * been setup prior by estabur.
 */
sureg()
{
	register *up, *rp, a;

	a = u.u_procp-&gt;p_addr;
	up = &amp;u.u_uisa[16];
	rp = &amp;UISA-&gt;r[16];
	if(cputype == 40) {
		up =- 8;
		rp =- 8;
	}
	while(rp &gt; &amp;UISA-&gt;r[0])
		*--rp = *--up + a;
	if((up=u.u_procp-&gt;p_textp) != NULL)
		a =- up-&gt;x_caddr;
	up = &amp;u.u_uisd[16];
	rp = &amp;UISD-&gt;r[16];
	if(cputype == 40) {
		up =- 8;
		rp =- 8;
	}
	while(rp &gt; &amp;UISD-&gt;r[0]) {
		*--rp = *--up;
		if((*rp &amp; WO) == 0)
			rp[(UISA-UISD)/2] =- a;
	}
}

/*
 * Set up software prototype segmentation
 * registers to implement the 3 pseudo
 * text,data,stack segment sizes passed
 * as arguments.
 * The argument sep specifies if the
 * text and data+stack segments are to
 * be separated.
 */
estabur(nt, nd, ns, sep)
{
	register a, *ap, *dp;

	if(sep) {
		if(cputype == 40)
			goto err;
		if(nseg(nt) &gt; 8 || nseg(nd)+nseg(ns) &gt; 8)
			goto err;
	} else
		if(nseg(nt)+nseg(nd)+nseg(ns) &gt; 8)
			goto err;
	if(nt+nd+ns+USIZE &gt; maxmem)
		goto err;
	a = 0;
	ap = &amp;u.u_uisa[0];
	dp = &amp;u.u_uisd[0];
	while(nt &gt;= 128) {
		*dp++ = (127&lt;&lt;8) | RO;
		*ap++ = a;
		a =+ 128;
		nt =- 128;
	}
	if(nt) {
		*dp++ = ((nt-1)&lt;&lt;8) | RO;
		*ap++ = a;
	}
	if(sep)
	while(ap &lt; &amp;u.u_uisa[8]) {
		*ap++ = 0;
		*dp++ = 0;
	}
	a = USIZE;
	while(nd &gt;= 128) {
		*dp++ = (127&lt;&lt;8) | RW;
		*ap++ = a;
		a =+ 128;
		nd =- 128;
	}
	if(nd) {
		*dp++ = ((nd-1)&lt;&lt;8) | RW;
		*ap++ = a;
		a =+ nd;
	}
	while(ap &lt; &amp;u.u_uisa[8]) {
		*dp++ = 0;
		*ap++ = 0;
	}
	if(sep)
	while(ap &lt; &amp;u.u_uisa[16]) {
		*dp++ = 0;
		*ap++ = 0;
	}
	a =+ ns;
	while(ns &gt;= 128) {
		a =- 128;
		ns =- 128;
		*--dp = (127&lt;&lt;8) | RW;
		*--ap = a;
	}
	if(ns) {
		*--dp = ((128-ns)&lt;&lt;8) | RW | ED;
		*--ap = a-128;
	}
	if(!sep) {
		ap = &amp;u.u_uisa[0];
		dp = &amp;u.u_uisa[8];
		while(ap &lt; &amp;u.u_uisa[8])
			*dp++ = *ap++;
		ap = &amp;u.u_uisd[0];
		dp = &amp;u.u_uisd[8];
		while(ap &lt; &amp;u.u_uisd[8])
			*dp++ = *ap++;
	}
	sureg();
	return(0);

err:
	u.u_error = ENOMEM;
	return(-1);
}

/*
 * Return the arg/128 rounded up.
 */
nseg(n)
{

	return((n+127)&gt;&gt;7);
}
