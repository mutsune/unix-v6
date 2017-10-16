system(str)
 char *str;
{
int status;
  if(fork() == 0)
    execl(&quot;/bin/sh&quot;, &quot;sh&quot;, &quot;-c&quot;, str, 0);
  wait(&amp;status);
  }
