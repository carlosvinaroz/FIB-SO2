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
	popl %esp
	test %eax, %eax #comprobar si hay algun error con el resultado
	jns end_time
	neg %eax
	movl %eax, errno
	movl $-1, %eax # mostrar -1 en caso de error

end_time:
	popl %esi
	popl %ecx
	popl %edx
	movl %ebp, %esp
	popl %ebp
	ret

