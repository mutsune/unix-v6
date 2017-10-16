#
/*
 * C library -- alloc/free
 */

#define	logical	char *

struct fb {
	logical	size;
	char	*next;
};

int	freelist[] {
	0,
	-1,
};
logical	slop	2;

alloc(asize)
logical asize;
{
	register logical size;
	register logical np;
	register logical cp;

	if ((size = asize) == 0)
		return(0);
	size =+ 3;
	size =&amp; ~01;
	for (;;) {
		for (cp=freelist; (np= cp-&gt;next) != -1; cp=np) {
			if (np-&gt;size&gt;=size) {
				if (size+slop &gt;= np-&gt;size) {
					cp-&gt;next = np-&gt;next;
					return(&amp;np-&gt;next);
				}
				cp = cp-&gt;next = np+size;
				cp-&gt;size = np-&gt;size - size;
				cp-&gt;next = np-&gt;next;
				np-&gt;size = size;
				return(&amp;np-&gt;next);
			}
		}
		asize = size&lt;1024? 1024: size;
		if ((cp = sbrk(asize)) == -1) {
			return (-1);
		}
		cp-&gt;size = asize;
		free(&amp;cp-&gt;next);
	}
}

free(aptr)
char *aptr;
{
	register logical ptr;
	register logical cp;
	register logical np;

	ptr = aptr-2;
	cp = freelist;
	while ((np = cp-&gt;next) &lt; ptr)
		cp = np;
	if (ptr+ptr-&gt;size == np) {
		ptr-&gt;size =+ np-&gt;size;
		ptr-&gt;next = np-&gt;next;
		np = ptr;
	} else
		ptr-&gt;next = np;
	if (cp+cp-&gt;size == ptr) {
		cp-&gt;size =+ ptr-&gt;size;
		cp-&gt;next = ptr-&gt;next;
	} else
		cp-&gt;next = ptr;
}
