/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>
#include <system.h>


#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}

int sys_write(int fd, char* buffer, int size)
{
	if (check_fd(fd, ESCRIPTURA) != 0) return check_fd(fd, ESCRIPTURA);
	if(buffer == NULL) return -EFAULT;
	if (size < 0) return -EINVAL;

	int i = 0;
	char buff[256];
	while(size > 256) {
		copy_from_user(buffer+i,buff,256);
		i += sys_write_console(buff,256);
		size -= 256;
	}
	if (size > 0) { //falta escribir un fragmento del buffer
		copy_from_user(buffer+i,buff,size);
		i += size;
		sys_write_console(buff,size);
	}
	return i;
}

int sys_gettime() {
	return zeos_ticks;
}
