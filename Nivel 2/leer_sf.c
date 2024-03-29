/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/
#include "ficheros_basico.h"

int main() {
    if (bmount("disco") < 0) return -1;
    struct superbloque sb;

    if (bread(posSB,&sb) < 0) return -1;
    printf("DATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d\n",sb.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %d\n",sb.posUltimoBloqueMB);
    printf("posPrimerBloqueAI = %d\n",sb.posPrimerBloqueAI);
    printf("posUltimoBloqueAI = %d\n",sb.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %d\n",sb.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n",sb.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n",sb.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n",sb.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n",sb.cantBloquesLibres);
    printf("cantInodosLibres = %d\n",sb.cantInodosLibres);
    printf("totBloques = %d\n",sb.totBloques);
    printf("totInodos = %d\n",sb.totInodos);

    printf("\nsizeof struct superbloque: %d\n",(int) sizeof(sb));
    printf("sizeof struct inodo: %d\n",(int) sizeof(struct inodo));

    printf("\nRECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");

    struct inodo arInodos[BLOCKSIZE/INODOSIZE];
    // iteramos a través del array de inodos
    for (int i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) {
        if (bread(i,arInodos) < 0) return -1;
        for (int j = 0; j < (BLOCKSIZE/INODOSIZE); j++) {
            printf("%d ",arInodos[j].punterosDirectos[0]);
        }
    }
    printf("\n");
    return bumount();
}