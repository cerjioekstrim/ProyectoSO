# thread_switch: cambio de contexto entre dos hebras
# void thread_switch(struct contexto *vieja, struct contexto *nueva)
#   a0 = direccion del contexto de la hebra que se pausa (aqui guardamos)
#   a1 = direccion del contexto de la hebra que despierta (de aqui cargamos)

# solo guardamos ra, sp y s0-s11 porque son los registros que la funcion llamada
# tiene que preservar (callee-saved). los demas (t0-t6, a0-a7) el compilador ya
# los guardo si los necesitaba antes de llamar a esta funcion.
# los offsets van de 8 en 8 porque cada registro es de 64 bits, y siguen el mismo
# orden que struct contexto en threadH2.h

    .text
    .globl thread_switch
thread_switch:
    # 1) guardar los registros de la h actual en su contexto (a0)
    #a0 = contexto h pausada
    #a1 = contexto h despertada

    sd ra, 0(a0)    # ra: por donde va a seguir cuando la reanudemos
    sd sp, 8(a0)    # sp: tope de su pila
    sd s0, 16(a0)
    sd s1, 24(a0)
    sd s2, 32(a0)
    sd s3, 40(a0)
    sd s4, 48(a0)
    sd s5, 56(a0)
    sd s6, 64(a0)
    sd s7, 72(a0)
    sd s8, 80(a0)
    sd s9, 88(a0)
    sd s10, 96(a0)
    sd s11, 104(a0)

    # 2) cargar los registros de la h que va a correr desde su contexto (a1)
    ld ra, 0(a1)
    ld sp, 8(a1)    # desde aqui ya estamos usando la pila de la otra h
    ld s0, 16(a1)
    ld s1, 24(a1)
    ld s2, 32(a1)
    ld s3, 40(a1)
    ld s4, 48(a1)
    ld s5, 56(a1)
    ld s6, 64(a1)
    ld s7, 72(a1)
    ld s8, 80(a1)
    ld s9, 88(a1)
    ld s10, 96(a1)
    ld s11, 104(a1)

    # ret salta a la direccion que quedo en ra. si la h ya habia corrido, vuelve
    # justo despues de su ultimo thread_switch. si es nueva, ra apunta a thread_inicio
    ret