/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

int main(int argc,char **argv) {
    if (argc != 4) {
        fprintf(stderr,"Sintaxis: ./mi_link disco /ruta_fichero_original /ruta_enlace\n");
        return -1;
    }
    if (bmount(argv[1]) < 0) return -1;

    // obtenemos la ruta y comprobamos que no se refiera a un directorio
    if ((argv[2][strlen(argv[2]) - 1] == '/') || (argv[3][strlen(argv[3]) - 1] == '/')){
        fprintf(stderr, "Error: la ruta se corresponde a un directorio");
        exit(-1);
    }
    mi_link(argv[2],argv[3]);
    return bumount();
}