/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
//Programa para testear las cachés de directorios

#include "directorios.h"

int main(int argc, char **argv){
    //Comprobamos sintaxis
    if (argc != 5){
        fprintf(stderr, "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        exit(-1);
    }

    //struct STAT stat;
    //montamos el dispositivo
    if (bmount(argv[1]) < 0) return -1;
    //obtenemos el texto y su longitud
    char *buffer_texto = argv[3];
    int longitud = strlen(buffer_texto);

    //obtenemos la ruta y comprobamos que no se refiera a un directorio
    if (argv[2][strlen(argv[2]) - 1] == '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un directorio");
        exit(-1);
    }
    char *camino = argv[2];
    //obtenemos el offset
    unsigned int offset = atoi(argv[4]);
    int escritos = 0;
    fprintf(stderr, "longitud texto: %d\n", longitud);
    // escribimos varias veces el texto desplazado 1 bloque
    escritos = mi_write(camino, buffer_texto, offset, longitud);
    if (escritos == -1) {
        escritos = 0;
    }
    fprintf(stderr, "Bytes escritos: %d\n", escritos);
    /* Visualización del stat
    mi_stat_f(ninodo, &stat);
    printf("stat.tamEnBytesLog=%d\n",stat.tamEnBytesLog);
    printf("stat.numBloquesOcupados=%d\n",stat.numBloquesOcupados);
    */

    if (bumount() < 0) return -1;
}
