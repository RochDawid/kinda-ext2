/*
    Sergi Moreno Pérez
    Antoni Payeras Munar
    Dawid Michal Roch Móll
*/

#include "directorios.h"

/*
    extraer_camino: función que pasándole una cadena de carácteres que indica el camino para acceder
                    al archivo que se desee lo separa en dos trozos (inicial y final) y determina el
                    tipo de archivo del que se trata (fichero o directorio).
    input: const char *camino, char *inicial, char *final, char *tipo
    output: 0 (success), 1 (failure)
    uses:
    used by: leer_sf.c
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
    if (camino[0] == '/') {
        char dst[80];
        char *token;
        const char s[2] = "/"; // separador
        unsigned int nBarras = 0;

        strcpy(dst,camino);
        // contamos el número de barras del camino
        for (int i=0;dst[i]!='\0';i++) {
            if (dst[i] == '/') nBarras++;
        }
        strcpy(dst,camino+1); // omitimos la primera barra

        if (nBarras == 1) {
            strcpy(inicial,dst);
            *tipo = 'f';
            strcpy(final,"");
        } else {
            token = strchr(dst,'/');
            strcpy(final,token);
            token = strtok(dst, s);
            strcpy(inicial,token);

            if (final[0] == '/') {
                *tipo = 'd';
            } else {
                *tipo = 'f';
            }
        }
        return EXIT_SUCCESS;
    }
    return -1;
}

/*
    mostrar_error_buscar_entrada: muestra el error de la entrada según el entero pasado por parámetro.
    input: int error
    output: -
    uses: -
    used by: leer_sf.c
*/
void mostrar_error_buscar_entrada(int error) {
    switch (error) {
        case -1: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
        case -2: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
        case -3: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
        case -4: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
        case -5: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
        case -6: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
        case -7: fprintf(stderr, "Error: No es un directorio.\n"); break;
    }
}

/*
    buscar_entrada: busca una determinada entrada entre las entradas del inodo correspondiente a su inodo padre.
    input: const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada,
           char reservar, unsigned char permisos
    output: -
    uses: 0 (success), 1 (failure)
    used by: leer_sf.c
*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[TAMNOMBRE];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;
    //unsigned int punteros[BLOCKSIZE/sizeof(unsigned int)];
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];

    if (!strcmp(camino_parcial,"/")) {
        struct superbloque SB;

        bread(posSB,&SB);
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }
    
    if (extraer_camino(camino_parcial, inicial, final, &tipo) == -1) return ERROR_CAMINO_INCORRECTO;
    //fprintf(stderr,"buscar_entrada() -> inicial : %s, final : %s, reservar : %d\n",inicial,final,reservar);
    leer_inodo(*p_inodo_dir, &inodo_dir);
    if ((inodo_dir.permisos & 4) != 4) {
        //fprintf(stderr,"buscar_entrada() -> El inodo %d no tiene permisos de lectura\n",*p_inodo_dir);
        return ERROR_PERMISO_LECTURA;
    }

    //inicializar buffer 
    //memset(entradas,0,BLOCKSIZE);
    // calcular cantidad de entradas que tiene el inodo
    cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada);
    num_entrada_inodo = 0;
    int indice = 0;
    memset(entradas,0,BLOCKSIZE); // ponemos a 0 el buffer de lectura independientemente de la cantidad de entradas del inodo
    if (cant_entradas_inodo > 0) {
        int offset = 0;
        mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial,entradas[indice].nombre)) {
            for (;(num_entrada_inodo < cant_entradas_inodo) && strcmp(inicial,entradas[indice].nombre) && indice < BLOCKSIZE/sizeof(struct entrada);num_entrada_inodo++,indice++) {}
            if (indice == BLOCKSIZE/sizeof(struct entrada)) {
                indice = 0;
                offset += BLOCKSIZE;
                memset(entradas,0,BLOCKSIZE);
                mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
            } 
        }

        /* int offset = 0;
        int modulo = 1;
        int numPunteroDirecto = 0;
        mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
        for (;(num_entrada_inodo < cant_entradas_inodo) && strcmp(inicial,entradas[modulo-1].nombre) && numPunteroDirecto < 12;num_entrada_inodo++,modulo++) {
            modulo %= BLOCKSIZE/sizeof(struct entrada)+1; // calculamos el número de entradas del bloque???

            if (modulo == 0) { // si quedan entradas por tratar???
                offset += BLOCKSIZE;
                numPunteroDirecto++;
                memset(entradas,0,BLOCKSIZE); // reseteamos el buffer de lectura antes de hacer la siguiente lectura
                mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
            }
        }
        int numPunteroInd = 0; // indice para localizar a qué puntero indirecto se apunta
        offset += BLOCKSIZE;
        int moduloPuntero = 1;
        memset(punteros,0,BLOCKSIZE);
        mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
        while ((num_entrada_inodo < cant_entradas_inodo) && strcmp(inicial,entradas[modulo-1].nombre) && numPunteroInd < 3) {
            offset += BLOCKSIZE;
            int numPuntero = 0;
            modulo = 1;
            memset(entradas,0,BLOCKSIZE); // antes de cada lectura limpiamos el buffer de lectura
            mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);

            for (;(num_entrada_inodo < cant_entradas_inodo) && strcmp(inicial,entradas[modulo-1].nombre);num_entrada_inodo++,modulo++) {
                modulo %= BLOCKSIZE/sizeof(struct entrada)+1; // calculamos el número de entradas tratadas del bloque

                if (modulo == 0) { // si quedan entradas por tratar
                    offset += BLOCKSIZE;
                    numPuntero++;
                    memset(entradas,0,BLOCKSIZE); // antes de cada lectura limpiamos el buffer de lectura
                    mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
                }
            }

            moduloPuntero %= BLOCKSIZE/sizeof(unsigned int)+1;
            if (moduloPuntero == 0) {
                offset += BLOCKSIZE;
                numPunteroInd++;
                memset(punteros,0,BLOCKSIZE); // reseteamos el buffer de punteros
                mi_read_f(*p_inodo_dir,entradas,offset,BLOCKSIZE);
            }
        } */
    }

    if (strcmp(inicial,entradas[indice].nombre)) {
        switch(reservar) {
            case 0: return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            case 1: 
                if (inodo_dir.tipo == 'f') {
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }
                if ((inodo_dir.permisos & 2) != 2) {
                    return ERROR_PERMISO_ESCRITURA;
                } else {
                    //copiar inicial en el nombre de la entrada
                    strcpy(entrada.nombre,inicial);
                    if (tipo == 'd') {
                        if (!strcmp(final,"/")) {
                            entrada.ninodo = reservar_inodo('d',permisos);
                        } else {
                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        }
                    } else {
                        entrada.ninodo = reservar_inodo('f',permisos);
                    }
                    //fprintf(stderr,"buscar_entrada() -> Reservado inodo %d tipo %c con permisos %d para %s\n",entrada.ninodo,tipo,permisos,entrada.nombre);
                    if ((mi_write_f(*p_inodo_dir,&entrada,inodo_dir.tamEnBytesLog,sizeof(struct entrada))) == -1) {
                        if (entrada.ninodo != -1) { // si la escritura da error
                            liberar_inodo(entrada.ninodo); // liberamos el inodo
                        }
                        return -1;
                    }
                    //fprintf(stderr,"buscar_entrada() -> Creada entrada: %s, %d\n",entrada.nombre,entrada.ninodo);
                }
        }
    }

    if (!strcmp(final,"") || !strcmp(final,"/")) {
        if ((num_entrada_inodo < cant_entradas_inodo) && reservar == 1) {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        *p_inodo = entradas[indice].ninodo;
        *p_entrada = num_entrada_inodo;

        return EXIT_SUCCESS;
    } else {
        *p_inodo_dir = entradas[indice].ninodo;

        return buscar_entrada(final,p_inodo_dir,p_inodo,p_entrada,reservar,permisos);
    }
    return EXIT_SUCCESS;
}

int mi_creat(const char *camino, unsigned char permisos) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 1;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,permisos)) < 0) {
        mostrar_error_buscar_entrada(error);
    }
    return 0;
}

int mi_dir(const char *camino, char *buffer) { // const char *camino, char *buffer, char tipo
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
    //unsigned int punteros[BLOCKSIZE/sizeof(unsigned int)];
    struct inodo inodo;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    leer_inodo(p_inodo, &inodo); // leemos el inodo de la entrada

    if (inodo.tipo != 'd') return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO; // no se trata de un directorio
    if ((inodo.permisos & 4) != 4) return ERROR_PERMISO_LECTURA; // no tiene permisos de lectura

    // calcular cantidad de entradas que tiene el inodo
    int cant_entradas_inodo = inodo.tamEnBytesLog/sizeof(struct entrada);
    int num_entrada_inodo = 0;
    int indice = 0;
    memset(entradas,0,BLOCKSIZE);
    if (cant_entradas_inodo > 0) {
        int offset = 0;
        mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
        while (num_entrada_inodo < cant_entradas_inodo) {
            for (;(num_entrada_inodo < cant_entradas_inodo) && indice < BLOCKSIZE/sizeof(struct entrada);num_entrada_inodo++,indice++) {
                strcat(buffer,entradas[indice].nombre);
                strcat(buffer,"\t");
            }
            if (indice == BLOCKSIZE/sizeof(struct entrada)) {
                indice = 0;
                offset += BLOCKSIZE;
                memset(entradas,0,BLOCKSIZE);
                mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
            } 
        }
        /* int offset = 0;
        int modulo = 1;
        int numPunteroDirecto = 0;
        mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
        for (;(num_entrada_inodo < cant_entradas_inodo) && numPunteroDirecto < 12;num_entrada_inodo++,modulo++) {
            strcat(buffer,entradas[modulo-1].nombre);
            strcat(buffer,"\t");
            modulo %= BLOCKSIZE/sizeof(struct entrada)+1; // calculamos el número de entradas del bloque???

            if (modulo == 0) { // si quedan entradas por tratar???
                offset += BLOCKSIZE;
                numPunteroDirecto++;
                memset(entradas,0,BLOCKSIZE);
                mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
            }
        }
        int numPunteroInd = 0; // indice para localizar a qué puntero indirecto se apunta
        offset += BLOCKSIZE;
        int moduloPuntero = 1;
        memset(punteros,0,BLOCKSIZE);
        mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
        while ((num_entrada_inodo < cant_entradas_inodo) && numPunteroInd < 3) {
            offset += BLOCKSIZE;
            int numPuntero = 0;
            modulo = 1;
            memset(entradas,0,BLOCKSIZE);
            mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);

            for (;(num_entrada_inodo < cant_entradas_inodo);num_entrada_inodo++,modulo++) {
                strcat(buffer,entradas[modulo-1].nombre);
                strcat(buffer,"\t");
                modulo %= BLOCKSIZE/sizeof(struct entrada)+1; // calculamos el número de entradas tratadas del bloque

                if (modulo == 0) { // si quedan entradas por tratar
                    offset += BLOCKSIZE;
                    numPuntero++;
                    memset(entradas,0,BLOCKSIZE);
                    mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
                }
            }

            moduloPuntero %= BLOCKSIZE/sizeof(unsigned int)+1;
            if (moduloPuntero == 0) {
                offset += BLOCKSIZE;
                numPunteroInd++;
                memset(punteros,0,BLOCKSIZE);
                mi_read_f(p_inodo,entradas,offset,BLOCKSIZE);
            }
        } */
    }

    return num_entrada_inodo;
}

int mi_chmod(const char *camino, unsigned char permisos) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,permisos)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }
    mi_chmod_f(p_inodo, permisos);
    return EXIT_SUCCESS;
}

int mi_stat(const char *camino, struct STAT *p_stat) {
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,6)) < 0) {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    mi_stat_f(p_inodo, p_stat);
    return p_inodo;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,2)) < 0) {
        mostrar_error_buscar_entrada(error);
        return 0;
    }

    return mi_write_f(p_inodo, buf, offset, nbytes);
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    char reservar = 0;
    int error;

    if ((error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,reservar,4)) < 0) {
        mostrar_error_buscar_entrada(error);
        return 0;
    }
    return mi_read_f(p_inodo, buf, offset, nbytes);
}
