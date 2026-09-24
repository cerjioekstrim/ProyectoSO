#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* namefile(char *ruta){ //se extrae solo el nombre final de una ruta completa.
    char *final_name_ru;
    for (final_name_ru = ruta + strlen(ruta); final_name_ru >= ruta && *final_name_ru != '/'; final_name_ru--)
        ; //busca el primer char después de la última /.
    final_name_ru++;
    return final_name_ru;
}

void buscar_archivo_enruta(char *ruta, char *archivo_objetivo){
    int fd_descriptores_archivos;
    struct stat st_que_abre;

    char nueva_ruta[512], *ultima_letra_ruta;
    struct dirent entrada_directorio; //usamos esta estructura de xv6 para manejar las entradas de directorio.

    if ((fd_descriptores_archivos = open(ruta, 0)) < 0){ //se intenta abrir una ruta en modo lectura (0) y open devuelve numero de fd.
        fprintf(2, "find.c no pudo abrir %s\n", ruta);
        return; //sale si falla en abrir la ruta en modo lectura por obtener un numero menor a 0.
    }
    
    if (fstat(fd_descriptores_archivos, &st_que_abre) < 0){ //fstat extrae la info del fd y la guarda en st.type, donde nos dirá si esa ruta es carpeta o archivo.
        fprintf(2, "find.c no pudo leer el stat de la ruta %s\n", ruta);
        close(fd_descriptores_archivos); //cerrar fd si hay error por no poder leer y obtener el stat (st.type).
        return;
    }

    switch (st_que_abre.type){ //segun el st.type (carpeta o archivo), entramos a los casos:
        case T_FILE: //caso archivo
            if (strcmp(namefile(ruta), archivo_objetivo) == 0){ //extraemos nombre real y lo comparamos con el buscado (objetivo).
                printf("%s\n", ruta); //si coincide con objetivo, se imprime la ruta completa original.
            }
            break;

        case T_DIR: //caso carpeta que contiene una lista de entradas de ditectorio, donde cada una tiene su nombre y inum= num id.
            strcpy(nueva_ruta, ruta); // se copia la ruta de carpeta base a nueva_ruta.
            ultima_letra_ruta = nueva_ruta + strlen(nueva_ruta);
            *ultima_letra_ruta++ = '/'; //se le agrega al final un / y se mueve un espacio a la derecha (no se sobreescribe en memmove).
            
            //se lee entrada por entrada de la carpeta.
            while (read(fd_descriptores_archivos, &entrada_directorio, sizeof(entrada_directorio)) == sizeof(entrada_directorio)){
                if (entrada_directorio.inum == 0){ // entrada vacia o archivo borrado.
                    continue;
                }

                memmove(ultima_letra_ruta, entrada_directorio.name, DIRSIZ); //el nombre de archivo encontrado se pega en ultima_letra_ruta con 14 chars por (DIRSIZ).
                ultima_letra_ruta[DIRSIZ] = 0; //cierre string para no leer memoria basura. en xv6, archivos tienen tamaño fijo definido de 14 chars llamado DIRSIZ.

                //se ignoran las carpetas invisibles por defecto que estan dentro de la carpeta . y .. (esta misma carp y la anterior carp).
                if (strcmp(entrada_directorio.name, ".") == 0 || strcmp(entrada_directorio.name, "..") == 0){
                    continue; //se salta para evitar un stack overflow y que el SO colapse, esto provocado por un bucle recursivo infi. de volver a entrar al mismo lugar una y otra vez.
                }
                
                buscar_archivo_enruta(nueva_ruta, archivo_objetivo); //se usa recursion para buscar_archivo_enruta otra vez con la nueva ruta armada.
            }
            break;
    }
    close(fd_descriptores_archivos);
}

int main(int argc, char *argv[]){ //cantidad_argumentos: cant words separadas por espacio cuando escribi en la consola para ejecutar programa y vector_argumento: array que guarda cada palabra de argc.
    if (argc < 3){ // se necesita ruta y archivo, entonces debe haber al menos 3 argc.
        printf("Error, faltan argumentos.\n");
        exit(0);
    }
    buscar_archivo_enruta(argv[1], argv[2]);
    exit(0);
}