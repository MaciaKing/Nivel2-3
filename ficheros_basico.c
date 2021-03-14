#include "ficheros_basico.h"
//REVISAR MENSAJES ERRORES ------------------------------------------------------------------------------

/*  tamMB(unsigned int nbloques){}
Calcula el tamaño en bloques necesario para el mapa de bits.

Devuelve el tamaño de bloques necesario para el mapa de bits.
*/
int tamMB(unsigned int nbloques){
    int tamBloques=(nbloques/8)/BLOCKSIZE;
    int calcula=tamBloques%BLOCKSIZE;
    if(calcula==0){
        return tamBloques; // Está justo.
    }
    return tamBloques+1; // Necesitamos más.
}

/*  tamAI(unsigned int ninodos){}
Calcula el tamaño en bloques del array de inodos.

Devuelve el tamaño de la array de inodos.
*/
int tamAI(unsigned int ninodos){
    int tam=(ninodos * INODOSIZE) / BLOCKSIZE ;// Tamaño de la array de inodos.
    // Miramos que el tamAI sea mod 0 con BLOCKSIZE
    if(tam%BLOCKSIZE==0){
        return tam;
    }
    // Se necesita añadir un bloque adicional.
    return tam+1;
}

/*  initSB(unsigned int nbloques, unsigned int ninodos){}
    Inicializa los datos del superbloque.
    Parametros: 
        - nbloques: Número de bloques
        - ninodos:  Número de inodos que tendrá el sist. de ficheros.
*/
int initSB(unsigned int nbloques, unsigned int ninodos){
    // Ahora inicializamos mi_superbloque
    struct superbloque mi_sb;

    //Asignamos posición de bloques para Mapa bits
    mi_sb.posPrimerBloqueMB=posSB + tamSB;
    int tam = tamMB(nbloques);
    mi_sb.posUltimoBloqueMB = (mi_sb.posPrimerBloqueMB + tam) - 1;  //p.ej: posPrimera = 1 + tam= 2 - 1 == posUltima = 2; 

    //Asignamos las posiciones de bloques de los inodos.
    mi_sb.posPrimerBloqueAI = mi_sb.posUltimoBloqueMB+1;
    tam = tamAI(ninodos);
    mi_sb.posUltimoBloqueAI = (mi_sb.posPrimerBloqueAI + tam) - 1; 

    //Asignamos las posiciones de los bloques de datos.
    mi_sb.posPrimerBloqueDatos = mi_sb.posUltimoBloqueAI + 1;
    mi_sb.posUltimoBloqueDatos = nbloques - 1;

    //Posición inodo raiz
    mi_sb.posInodoRaiz = 0;

    //Posición primer inodo libre
    mi_sb.posPrimerInodoLibre = 0;

    //Cantidad de bloques libres
    mi_sb.cantBloquesLibres = nbloques;

    //Cantidad de inodos libres.
    mi_sb.cantInodosLibres = ninodos;

    //Cantidad de bloques total.
    mi_sb.totBloques = nbloques;

    //Cantidad total de inodos
    mi_sb.totInodos = ninodos;


    //Escritura en disco virtual
    int estructura = bwrite(posSB, &mi_sb);
    if (estructura == -1)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        perror("Error");
        return estructura;
    }
    return estructura;
}

/*  initMB(){}
Inicializamos el mapa de bits. Ahora lo pondremos todo a 0 el mapa de bits.

Devuelve 1 si ha ido bien, devuelve -1 si ha ido mal.
*/
int initMB(){

    struct superbloque sb;
    unsigned char buffer[BLOCKSIZE];
    // Leemos el superbloque
    if(bread(posSB,&sb)==-1){
        fprintf(stderr, "[initMB] Error %d: %s\n", errno, strerror(errno));
        return -1;
    }
    // Ponemos todos los bytes a 0.
    memset(buffer,0,BLOCKSIZE);
    int numBloquesMB= tamMB(sb.totBloques);

    int i = 0;
    int j = sb.posPrimerBloqueMB;
    while(i<=numBloquesMB){
        if(bwrite(j,buffer)==-1){
            fprintf(stderr, "[ficheros_basicos.c -> initMB] Error %d: %s\n", errno, strerror(errno));
            return -1; 
        }
        i++;
        j++;
    }
    //Modificar MB con los bloques ocupados por SB + MB + AI
    int total = (tamSB+tamMB(sb.totBloques)+tamAI(sb.totInodos));   // Número de bloques ocupados
    int numBloques = total/8/BLOCKSIZE;                             // Número de bloques que necesitamos del MB
    sb.cantBloquesLibres -= numBloques;                             // Restamos los bloques libres
    i=sb.posPrimerBloqueMB;                                         // Iniciamos el contador de los bloques a modificar de MB
    int totalBytes = total/8;                                       // Total de bytes a escribir en el bloque MB
    int resto = total%8;                                            // Resto por si no son bytes enteros a 1's.
    int contadorBytes = 0;                                          // Contador de bytes escritos
    
    while(i<=numBloques){
        if(bread(i,buffer)==-1){
            fprintf(stderr, "Error [initSB], Modificación MB %d: %s\n", errno, strerror(errno));
            return -1;
        }
        j=0;                                                    // Contador del buffer.
        while((contadorBytes<=totalBytes) & (j<BLOCKSIZE)){         
            if(contadorBytes == totalBytes){                        // Comprobación si llega al último Byte de escritura.
                if(resto != 0){                                     // Si != 0 ; Habrá que conocer número decimal.
                    int byteToDecimal = bitToDecimal(resto);
                    //almacenar último byte.
                    buffer[j]=byteToDecimal;                        // Escribimos este último byte.
                    contadorBytes++;
                    j++;
                }
            }else{
                buffer[j]=255;
                contadorBytes++;
                j++;
            }
            
        }
        // Vamos guardando los bloques MB modificados.
        if(bwrite(i,buffer)==-1){
            fprintf(stderr, "Error [initSB], Modificación MB %d: %s\n", errno, strerror(errno));
            return -1;
        }
        i++;
    }
    // Guardamos el superbloque modificado.
    if(bwrite(posSB,&sb)==-1){
            fprintf(stderr, "Error [initSB], Modificación MB %d: %s\n", errno, strerror(errno));
            return -1;
        }

    return 1;
}


/*  initAI(){}
    Se encarga de inicializar la lista de inodos libres.

*/
int initAI(){
    // Cargamos el superbloque
    struct superbloque sb;
    bread(posSB,&sb);

    // Definimos el array de inodos
    struct inodo inodos [BLOCKSIZE/INODOSIZE];
    
    // Iteramos para enlazar los inodos.
    int contInodos = sb.posPrimerInodoLibre+1;
    bool centinela = true;
    for(int i=sb.posPrimerBloqueAI; i<sb.posUltimoBloqueAI;i++){ //----------preguntar por <=
        bread(i,inodos);
        int j=0;
        while((j<(BLOCKSIZE/INODOSIZE)) & centinela){
            if(contInodos<sb.totInodos){
                inodos[j].tipo= 'l';
                inodos[j].punterosDirectos[0]=contInodos;
                contInodos++;
            }else{
                inodos[j].tipo= 'l';
                inodos[j].punterosDirectos[0]=UINT_MAX;
                centinela=false;
            }
            j++;
        }
        bwrite(i,inodos);
    }
    return 1; //EXIT_SUCCESS
}

/*  escribir_bit(unsigned int nbloque, unsigned int bit){}

*/
int escribir_bit(unsigned int nbloque, unsigned int bit){
    //Declaramos el superbloque
    struct superbloque SB;
    //Lectura del superbloque
    if (bread(posSB, &SB) < 0){
        fprintf(stderr, "Error [escribir_bit], Error al intentar leer el superbloque %d: %s\n", errno, strerror(errno));
        return -1;
    }
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    //Posición del mapa de bits
    int bloqueMB = SB.posPrimerBloqueMB;
    //Determinamos en que bloque del MB se haya el bit
    bloqueMB += posbyte / BLOCKSIZE;
    posbyte %= BLOCKSIZE;
    int bloqueMBabs = SB.posPrimerBloqueMB + bloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    if (bread(bloqueMB, bufferMB) < 0){
        return -1;
    }
    // Leemos el bloque donde se encuentra el bit a escribir
    unsigned int mascara = 128; // 10000000
    mascara >>= posbit;          // desplazamiento de bits a la derecha
    // Caso en el que hay que escribir un 0
    if (bit == 0){                                 
        bufferMB[posbyte] &= ~mascara; //AND y NOT para bits
    }
    // Caso en el que hay que escribir un 1
    else{                              
        bufferMB[posbyte] |= mascara; //OR para bits
    }
    // Escribimos el bloque
    if (bwrite(bloqueMB, bufferMB) < 0){
        return -1;
    }
    return 1;
}

/*  int liberar_bloque(unsigned int nbloque){}
Libera un bloque determinado a partir de la función escribir_bit()


*/
int liberar_bloque(unsigned int nbloque){
 struct superbloque SB;
 //Lectura del superbloque
 if (bread(posSB, &SB) < 0){
     fprintf(stderr, "Error [liberar_bloque], Error al intentar leer el superbloque %d: %s\n", errno, strerror(errno));
     return -1;
  }
 if(escribir_bit(nbloque,0)){ // Escribimos en el MB como libre.
    fprintf(stderr, "Error [liberar_bloque], Error al intentar escribir_bit  %d: %s\n", errno, strerror(errno));
    return -1;
 }
 SB.cantBloquesLibres=SB.cantBloquesLibres+1; //Incrementamos la cantidad de bloques libres.
 return nbloque; //Devolvemos el num. de bloque liberado.
}


/*  int escribir_inodo(unsigned int ninodo, struct inodo inodo){}

*/
int escribir_inodo(unsigned int ninodo, struct inodo inodo){
 struct superbloque SB;
 //Lectura del superbloque
 if (bread(posSB, &SB) < 0){
     fprintf(stderr, "Error [liberar_bloque], Error al intentar leer el superbloque %d: %s\n", errno, strerror(errno));
     return -1;
  }
  
}

/**
 * 
 *  Funciones auxiliares.
 * 
 * */
int bitToDecimal (int resto) {
    int exponencial = 7;
    int i = 0;
    double decimal = 0;
    while(i<resto){
    //    decimal += pow(2,exponencial);              ---- PROBLEMA AL CIMPILAR
        exponencial--;
        i++;
    }
    return decimal;
}