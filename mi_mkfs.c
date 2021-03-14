#include "bloques.h"
#include "ficheros_basico.h"


int main(int argc, char const *argv[]){
    // Antes de montar el dispositivo virtual, comprovamos que nos lleguen 3 parametros.
    // El argv[0] sabemos que sí nos llega porque es el mismo quin llama al programa.
    // Entonces comprovamos para argv[1] y para argv[2]
    if(argv[1]==NULL){
    fprintf(stderr, "[mi_mkfs -> main] ERROR FALTA EL PRIMER ARGUMENTO ('disco')\n");
    return -1;
    }
    if(argv[2]==NULL){
    fprintf(stderr, "[mi_mkfs -> main] ERROR FALTA EL TERCER ARGUMENTO (numero de bloques) \n");
    return -1;
    }
    // Montamos el dispositivo virtual.
    int nBloques=atoi(argv[2]);// Pasamos de char array a int con la función atoi.
    bmount(argv[1]);// argv[1] tendremos el dispositivo.

    // Inicializamos todos los bloques a 0 con bwrite.
    unsigned char bloque[BLOCKSIZE];
    memset(bloque,0,BLOCKSIZE);
    for(int i=0;i<nBloques;i++){
        bwrite(i,bloque);
    }

    initSB(nBloques,nBloques/4);
    initAI();
    initMB();   
    // Desmontamos el dispositivo virtual con bumount()
    bumount();

}