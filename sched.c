/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <utils.h> //get_ticks
#include<entry.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

//#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
//#endif

extern struct list_head blocked;
//extern struct list_head freequeue;

int ultimo_PID_assig; //


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");
	printk ("Estado IDLE\n");
	while(1)
	{
	;
	}
}

void inicializar_stats(struct stats *s) {
	s->user_ticks = 0;
	s->system_ticks = 0;
	s->blocked_ticks = 0;
	s->ready_ticks = 0;
	s->elapsed_total_ticks = get_ticks();
	s->total_trans = 0;
	s->remaining_ticks = 0;
}


void init_idle (void)
{
	struct list_head *fir = list_first(&freequeue);
	list_del(fir);
	struct task_struct *pcb = list_head_to_task_struct(fir);
	pcb->PID = get_new_PID();
	allocate_DIR(pcb);
	pcb->quantum = 5;
	pcb->estado = ST_READY;
	//inicializar stats
	inicializar_stats(&pcb->proc_stats);
	union task_union *task_un;
	task_un = (union task_union *) pcb;
	task_un->stack[1023] = (DWord) &cpu_idle; //1023
	task_un->stack[1022] = 0xCACA; //1022 -> KERNEL_STACK_SIZE-2 
	pcb->Kernel_esp_task = (DWord) &task_un->stack[1022];

	idle_task = pcb;
}

void init_task1(void)
{
	struct list_head *fir = list_first(&freequeue);
	list_del(fir);
	struct task_struct *pcb = list_head_to_task_struct(fir);
	pcb->PID = get_new_PID();
	pcb->quantum = 5;
	quantum_restante = pcb->quantum;
	pcb->estado = ST_RUN;
	pcb->proc_stats.remaining_ticks = pcb->quantum;
	//inicializar stats
	inicializar_stats(&pcb->proc_stats);

	allocate_DIR(pcb);
	set_user_pages(pcb);


	union task_union *task_un = (union task_union *) pcb;
	tss.esp0 = (DWord) &task_un->stack[1024]; //KERNEL_STACK_SIZE
	//tss.esp0 = (DWord) pcb+4096;
	writeMSR(0x175,tss.esp0);
	set_cr3(pcb->dir_pages_baseAddr);	
}

void init_sched()
{
	ultimo_PID_assig = -1;
	INIT_LIST_HEAD(&freequeue);
	for (int i = 0; i < NR_TASKS; i++) {
		task[i].task.PID = -1;
		list_add_tail(&(task[i].task.list), &freequeue);
	}
	INIT_LIST_HEAD(&readyqueue);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

int get_new_PID() {
	return ++ultimo_PID_assig;
}

void inner_task_switch(union task_union *new) {
	tss.esp0 = (DWord) new->task.Kernel_esp_task;
	writeMSR(0x175, tss.esp0);
	set_cr3(new->task.dir_pages_baseAddr);
	current()->Kernel_esp_task = getEsp();
	setEsp(new->task.Kernel_esp_task);
}


//COSAS PLANIFICADOR

void schedule() {
	update_sched_data_rr();
	if(needs_sched_rr()) {
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}


void update_sched_data_rr() {
	--quantum_restante;
}

int needs_sched_rr() {
	int val = 0;
	if(quantum_restante == 0 && !list_empty(&freequeue)) val = 1;
	if(quantum_restante == 0) quantum_restante = get_quantum(current());
	return val;
}

int get_quantum(struct task_struct *t) {
	return t->quantum;
}
void set_quantum(struct task_struct *t, int new_quantum) {
	if(new_quantum > 0) t->quantum = new_quantum;
}


//funcion auxiliar solo para saber estados de los procesos
void printar_estados (const enum state_t estado_ant,const enum state_t estado) {
	printk("Estado_ant: ");
	if (estado_ant == ST_RUN) printk("RUN\n");
	else if (estado_ant == ST_READY) printk("READY\n");
	else if (estado_ant == ST_BLOCKED) printk("BLOCKED\n");
	printk("Estado_final: ");
	if (estado == ST_RUN) printk("RUN\n");
	else if (estado == ST_READY) printk("READY\n");
	else if (estado == ST_BLOCKED) printk("BLOCKED\n");
	printk("\n");
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
	list_add_tail(&t->list, dest);
	//enum state_t estado_ant = t->estado;
	if(dest != NULL) {
		if(dest != &readyqueue) {
			t->estado = ST_BLOCKED;
			stats_act_runsystem_ready(t);
		}
		else {
			t->estado = ST_READY;
			stats_act_runsystem_ready(t);
		}
	}
	else {
		t->estado = ST_RUN;
		//t->proc_stats.total_trans++;
	}
	//printar_estados(estado_ant,t->estado);
}


void sched_next_rr() {
	struct list_head *siguiente;
	struct task_struct *t_sig;
	if(list_empty(&readyqueue)) task_switch((union task_union *) idle_task);
	else {
		siguiente = list_first(&readyqueue);
		t_sig = list_head_to_task_struct(siguiente);
		list_del(siguiente);
		quantum_restante = get_quantum(t_sig);
		t_sig->estado = ST_RUN;
		t_sig->proc_stats.remaining_ticks = t_sig->quantum;
		t_sig->proc_stats.total_trans++;
		//current()->proc_stats.total_trans++;
		stats_act_runsystem_ready(current());
		stats_act_ready_runsystem(t_sig);
		task_switch((union task_union *) t_sig);
	}
}

//actualizar stats
void stats_act_user_system(struct task_struct *pcb) {//a)
	pcb->proc_stats.user_ticks += (get_ticks() - pcb->proc_stats.elapsed_total_ticks);
	pcb->proc_stats.elapsed_total_ticks = get_ticks();
}

void current_act_user_system() {
	current()->proc_stats.user_ticks += (get_ticks() - current()->proc_stats.elapsed_total_ticks);
	current()->proc_stats.elapsed_total_ticks = get_ticks();
}


void stats_act_system_user(struct task_struct *pcb){ //b)
	pcb->proc_stats.system_ticks += (get_ticks() - pcb->proc_stats.elapsed_total_ticks);
	pcb->proc_stats.elapsed_total_ticks = get_ticks();
}

void current_act_system_user(){ 
	current()->proc_stats.system_ticks += (get_ticks() - current()->proc_stats.elapsed_total_ticks);
	current()->proc_stats.elapsed_total_ticks = get_ticks();
}

void stats_act_runsystem_ready(struct task_struct *pcb){//c)
	pcb->proc_stats.system_ticks += (get_ticks() - pcb->proc_stats.elapsed_total_ticks);
	pcb->proc_stats.elapsed_total_ticks = get_ticks();
}

void current_act_runsystem_ready(){
	current()->proc_stats.system_ticks += (get_ticks() - current()->proc_stats.elapsed_total_ticks);
	current()->proc_stats.elapsed_total_ticks = get_ticks();
}


void stats_act_ready_runsystem(struct task_struct *pcb){//d)
	pcb->proc_stats.ready_ticks += (get_ticks() - pcb->proc_stats.elapsed_total_ticks);
	pcb->proc_stats.elapsed_total_ticks = get_ticks();
	pcb->proc_stats.total_trans++;
}


void current_act_ready_runsystem(){
	current()->proc_stats.ready_ticks += (get_ticks() - current()->proc_stats.elapsed_total_ticks);
	current()->proc_stats.elapsed_total_ticks = get_ticks();
	current()->proc_stats.total_trans++;
}


