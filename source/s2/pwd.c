char dot[] &quot;.&quot;;
char dotdot[] &quot;..&quot;;
char root[] &quot;/&quot;;
char name[512];
int file, off -1;
struct statb {int devn, inum, i[18];}x;
struct entry { int jnum; char name[16];}y;

main() {
	int n;

loop0:
	stat(dot, &amp;x);
	if((file = open(dotdot,0)) &lt; 0) prname();
loop1:
	if((n = read(file,&amp;y,16)) &lt; 16) prname();
	if(y.jnum != x.inum)goto loop1;
	close(file);
	if(y.jnum == 1) ckroot();
	cat();
	chdir(dotdot);
	goto loop0;
}
ckroot() {
	int i, n;

	if((n = stat(y.name,&amp;x)) &lt; 0) prname();
	i = x.devn;
	if((n = chdir(root)) &lt; 0) prname();
	if((file = open(root,0)) &lt; 0) prname();
loop:
	if((n = read(file,&amp;y,16)) &lt; 16) prname();
	if(y.jnum == 0) goto loop;
	if((n = stat(y.name,&amp;x)) &lt; 0) prname();
	if(x.devn != i) goto loop;
	x.i[0] =&amp; 060000;
	if(x.i[0] != 040000) goto loop;
	if(y.name[0]==&#39;.&#39;)if(((y.name[1]==&#39;.&#39;) &amp;&amp; (y.name[2]==0)) ||
				(y.name[1] == 0)) goto pr;
	cat();
pr:
	write(1,root,1);
	prname();
}
prname() {
	if(off&lt;0)off=0;
	name[off] = &#39;\n&#39;;
	write(1,name,off+1);
	exit();
}
cat() {
	int i, j;

	i = -1;
	while(y.name[++i] != 0);
	if((off+i+2) &gt; 511) prname();
	for(j=off+1; j&gt;=0; --j) name[j+i+1] = name[j];
	off=i+off+1;
	name[i] = root[0];
	for(--i; i&gt;=0; --i) name[i] = y.name[i];
}
