#include "bloques.h"
#include "ficheros_basico.h"


int main(int argc, char const *argv[]){
// Controlamos que lleguen bien los argumentos.
 if(argv[1]==NULL){
  fprintf(stderr, "[leer_sf.c -> main] ERROR FALTA EL SEGUNDO ARGUMENTO <nombre_dispositivo> \n");
  return -1;
 }
 //Montamos el disco
 bmount(argv[1]);

// Leemos el contenido del fichero y lo imprimimos por consola.
 struct superbloque superbloqueLeido;
 bread(posSB,&superbloqueLeido);
 printf("DATOS DEL SUPERBLOQUE\n");
 printf("posPrimerBLoqueMB = %d\n",superbloqueLeido.posPrimerBloqueMB);
 printf("posUltimoBloqueMB = %d\n",superbloqueLeido.posUltimoBloqueMB);
 printf("posPrimerBloqueAI = %d\n",superbloqueLeido.posPrimerBloqueAI);
 printf("posUltimoBloqueAI = %d\n",superbloqueLeido.posUltimoBloqueAI);
 printf("posPrimerBloqueDatos = %d\n",superbloqueLeido.posPrimerBloqueDatos);
 printf("posUltimoBloqueDatos = %d\n",superbloqueLeido.posUltimoBloqueDatos);
 printf("posInodoRaiz = %d\n",superbloqueLeido.posInodoRaiz);
 printf("posPrimerInodoLibre = %d\n",superbloqueLeido.posPrimerInodoLibre);
 printf("cantBloquesLibres = %d\n",superbloqueLeido.cantBloquesLibres);
 printf("cantInodosLibres = %d\n",superbloqueLeido.cantInodosLibres);
 printf("totBloques = %d\n",superbloqueLeido.totBloques);
 printf("totInodos = %d\n",superbloqueLeido.totInodos);
 printf("\n");
 printf("sizeof struct superbloque = %ld\n",sizeof(superbloqueLeido));
 printf("sizeof struct inodo = %ld\n",sizeof(struct inodo));
 printf("\n");
 printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");
 // Será un recorrido desde posPrimerBloqueAI hasta posUltimoBloqueAI
 //int numInodos= superbloqueLeido.posUltimoBloqueAI-superbloqueLeido.posPrimerBloqueAI;
 
 struct inodo inodoLeido;
 for(int i=superbloqueLeido.posPrimerBloqueAI; i<superbloqueLeido.posUltimoBloqueAI; i++){
    printf("hola\n");
    bread(i,&inodoLeido);
    if(inodoLeido.tipo == 'l'){ // Si el inodo leido está libre.
     printf("%d, ",i);
    }
 } 
 
 return 1;
}