copen (filename, type)
char *filename;
int type;
{
int fn;
switch (type)
	{
	case &#39;r&#39;: type = 0;
	case 0:	  fn = open(filename,0);
		break;
	case &#39;a&#39;: type = 2;
	case 2:	if ((fn=open(filename,1))&gt;=0)
			break;	/* courtesy of sny */
	/* append defaults to write if file missing */
	case &#39;w&#39;: type = 1;
	case 1: fn = creat(filename,0666);
		break;
	default: IEH3err(&quot;copen: bad file %s&quot;,filename);
	}
if (fn &gt;= 0)
	IEH3mbuf(fn,type);
return(fn);
}
