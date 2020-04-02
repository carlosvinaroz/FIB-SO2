/*
 * entry.h - Definició del punt d'entrada de les crides al sistema
 */

#ifndef __ENTRY_H__
#define __ENTRY_H__

void keyboard_handler();//

void system_call_handler();//
void syscall_handler_sysenter();

void writeMSR(int reg, int val);

void clock_handler();

int ret_from_fork();

#endif  /* __ENTRY_H__ */
