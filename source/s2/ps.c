#

/*
 *	ps - process status
 *	examine and print certain things about processes
 */

#include &quot;/usr/sys/param.h&quot;
#include &quot;/usr/sys/proc.h&quot;
#include &quot;/usr/sys/tty.h&quot;
#include &quot;/usr/sys/user.h&quot;

struct {
	char name[8];
	int  type;
	char  *value;
} nl[3];

struct proc proc[NPROC];
struct tty tty;
struct user u;
int	lflg;
int	kflg;
int	xflg;
int	tflg;
int	aflg;
int	mem;
int	swap;

int	stbuf[257];
int	ndev;
char	devc[65];
int	devl[65];
int	devt[65];
char	*coref;
struct ibuf {
	char	idevmin, idevmaj;
	int	inum;
	int	iflags;
	char	inl;
	char	iuid;
	char	igid;
	char	isize0;
	int	isize;
	int	iaddr[8];
	char	*ictime[2];
	char	*imtime[2];
	int	fill;
};
int	obuf[259];


main(argc, argv)
char **argv;
{
	struct proc *p;
	int n, b;
	int i, c, mtty;
	char *ap;
	int uid, puid;

	obuf[0] = 1;
	if (argc&gt;1) {
		ap = argv[1];
		while (*ap) switch (*ap++) {
		case &#39;a&#39;:
			aflg++;
			break;

		case &#39;t&#39;:
			tflg++;
			break;

		case &#39;x&#39;:
			xflg++;
			break;

		case &#39;l&#39;:
			lflg++;
			break;

		case &#39;k&#39;:
			kflg++;
			break;

		}
	}

	if(chdir(&quot;/dev&quot;) &lt; 0) {
		printf(&quot;cannot change to /dev\n&quot;);
		done();
	}
	setup(&amp;nl[0], &quot;_proc&quot;);
	setup(&amp;nl[1], &quot;_swapdev&quot;);
	nlist(argc&gt;2? argv[2]:&quot;/unix&quot;, nl);
	if (nl[0].type==0) {
		printf(&quot;No namelist\n&quot;);
		return;
	}
	coref = &quot;/dev/mem&quot;;
	if(kflg)
		coref = &quot;/usr/sys/core&quot;;
	if ((mem = open(coref, 0)) &lt; 0) {
		printf(&quot;No mem\n&quot;);
		done();
	}
	seek(mem, nl[1].value, 0);
	read(mem, &amp;nl[1].value, 2);
	seek(mem, nl[0].value, 0);
	read(mem, proc, sizeof proc);
	getdev();
	mtty = ttyn(0);
	uid = getuid() &amp; 0377;
	if(lflg)
	printf(&quot;TTY F S UID   PID PRI ADDR  SZ  WCHAN COMMAND\n&quot;); else
		printf(&quot;TTY  PID COMMAND\n&quot;);
	for (i=0; i&lt;NPROC; i++) {
		if (proc[i].p_stat==0)
			continue;
		if (proc[i].p_ttyp==0) {
			if (xflg==0)
				continue;
			c = &#39;?&#39;;
		} else {
			for(c=0; c&lt;ndev; c++)
			if(devt[c] == proc[i].p_ttyp) {
				c = devc[c];
				goto out;
			}
			seek(mem, proc[i].p_ttyp, 0);
			read(mem, &amp;tty, sizeof tty);
			for(c=0; c&lt;ndev; c++)
			if(devl[c] == tty.t_dev) {
				devt[c] = proc[i].p_ttyp;
				c = devc[c];
				goto out;
			}
			c = &#39;?&#39;;
		out:;
		}
		puid = proc[i].p_uid &amp; 0377;
		if (uid != puid &amp;&amp; aflg==0)
			continue;
		if (lflg || c!=mtty)
			printf(&quot;%c:&quot;, c);
		else
			printf(&quot;  &quot;);
		if (lflg) {
			printf(&quot;%3o %c%4d&quot;, proc[i].p_flag,
				&quot;0SWRIZT&quot;[proc[i].p_stat], puid);
		}
		printf(&quot;%6l&quot;, proc[i].p_pid);
		if (lflg) {
			printf(&quot;%4d%5o%4d&quot;, proc[i].p_pri, proc[i].p_addr,
				(proc[i].p_size+7)&gt;&gt;3);
			if (proc[i].p_wchan)
				printf(&quot;%7o&quot;, proc[i].p_wchan); else
				printf(&quot;       &quot;);
		}
		if (proc[i].p_stat==5)
			printf(&quot; &lt;defunct&gt;&quot;);
		else
			prcom(i);
		printf(&quot;\n&quot;);
	}
	done();
}

getdev()
{
	register struct { int dir_ino; char dir_n[14]; } *p;
	register i, c;
	int f;
	char dbuf[512];
	int sbuf[20];

	f = open(&quot;/dev&quot;);
	if(f &lt; 0) {
		printf(&quot;cannot open /dev\n&quot;);
		done();
	}
	swap = -1;
	c = 0;

loop:
	i = read(f, dbuf, 512);
	if(i &lt;= 0) {
		close(f);
		if(swap &lt; 0) {
			printf(&quot;no swap device\n&quot;);
			done();
		}
		ndev = c;
		return;
	}
	while(i &lt; 512)
		dbuf[i++] = 0;
	for(p = dbuf; p &lt; dbuf+512; p++) {
		if(p-&gt;dir_ino == 0)
			continue;
		if(p-&gt;dir_n[0] == &#39;t&#39; &amp;&amp;
		   p-&gt;dir_n[1] == &#39;t&#39; &amp;&amp;
		   p-&gt;dir_n[2] == &#39;y&#39; &amp;&amp;
		   p-&gt;dir_n[4] == 0 &amp;&amp;
		   p-&gt;dir_n[3] != 0) {
			if(stat(p-&gt;dir_n, sbuf) &lt; 0)
				continue;
			devc[c] = p-&gt;dir_n[3];
			devl[c] = sbuf-&gt;iaddr[0];
			c++;
			continue;
		}
		if(swap &gt;= 0)
			continue;
		if(stat(p-&gt;dir_n, sbuf) &lt; 0)
			continue;
		if((sbuf-&gt;iflags &amp; 060000) != 060000)
			continue;
		if(sbuf-&gt;iaddr[0] == nl[1].value)
			swap = open(p-&gt;dir_n, 0);
	}
	goto loop;
}

setup(p, s)
char *p, *s;
{
	while (*p++ = *s++);
}

prcom(i)
{
	int baddr, laddr, mf;
	register int *ip;
	register char *cp, *cp1;
	int c, nbad;

	baddr = 0;
	laddr = 0;
	if (proc[i].p_flag&amp;SLOAD) {
		laddr = proc[i].p_addr;
		mf = mem;
	} else {
		baddr = proc[i].p_addr;
		mf = swap;
	}
	laddr =+ proc[i].p_size - 8;
	baddr =+ laddr&gt;&gt;3;
	laddr = (laddr&amp;07)&lt;&lt;6;
	seek(mf, baddr, 3);
	seek(mf, laddr, 1);
	if (read(mf, stbuf, 512) != 512)
		return(0);
	for (ip = &amp;stbuf[256]; ip &gt; &amp;stbuf[0];) {
		if (*--ip == -1) {
			cp = ip+1;
			if (*cp==0)
				cp++;
			nbad = 0;
			for (cp1 = cp; cp1 &lt; &amp;stbuf[256]; cp1++) {
				c = *cp1;
				if (c==0)
					*cp1 = &#39; &#39;;
				else if (c &lt; &#39; &#39; || c &gt; 0176) {
					if (++nbad &gt;= 5) {
						*cp1++ = &#39; &#39;;
						break;
					}
					*cp1 = &#39;?&#39;;
				}
			}
			while (*--cp1==&#39; &#39;)
				*cp1 = 0;
			printf(lflg?&quot; %.16s&quot;:&quot; %.64s&quot;, cp);
			return(1);
		}
	}
	return(0);
}

done()
{

	fflush(obuf);
	exit();
}

putchar(c)
{

	putc(c, obuf);
}
