#include <asm.h>

ENTRY(write) #int write (int fd, char * buffer, int size)
	pushl %ebp
	movl %esp, %ebp
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %esi
	movl 8(%ebp), %ebx  #fd
	movl 12(%ebp), %ecx #buff
	movl 16(%ebp), %edx #size
	movl $4, %eax #identificador de la system call

	leal write_ret, %esi #dir de retorno
	pushl %esi

	pushl %ebp #preparar stack syscall
	movl %esp, %ebp

	sysenter

write_ret:
	movl %ebp, %esp
	popl %ebp
	test %eax, %eax #comprobar si hay algun error con el resultado
	jns end_w
	neg %eax
	movl %eax, errno
	movl $-1, %eax # mostrar -1 en caso de error

end_w:
	popl %esi
	popl %ebx
	popl %ecx
	popl %edx
	movl %ebp, %esp
	popl %ebp
	ret

#############################################################################

ENTRY(gettime)
	pushl %ebp
	movl %esp, %ebp
	movl $10, %eax #identificador syscall gettime

	pushl %edx
	pushl %ecx
	pushl %esi

	leal r_time, %esi
	pushl %esi
	pushl %ebp
	movl %esp, %ebp
	sysenter

r_time:
	movl %ebp, %esp
	popl %ebp
	test %eax, %eax #comprobar si hay algun error con el resultado
	jns end_time
	neg %eax
	movl %eax, errno
	movl $-1, %eax # mostrar -1 en caso de error

end_time:
	popl %esi #leal r_time
	popl %esi
	popl %ecx
	popl %edx
	movl %ebp, %esp
	popl %ebp
	ret

#############################################################################

ENTRY(getpid)
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx
	pushl %esi
	movl $20, %eax #identificador syscall getpid
	leal r_getpid, %esi
	pushl %esi

	pushl %ebp
	movl %esp, %ebp
	sysenter
r_getpid:
	movl %ebp, %esp
	popl %ebp
	test %eax, %eax #comprobar si hay algun error con el resultado
	jns end_getpid
	neg %eax
	movl %eax, errno
	movl $-1, %eax # mostrar -1 en caso de error
end_getpid:
	popl %esi #r_getpid
	popl %esi
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	ret

#############################################################################

ENTRY(fork)
  	pushl %ebp
  	movl %esp, %ebp
  	pushl %ecx
  	pushl %edx
  	pushl %esi
	movl $2, %eax
  	leal r_fork, %esi
  	pushl %esi

  	pushl %ebp
  	movl %esp, %ebp

  	sysenter
r_fork:
	movl %ebp, %esp
	popl %ebp

  	popl %esi 

	test %eax, %eax #comprobar si hay algun error con el resultado
	jns end_fork
	neg %eax
	movl %eax, errno
	movl $-1, %eax # mostrar -1 en caso de error

end_fork:
  	popl %esi
  	popl %edx
  	popl %ecx
  	movl %ebp, %esp
  	popl %ebp
  	ret



#############################################################################

ENTRY(exit)
	pushl %ebp
	movl %esp, %ebp
	pushl %ecx
	pushl %edx
	pushl %esi
	movl $1, %eax #identificador syscall exit
	leal r_exit, %esi
	pushl %esi

	pushl %ebp
	movl %esp, %ebp
	sysenter
r_exit:
	movl %ebp, %esp
	popl %ebp
	test %eax, %eax #comprobar si hay algun error con el resultado
	jns end_exit
	neg %eax
	movl %eax, errno
	movl $-1, %eax # mostrar -1 en caso de error
end_exit:
	popl %esi #r_exit
	popl %esi
	popl %edx
	popl %ecx
	movl %ebp, %esp
	popl %ebp
	ret


#############################################################################

ENTRY(get_stats)
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %edx
	pushl %ecx
	pushl %esi
	movl $35, %eax #identificador syscall get_stats

	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx

	leal r_exit, %esi
	pushl %esi

	pushl %ebp
	movl %esp, %ebp
	sysenter
r_getstats:
	movl %ebp, %esp
	popl %ebp
	test %eax, %eax #comprobar si hay algun error con el resultado
	jns end_getstats
	neg %eax
	movl %eax, errno
	movl $-1, %eax # mostrar -1 en caso de error
end_getstats:
	popl %esi #r_getstats
	popl %esi
	popl %ecx
	popl %edx
	popl %ebx

	movl %ebp, %esp
	popl %ebp
	ret


#############################################################################