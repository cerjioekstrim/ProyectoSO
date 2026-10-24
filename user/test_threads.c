#include "kernel/types.h"
#include "user/user.h"
#include "user/threadH2.h"

//tarea a ejecutar en cada hebra
void tarea(void){
    int id = actual - tabla;
    for (int i = 0; i < 5; i++){
        printf("hebra %d: iteracion %d\n", id, i);
        thread_yield();
    }
}

int main(void){
    thread_init();

    thread_create(tarea);
    thread_create(tarea);
    thread_create(tarea);

    thread_schedule();
    printf("todas las hebras terminaron sus tareas\n");
    exit(0);
}