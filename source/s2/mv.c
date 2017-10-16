/*
mv [-d] file1 file2

unlink file2
link file1 file2
unlink file1
*/
int stbuf[42];
struct sbuf {
int dev;
int inum;
int imode;
char nlink;
char uid;
char gid;
char siz0;
char siz1;
int addr[8];
int adate[2];
int mdate[2];
};
char strbuf[70];

main(argc,argv)
int argc;
char *argv[];
{
char **argp;
char *argp1, *argp2, *argp3, *argp4;
char *p, *p1, *p2;
char place[100];
int i;
int status;
int b;
argp = argv;
/*
	check for correct number
	of arguments
*/
if(argc != 3){
	write(1,&quot;Usage: mv name1 name2\n&quot;,22);
	exit();
}
/*
	is there anything to do?
*/
argp3 = argp[1];
argp4 = argp[2];
if(stat(argp[1], stbuf) &lt; 0){
	write(1,&quot;Source file non-existent\n&quot;,25);
	exit();
}
/*
	yes, there is a source.
	check whether file or directory
*/
if((stbuf[0].imode &amp; 060000) == 040000){
/*
	The source is a directory, so
	we do lots of checking and
	messing around so as not to get
	into trouble.  This patch of
	code contains administrative
	policies rather than system
	restrictions.
*/
	if(stat(argp[2], stbuf) &gt;= 0){
		write(1,&quot;Directory target exists.\n&quot;,25);
		exit();
	}
	argp1 = argp[1];
	argp2 = argp[2];
	while(*argp1 == *argp2){
		argp1++;
		if(*argp2++ == 0){
			write(1,&quot;???\n&quot;,4);
			exit();
		}
	}
	while(*argp1)if(*argp1++ == &#39;/&#39;){
		write(1,&quot;Directory rename only\n&quot;,22);
		exit();
	}
	while(*argp2)if(*argp2++ == &#39;/&#39;){
		write(1,&quot;Directory rename only\n&quot;,22);
		exit();
	}
	if(*--argp1 == &#39;.&#39;){
		write(1,&quot;values of B will give rise to dom!\n&quot;,37);
		exit();
	}
}else{
/*
	the source is a file.
*/
	setuid(getuid());
	if(stat(argp4, &amp;stbuf[2]) &gt;= 0){
		if((stbuf[2].imode &amp; 060000) == 040000){
			argp2 = strbuf;
			while(*argp2++ = *argp4++);
			argp2[-1] = &#39;/&#39;;
			argp4 = argp[1];
			argp1 = argp[1];
			while(*argp4)
				if(*argp4++ == &#39;/&#39;)
					argp1 = argp4;
			while(*argp2++ = *argp1++);
			argp4 = strbuf;
		}
		if(stat(argp4, &amp;stbuf[2]) &gt;= 0){
			if((stbuf[0]==stbuf[2]) &amp;&amp; (stbuf[1]==stbuf[3])){
				write(1,&quot;Files are identical.\n&quot;,21);
				exit();
			}
			if((getuid()&amp;0377) == stbuf[2].uid)
				b = 0200; else
			if((getgid()&amp;0377) == stbuf[2].gid)
				b = 020; else
				b = 02;
			if((stbuf[2].imode &amp; b) == 0) {
				printf(&quot;%s: %o mode &quot;, argp4,
					stbuf[2].imode &amp; 07777);
				i = b = getchar();
				while(b != &#39;\n&#39; &amp;&amp; b != &#39;\0&#39;)
					b = getchar();
				if(i != &#39;y&#39;)
					exit();
			}
			if(unlink(argp4) &lt; 0){
				write(1,&quot;Cannot remove target file.\n&quot;,27);
				exit();
			}
		}
	}
}
if(link(argp3, argp4) &lt; 0){
	i = fork();
	if(i == -1){
		write(1,&quot;Try again.\n&quot;,11);
		exit();
	}
	if(i){
		while(wait(&amp;status) != i);
	}else{
		p = place;
		p1 = p;
		while(*p++ = *argp3++);
		p2 = p;
		while(*p++ = *argp4++);
		execl(&quot;/bin/cp&quot;,&quot;cp&quot;, p1, p2, 0);
		write(1, &quot;no cp\n&quot;, 6);
		exit(1);
	}
	if((status &amp; 0377) != 0){
		write(1,&quot;?\n&quot;, 2);
		exit();
	}
	if(status != 0) exit();
}
if(unlink(argp3) &lt; 0){
	write(1,&quot;Cannot unlink source file.\n&quot;,27);
	exit();
}
}

putchar(c)
{
	write(1, &amp;c, 1);
}

getchar()
{
	char c;

	if(read(0, &amp;c, 1) != 1) return(0);
	return(c);
}
