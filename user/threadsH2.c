#include "kernel/types.h"
#include "user/user.h"
#include "user/threadsH2.h"

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

    memset(&h->ctx, 0, sizeof(h->ctx));
    h->func   = func;
    h->ctx.ra = (uint64)thread_inicio;
    h->ctx.sp = ((uint64)h->pila + TAM_PILA) & ~0xFUL;
    h->estado = LISTA;
    return 0;
}