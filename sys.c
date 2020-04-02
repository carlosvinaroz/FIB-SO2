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

#include <entry.h>

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

//int ret_from_fork() {return 0;}

int sys_fork()
{
  int PID;
  struct task_struct *padre_pcb, *hijo_pcb;
  union task_union *padre_task_un, *hijo_task_un;
  page_table_entry *padre_pt, *hijo_pt;

  padre_pcb = current();
  padre_task_un = (union task_union *) padre_pcb;
  padre_pt = get_PT(padre_pcb);

  if (list_empty(&freequeue)) {
  	return -EAGAIN;//no hay tareas libres
  }
  // creates the child process
   struct list_head *fir = list_first(&freequeue);
  hijo_pcb = list_head_to_task_struct(fir);
  list_del(fir);
  hijo_task_un = (union task_union *) hijo_pcb;

  //reservar frames (data+stack) nuevo proceso (d)
  int frames[NUM_PAG_DATA];
  for (int i = 0; i < NUM_PAG_DATA; ++i) {
  	frames[i] = alloc_frame();
  	if (frames[i] == -1) { //No hay mas paginas
  		while (i < 0) { //liberar y devolver error
  			free_frame(frames[i]);
  			i--;
  		}
  		return -ENOMEM; //No hay suficiente mem.
  	}
  }

  //copiar PCB+stack padre a hijo
  copy_data((void *) padre_task_un, (void *) hijo_task_un, sizeof(union task_union));
  allocate_DIR(hijo_pcb);
  hijo_pt = get_PT(hijo_pcb);

  //copiar entradas Tabla Pagina padre
  for (int i = 0;i < NUM_PAG_CODE; ++i) {
  	hijo_pt[PAG_LOG_INIT_CODE + i].entry = padre_pt[PAG_LOG_INIT_CODE + i].entry;
  }
  //copiar paginas padre
  int pag = NUM_PAG_KERNEL + NUM_PAG_DATA + NUM_PAG_CODE; //1ra pag libre
  for (int i = 0; i < NUM_PAG_DATA; ++i) {
    //ss temporal
    set_ss_pag(hijo_pt,PAG_LOG_INIT_DATA + i, frames[i]);


    set_ss_pag(padre_pt, pag, frames[i]);
    copy_data((void *) ((PAG_LOG_INIT_DATA + i) << 12),(void *)  ((pag) << 12), PAGE_SIZE);
    del_ss_pag(padre_pt, pag);
    //Flush TLB
    set_cr3(padre_pcb->dir_pages_baseAddr);
  }
  PID = get_new_PID();
  hijo_pcb->PID = PID;


  //PREPARAR stack (task_switch)
  
  hijo_task_un->stack[1024-17] = (DWord) &ret_from_fork;
  hijo_task_un->stack[1024-18] = 0xCACA;
  hijo_task_un->task.Kernel_esp_task = (DWord) &hijo_task_un->stack[1024-18];

  //inicializar stats hijo
  inicializar_stats(&hijo_pcb->proc_stats);

  list_add_tail(&hijo_pcb->list, &readyqueue);
  return PID;
}


void sys_exit()
{
	struct task_struct *ts = current();
	free_user_pages(ts);
	ts->PID= -1;
	list_add_tail(&ts->list, &freequeue);
  	//update_process_state_rr(ts, &freequeue);
	sched_next_rr();
}

int sys_write(int fd, char *buffer, int size) {
	//COMPROBAR ERROR
	int fd_error = check_fd(fd, ESCRIPTURA);
	if (fd_error != 0) return fd_error;
	if(buffer == NULL) return -EFAULT;
	if (size < 0) return -EINVAL;
	int i = 0;
	int tam_size = 256;
	char buff[tam_size];
	while(size > tam_size) {
		copy_from_user(buffer+i,buff,tam_size);
		i += sys_write_console(buff,tam_size);
		size = size - tam_size;
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

int sys_get_stats(int pid, struct stats *st) {
  if(pid < 0) return -EINVAL;
  if(!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT;

  union task_union *t_un;
  int encontrado = 0;
  if (current()->PID == pid) t_un = (union task_union*) current();
  else {
    for (int i = 0; i < NR_TASKS; i++) {
      if(task[i].task.PID == pid) { //encontrado
        t_un = (union task_union *) &task[i];
        encontrado = 1;
      }
    }
    if (!encontrado) return -ESRCH; //no encontrado	
  }
  copy_to_user(&t_un->task.proc_stats, st, sizeof(struct stats));
  return 0;
}

