#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

void filtro(int pipe_izq[2]){
    close(pipe_izq[WRITE]); //se cierra la escritura del pipe izquierdo

    int primo;
    int bytes = read(pipe_izq[READ], &primo, sizeof(primo)); //se lee el primer numero del pipe izquierdo

    if(bytes == 0){
        close(pipe_izq[READ]); //si no hay mas numeros, se cierra la lectura del pipe izquierdo
        exit(0);
        return;
    }

    printf("prime %d\n", primo); //se imprime el primo encontrado

    int pipe_der[2]; //se crea un pipe derecho
    pipe(pipe_der); //se inicializa el pipe derecho

    int pid = fork(); //se crea un nieto

    if(pid < 0){
        fprintf(2, "Error al crear el fork\n");
        exit(1);
        return;
    }
    if(pid == 0){ //si es el nieto
        close(pipe_izq[READ]); //se cierra la lectura del pipe izquierdo en el nieto
        filtro(pipe_der); //se llama a la funcion filtro en el nieto
        exit(0);
        return;
    } else { //si es el padre
        close(pipe_der[READ]); //se cierra la lectura del pipe derecho en el padre

        int numero;
        while(read(pipe_izq[READ], &numero, sizeof(numero)) > 0){ //mientras haya numeros en el pipe izquierdo
            if(numero % primo != 0){ //si el numero no es divisible por el primo
                write(pipe_der[WRITE], &numero, sizeof(numero)); //se escribe el numero en el pipe derecho
            }
        }
    close(pipe_izq[READ]); //se cierra la lectura del pipe izquierdo en el padre
    close(pipe_der[WRITE]); //se cierra la escritura del pipe derecho en el padre
    wait(0); //se espera a que termine el nieto
    exit(0);
    return; //se termina el proceso del padre
    }
}


int main(){
    int fd[2];
    pipe(fd); //se crea el pipe raiz
    int pid = fork(); //se crea un hijo

    if(pid < 0){
        fprintf(2, "Error al crear el fork\n");
        exit(1);
    }

    if(pid > 0){ //si es el padre
        close(fd[READ]); //se cierra la lectura del pipe
        for(int i = 2; i <= 35; i++){ //se envian los numeros del 2 al 35
            write(fd[WRITE], &i, sizeof(i));
        }
        close(fd[WRITE]); //se cierra la escritura del pipe
        wait(0); //se espera a que termine el hijo
        exit(0);
    } else{ //si es el hijo
        filtro(fd); //se llama a la funcion filtro
    }
    return 0;
}