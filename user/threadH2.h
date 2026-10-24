#ifndef THREADH2_H
#define THREADH2_H

#include "kernel/types.h"

#define MAX_HEBRAS 8    //para tener margen
#define TAM_PILA   4096 //por simplicidad, no se hace dinámica

enum estado { LIBRE, LISTA, CORRIENDO };

struct
  contexto { // los registros t y a no se guardan porque pueden ser sobreescritos por la función que se llama.
  uint64 ra;
  uint64 sp;
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

struct hebra {
  struct contexto contexto; // "fotografía" de la CPU cuando está pausada
  enum estado estado;
  void (*func)(void); // la tarea que debe ejecutar
  char pila[TAM_PILA];
};

extern struct hebra tabla[MAX_HEBRAS]; // tabla de hebras para el planificador
extern struct hebra
  *actual; // puntero a la hebra que está corriendo actualmente

void thread_init(void);
int thread_create(void (*func)(void));
void thread_schedule(void);
void thread_yield(void);

//se implementa en assembly thread_switch.S
void thread_switch(struct contexto *vieja, struct contexto *nueva);
#endif