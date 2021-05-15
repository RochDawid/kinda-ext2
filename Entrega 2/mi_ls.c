/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 3) {
        fprintf(stderr,"Sintaxis: ./mi_ls <disco> </ruta_directorio>\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;
    if (argv[2][strlen(argv[2]) - 1] != '/'){
        fprintf(stderr, "Error: la ruta se corresponde a un fichero\n");
        return -1;
    }
    char buffer[TAMBUFFER];
    memset(buffer,0,TAMBUFFER);
    int numEntradas = mi_dir(argv[2],buffer);
    if (numEntradas == -1) return -1;
    fprintf(stderr,"Total: %d\n",numEntradas);
    fprintf(stderr,"Contenido buffer: \n %s\n",buffer);
    return bumount();
}