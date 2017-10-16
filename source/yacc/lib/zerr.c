extern int yyline;  /* input line number */

yyerror(s) char *s; {
  extern int yychar;
  extern char *yysterm[];
  printf(&quot;\n%s&quot;, s );
  if( yyline ) printf(&quot;, line %d,&quot;, yyline );
  printf(&quot; on input: &quot;);
  if( yychar &gt;= 0400 ) printf(&quot;%s\n&quot;, yysterm[yychar-0400] );
  else switch ( yychar ) {
    case &#39;\t&#39;: printf( &quot;\\t\n&quot; ); return;
    case &#39;\n&#39;: printf( &quot;\\n\n&quot; ); return;
    case &#39;\0&#39;: printf( &quot;$end\n&quot; ); return;
    default: printf( &quot;%c\n&quot; , yychar ); return;
    }
  }
