/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <stats.h>
#include <mm_address.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

struct task_struct { //PCB
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  page_table_entry * dir_pages_baseAddr;
  struct list_head list;
  DWord Kernel_esp_task;
  int quantum;
  enum state_t estado;
  struct stats proc_stats;
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};


extern union task_union task[NR_TASKS]; /* Vector de tasques */
struct list_head freequeue;
struct list_head readyqueue;
struct task_struct *idle_task;

int quantum_restante;

#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

/* Inicialitza les dades del proces inicial */
void init_task1(void);

void init_idle(void);

void init_sched(void);

struct task_struct * current();

void task_switch(union task_union*t);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();


//NUEVAS FUNCIONES Y COSAS
int get_new_PID();

void inner_task_switch(union task_union *new);
int getEsp();
void setEsp(int);

int get_quantum(struct task_struct *t);
void set_quantum(struct task_struct *t, int new_quantum);

void schedule();

void inicializar_stats(struct stats *s);

//actualizar stats
void stats_act_user_system(struct task_struct *pcb); //a)
void current_act_user_system();

void stats_act_system_user(struct task_struct *pcb); //b)
void current_act_system_user();

void stats_act_runsystem_ready(struct task_struct *pcb);//c)
void current_act_runsystem_ready();

void stats_act_ready_runsystem(struct task_struct *pcb);//d)
void current_act_ready_runsystem();


#endif  /* __SCHED_H__ */
