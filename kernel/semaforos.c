#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

#define NSEMAFOROS 100

// defino la estructura del semaforo
struct semaforo
{
  int value;            // cantidad de recursos que el semaforo controla
  int semActivo;        // Indica con 1 si el semaforo ya esta un eso y 0 si no esta en uso
  struct spinlock lock; // spinlock encargado de proteger el acceso al semaforo
};

struct semaforo arreglo_semaforos[NSEMAFOROS]; // arreglo donde se ubican los semaforos


void init_arr()
{
  for (unsigned int i = 0; i < NSEMAFOROS; ++i)
  {
    arreglo_semaforos[i].value = 0;  // inicializamos en 0 
    arreglo_semaforos[i].semActivo = 0; // inicializamos en 0 ya q esta inactivo el semaforo
    initlock(&(arreglo_semaforos[i].lock), "semaforo"); // spinlock encargado de proteger el acceso al semaforo 
  }
}

int sem_open(int sem, int value)
{
  // Validamos el ID del semaforo 
  if (sem < 0 || sem >= NSEMAFOROS)
  {
    printf("Error, ID del semaforo invalido\n");
    return 0;
  }

  // Se bloquea el lock del semaforo para evitar que otros procesos lo cierren/modifiquen
  acquire(&(arreglo_semaforos[sem].lock));

  // Verificamos si el semaforo ya esta activo
  if (arreglo_semaforos[sem].semActivo)
  {
    // Si esta activo, liberamos el lock y retornamos error 
    release(&(arreglo_semaforos[sem].lock));
    printf ("Error, semáforo ya esta en uso\n");
    return 0;
  }
  
  // Inicializamos el semáforo
  arreglo_semaforos[sem].value = value;
  arreglo_semaforos[sem].semActivo = 1;

  // Liberamos el lock
  release(&(arreglo_semaforos[sem].lock));

  return sem; // Retornamos el ID del semáforo
}


int sem_up(int sem)
{
  if (sem < 0 || sem >= NSEMAFOROS)
  {
    printf("Error, ID del semaforo invalido\n"); // chequeo error de limites
    return 0;
  }

  acquire(&(arreglo_semaforos[sem].lock)); // reservamos el spinlock para evitar carreras entre procesos

  if (arreglo_semaforos[sem].value == 0) // si luego de incrementar el valor es menor a 0, entonces hay procesos en espera de recursos
  {
    wakeup(&(arreglo_semaforos[sem])); // despertamos los procesos del canal asociado al semaforo
  }
  arreglo_semaforos[sem].value++; // aumentamos el valor en 1 indicando que se libero un recurso

  release(&(arreglo_semaforos[sem].lock)); // Liberamos el spinlock reservado

  return arreglo_semaforos[sem].value; // retornamos 1 como caso de exito
}

int sem_close(int sem)
{
  // controlamos que el id del semaforo "sem" sea un entero entre 0 y NSEMAFOROS
  if (sem < 0 || sem >= NSEMAFOROS)
  {
    printf("Error, ID del semaforo invalido\n");
    return 0;
  }
  // se bloquea el lock del semaforo para evitar que otros procesos lo cierren/modifiquen
  acquire(&(arreglo_semaforos[sem].lock));

  arreglo_semaforos[sem].value = -1;       // semaforo cerrado
  release(&(arreglo_semaforos[sem].lock)); // liberamos lock
  return 1;
}

int sem_down(int sem)
{
  acquire(&(arreglo_semaforos[sem].lock));
  if(arreglo_semaforos[sem].value == -1)
  {
    printf("ERROR: no se puede aumentar el valor de un semaforo cerrado\n"); // Chequeamos si el semaforo esta cerrado.
    release(&(arreglo_semaforos[sem].lock));                                 // Liberamos el spinlock reservado
    return 0;
  }

  while (arreglo_semaforos[sem].value == 0)
  {
    // si el semaforo no tiene recursos, utilizamos sleep para "dormir" los procesos
    sleep(&(arreglo_semaforos[sem]), &(arreglo_semaforos[sem].lock));
  }

  arreglo_semaforos[sem].value--;          // decrementamos el valor del semaforo
  release(&(arreglo_semaforos[sem].lock)); // liberamos lock

  return arreglo_semaforos[sem].value;
}
