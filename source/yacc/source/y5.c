/* fake portable I/O routines, for those
    sites so backward as to not have the
     port. library */

int cin, cout;
extern int fin, fout;

copen( s, c ) char *s; {
  int f;

  if( c == &#39;r&#39; ){
    fin = f = open( s, 0 );
    }

  else if( c == &#39;a&#39; ){
    f = open( s, 1 );
    seek( f, 0, 2 );
    }

  else {  /* c == w */
    f = creat( s, 0666 );
    }

  return( f );
  }

cflush(x){ /* fake! sets file to x */
  flush();
  fout = x;
  }

system(){
  error( &quot;The function \&quot;system\&quot; is called&quot; );
  }

cclose(i){
  close(i);
  }

cexit(i){
  flush();
  exit();
  }
