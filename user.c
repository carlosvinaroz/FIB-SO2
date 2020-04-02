#include <libc.h>

char buff[24];
int pid;

int add(int par1,int par2) {
  return par1+par2;
}

int addASM(int par1, int par2);

void prueba_suma() {
  int test;
  test = addASM(1, 17);
  char buff[20];
  itoa(test,buff);
  write(1,buff,strlen(buff));
}

void prueba_write() {
	char buff[40] = "\n Prueba write TODO ok\n";
  //if(write(1,buff,-2) == -1 ) perror(); //PRUEBA PARA ver que salta bien el perror()
  if(write(1,buff,strlen(buff)) == -1 ) perror();
  write(1,"\n",strlen("\n"));
}

void prueba_time() {
  int tiempo = gettime();
  char buff[50];
  itoa(tiempo,buff);
  write(1,buff,strlen(buff));
  write(1,"\n",strlen("\n"));
}

void prueba_getpid() {
  int pid = getpid();
  if (pid == -1) perror();
  itoa(pid,buff);
  if(write(1,"\n Tu PID: ",strlen("\n Tu PID: ")) == -1 ) perror();
  if(write(1,buff,strlen(buff)) == -1 ) perror();
}

void prueba_fork() {
  int pd = fork();
  char pid[64];
  itoa(pd,pid);
  if(pd == 0) { //hijos
    write(1,pid, strlen(pid));
	  write(1,", HIJO\n", strlen(", HIJO\n"));
    write(1,"HIJO EXIT\n", strlen("HIJO EXIT\n"));
    exit();
    write(1,"\nHIJO MUERTO", strlen("\nHIJO MUERTO"));
  }
  else {
    write(1,pid, strlen(pid));
	  write(1,", PADRE\n", strlen(", PADRE\n"));
    write(1,"PADRE EXIT\n", strlen("PADRE EXIT\n"));
    exit();
    write(1,"\nPADRE MUERTO", strlen("\nPADRE MUERTO"));
  }
}

void prueba_fork2() {
  int pd = fork();
  fork();
  fork();
  char pid[64];
  itoa(pd,pid);
  if(pd == 0) { //hijos
    write(1,pid, strlen(pid));
    write(1,", HIJO\n", strlen(", HIJO\n"));
    write(1,"HIJO EXIT\n", strlen("HIJO EXIT\n"));
    exit();
    write(1,"\nHIJO MUERTO", strlen("\nHIJO MUERTO"));
  }
  else {
    write(1,pid, strlen(pid));
    write(1,", PADRE\n", strlen(", PADRE\n"));
    write(1,"PADRE EXIT\n", strlen("PADRE EXIT\n"));
    exit();
    write(1,"\nPADRE MUERTO", strlen("\nPADRE MUERTO"));
  }
}



int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  //prueba_suma();
  //prueba_write();
  //prueba_time();
  //prueba_getpid();
  //prueba_fork();
  //prueba_fork2();
  runjp(); //juego de pruebas
  while(1) {
  	//prueba_time();
  }
}
