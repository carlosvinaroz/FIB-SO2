/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>
#include <errno.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror(void) {
	char buffer[20] = "Error ";
	write(1,buffer,strlen(buffer));
	char bf2[20];
	itoa(errno,bf2);
	write(1,bf2,strlen(bf2));
	if(errno == ENOSYS) {
		char b_error[200] = " , Function not implemented\n";
		write(1,b_error,strlen(b_error));
	}
	else if(errno == EFAULT) {
		char b_error[200] = " , (buffer NULL ? )\n";
		write(1,b_error,strlen(b_error));
	}
	else if(errno == EINVAL) {
		char b_error[200] = " , (size < 0 ?)\n";
		write(1,b_error,strlen(b_error));
	}
}
