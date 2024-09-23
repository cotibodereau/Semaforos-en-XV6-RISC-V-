#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

#define NSEMAFOROS 100

//defino la estructura del semaforo
struct semaforo {
  int value;              // cantidad de recursos q el semaforo controla
  struct spinlock lock;   // spinlock encargado de proteger el acceso al semaforo
};

struct semaforo arreglo_semaforos[NSEMAFOROS]; //arreglo donde se ubican los semaforos (no se si esta bien dicho asi lo hice con la ayuda)

//se inicializa cada semaforo con value = -1 para indicar q aun no ha sido abierto y a su vez se le inicia un spinlock
void 
init_arr() 
{
  for (unsigned int i = 0; i < NSEMAFOROS; ++i) {
    arreglo_semaforos[i].value = -1;
    initlock(&(arreglo_semaforos[i].lock), "semaforo");
    }
}
      
int
sem_open (int sem, int value)
{
  //controlamos q el id del semaforo "sem" sea un entero entre 0 y NSEMAFOROS
  if (sem < 0 || sem > NSEMAFOROS){
    printf ("Error, id del semaforo invalido");
    return 0;
  } 
  
  //se bloque el lock del semaforo para evitar condiciones de carrera 
  acquire(&(arreglo_semaforos[sem].lock));
  
  //si el semaforo no esta abierto, lo abrimos 
  if (arreglo_semaforos[sem].value == -1) {
    arreglo_semaforos[sem].value = value; 
    release(&(arreglo_semaforos[sem].lock));  //se desbloque el spinlock del semaforo ya q no se utilizara mas 
    return 1;
  }
 //si ya esta abierto e intentamos volver a abrirlo, se produce un error 
  else {
    release(&(arreglo_semaforos[sem].lock));
    printf ("Error, semaforo ya abierto");
    return 0;    
  }
}


