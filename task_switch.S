#include <asm.h>

ENTRY(task_switch)
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
	pushl %ebx
	pushl 8(%ebp) #push new
	call inner_task_switch

	addl $4, %esp
	popl %ebx
	popl %edi
	popl %esi
	movl %ebp, %esp
	popl %ebp

	ret

ENTRY(getEsp)
	movl %ebp, %eax
	ret

ENTRY (setEsp)
	movl 4(%esp), %esp
	popl %ebp
	ret

