IEH3nodg (ch)
char ch;
{
if (IEH3digt(ch)) return (0);
switch (ch)
	{
	case &#39;E&#39;:
	case &#39;e&#39;:
	case &#39;.&#39;: case &#39;+&#39;: case &#39;-&#39;:
		return (0);
	}
return (1);
}
