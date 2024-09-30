#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argcv)
{
    // Revisamos si la cantidad de argumentos son correctos
    if (argc != 2)
    {
        printf("ERROR: La cantidad de argumentos pasados son incorrectos\n");
        exit(1);
    }
    int numeroDeCiclos; // la cantidad de ping pong que haremos

    numeroDeCiclos = atoi(argcv[1]); // Usamos la funcion atoi para convertir el string en un numero.

    // Comprobamos que el numero sea positivo
    if (numeroDeCiclos <= 0)
    {
        printf("ERROR: El numero debe ser mayor a 0\n");
        exit(1);
    }

    // Creamos los semaforos
    int semaforo_0 = sem_open(0, 1);
    int semaforo_1 = sem_open(1, 0);

    int pid = fork(); // Creamos el proceso hijo

    // Caso de error si falla la creacion del proceso
    if (pid < 0)
    {
        printf("ERROR: Ocurrio un error creando el proceso\n");
        exit(1);
    }
    for (unsigned int i = 0; i < numeroDeCiclos; ++i)
    {
        if (pid > 0)
        {
            sem_down(semaforo_0);
            printf("ping\n");
            sem_up(semaforo_1);
        }
        else
        {
            sem_down(semaforo_1);
            printf("       pong\n");
            sem_up(semaforo_0);
        }
    }
    wait(0);
    sem_close(semaforo_0);
    sem_close(semaforo_1);

    return 0;
}
/*
    ping -> sem0
    pong -> sem1
    ping
    pong
    ping
    pong ...
    */
