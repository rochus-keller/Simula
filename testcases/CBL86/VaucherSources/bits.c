/* bits.c:

  Minimal set C routines to do systems work in SIMULA
  ----------------------------------------------------
  J. Vaucher  (v1.0  sept 1996)
		(v1.1  august 1996)  
		
  - c_peek32(adr)	:	gets WORD from ADR
  - c_peek16(adr)	:	gets HALF from ADR
  - c_peek8(adr)	:	gets BYTE from ADR
  - c_poke32(adr, val): put WORD Val at ADR 
  - c_poke16(adr, val): put HALF Val at ADR 
  - c_poke8(adr, val) : put BYTE Val at ADR 

  - c_bit_set( Word,n )	:	returns Word with bit N set to 1
  - c_bit_clear( Word,n ):	returns Word with bit N set to 0
  - c_bit_in( Word,n )	:	returns TRUE if bit N is "1"
  - getbits( x, p, n):	returns N bits from X with rightmost at 
                        position P
                        (p=0 is furthest right position) 

  - c_and( W1, W2 ):	returns W1 and W2
  - c_or ( W1, W2 ):	returns W1 or W2
  - c_xor( W1, W2 ):	returns W1 xor W2
  - c_not( W1 ):		returns not W1
  - c_sub( W1, W2 ):	returns W1 and not W2

  - c_subverttype( W1 ):	returns 4 byte value as INTEGER
                         - used for REF or REALS

------------------------------------------------------ */

extern char ** environ;

char ** envp (void){
   return environ;
}

/* -----------------------------
    PEEK and POKE 
   -----------------------------  */
   
long c_peek32(adr)	long * adr;
	{  return( *adr);  }

int  c_peek16(adr)	short * adr;
	{  unsigned short X;
	   X =  *adr;
	   return( X );  }

int c_peek8(adr)	char * adr;
{       
    return  *adr & 0xFF ;
}

void c_poke32(adr,  val)
  long * adr;
  long   val;
{       adr = (long *) ( (int) adr & ~03); 
	   *adr = val;  
}

void  c_poke16(adr,val)
 short * adr;
 short   val;
{  	    adr = (  short *) ( (int) adr & ~01); 
	   *adr = val;  
}

void c_poke8(adr,val)
 char *adr;
 char val;
{ 
	*adr = val;  
}

/* ==============================
   BITACCESS procedures:
  ===============================  */
   
long c_bit_set( S, n)
	long S;
	int	n;
{ return( S | (1<<n) ) ;
}

long c_bit_clear( S, n)
	long S;
	int	n;
{ return( S & ~(1<<n) ) ;
}

int c_bit_in( S, n)
	long S;
	int	n;
{ 	return( (S & (1<<n)) != 0 ) ;
}

unsigned long getbits( unsigned long x, int p, int n)
{
	return (x >> p) & ~(~0 << n);
}

/* ===============================
    BIT by BIT BOOLEAN OPERATIONS
   ===============================  */
   
long c_and( S, T)
	long S,T;
{ return( S & T ) ;
}

long c_or( S, T)
	long S,T;
{ return( S | T ) ;
}

long c_xor( S, T)
	long S,T;
{ return( S ^ T ) ;
}

long c_not( S)
	long S;
{ return( ~S ) ;
}

long c_sub( S, T)
	long S,T;
{ return( S & ~T ) ;
}

long subverttype(x)
	long x;
{  return(x);  }

/* ===============================
     UNIX DIRECTORY ACCESS
   ===============================  */

#include <dirent.h>
#include <sys/stat.h>
   
char *nameofentry(entry)
     int entry;
{
  return (char *) ((struct dirent *)entry)->d_name;
}

int entryisdir(fname)
char *fname;
{
  struct stat info;
  if ( (!stat(fname, &info)) && (info.st_mode & S_IFDIR))  return 1;
  return 0;
}

int getsize(fname)
     char *fname;
{
  struct stat info;
  if  (!stat(fname, &info)) return info.st_size;
  return 0;
}

int getdatelastmod(fname,date)
     char *fname,*date;
{
  struct stat info;
  if  (!stat(fname, &info))
    return (int) strcpy(date,ctime(&(info.st_mtime)));

  return 0;
}




