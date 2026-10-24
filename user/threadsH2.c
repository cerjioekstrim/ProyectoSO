#include "kernel/types.h"
#include "user/user.h"
#include "user/threadH2.h"

struct hebra  tabla[MAX_HEBRAS];
struct hebra *actual;

void thread_init(void) {
    actual = &tabla[0];
    actual->estado = CORRIENDO;   // main() es la hebra 0
}

// Función de arranque: todas las hebras empiezan aquí
static void thread_inicio(void) {
    actual->func();               // ejecuta la tarea del usuario
    actual->estado = LIBRE;       // cuando termina, se libera
    thread_schedule();            // pasa el control a otra hebra
}

int thread_create(void (*func)(void)) {
    struct hebra *h;

    for (h = tabla; h < &tabla[MAX_HEBRAS]; h++) {
        if (h->estado == LIBRE)
            break;
    }
    if (h == &tabla[MAX_HEBRAS])
        return -1; // tabla llena

    memset(&h->contexto, 0, sizeof(h->contexto));
    h->func   = func;
    h->contexto.ra = (uint64)thread_inicio;
    h->contexto.sp = ((uint64)h->pila + TAM_PILA) & ~0xFUL;
    h->estado = LISTA;
    return 0;
}

//Req C: Planificador RR
//Decide que hebra corre en este momento y hace el cambio de contexto hacia esta.
void thread_schedule(void){
    struct hebra *prev = actual; //La h que esta corriendo ahora.
    struct hebra *next = 0; //La h que se va a elegir.
    int i = prev - tabla; //La pos de la h actual en la tabla.

    //Si la h no termino (esta viva), queda lista para volver a correr despues.
    //Si la h estaba libre (termino) o lista (yield) no se hace nada ahora.
    if (prev->estado == CORRIENDO)
        prev->estado = LISTA;

    // Buscamos la siguiente h lista partiendo desde la que sigue a la actual.
    for (int k = 1; k <= MAX_HEBRAS; k++){
        // % hace que al llegar al final de la tabla se de la vuelta al inicio.
        struct hebra *h = &tabla[(i+k) % MAX_HEBRAS];
        // La h 0 (main) se salta en esta pasada, porque queremos que main solo
        // retome el control cuando todas las demas h ya terminaron.
        if (h != &tabla[0] && h->estado == LISTA){
            next = h;
            break;
        }
    }
    //Si no hay ninguna otra h lista, volver a main.
    if (next == 0 && tabla[0].estado == LISTA)
        next = &tabla[0];

    //Si no hay nadie para correr algo salio mal, mostramos error y salimos.
    if (next == 0){
        printf("En schedule: No hay hebras listas\n");
        exit(1);
    }

    // Si next es la misma h que ya corre (queda una) no hace falta cambiar de
    // contexto. si es otra, actualizamos 'actual' y llamamos al assembly, que
    // guarda los registros de prev y carga los de next.
    next->estado = CORRIENDO;
    if (next != prev){
        actual = next;
        thread_switch(&prev->contexto, &next->contexto);
    }
}

//Req D: Ceder turno de forma voluntaria
// Como las h son cooperativas, cada una llama a esto cuando quiera
// dejar que otra corra. 
// Solo la deja lista y le pasa el control al planificador
void thread_yield(void){
    actual->estado = LISTA;
    thread_schedule();
}