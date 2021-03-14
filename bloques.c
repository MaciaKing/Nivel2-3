#include "bloques.h"

static int descriptor=0;
const char *disco=NULL;//=camino.

/*  bmount(const char *camino){}

Función para montar el dispositivo virtual, y 
dado que se trata de un fichero consistirá en abrirlo
*/
int bmount(const char *camino){
  disco=camino;
  descriptor=open(camino,O_RDWR|O_CREAT,0666);
  if(descriptor==-1){
    // Error al intentar abrir el fichero.
    fprintf(stderr, "[bmount] Error %d: %s\n", errno, strerror(errno));
    return -1;
  }
  //Ok
  return descriptor;
  
}

/*  bumount(){}

Desmonta el dispoditivo virtual, que es lo mismo que
cerrar el fichero con la función close().

Devuelve 0 (EXIT_SUCCES) si ha ido bien. Devuelve -1
(EXIT_FAILURE) si ha ido mal.
*/
int bumount(){
  // Miramos si el fichero se cierra correctamente.
  if(close(descriptor)==-1){
    fprintf(stderr, "[bumount] Error %d: %s\n", errno, strerror(errno));
    // EXIT_FAILURE
    return -1;
  }
  return 0;// EXIT_SUCCESS
}

/*  int bwrite(unsigned int nbloque, const void *buf){}

Escribe 1 bloque al dispositivo virtual, en el bloque físico
específicado por nbloque.

Devuleve el num. de bytes que ha podido escribir(= BLOCKSIZE).
Devuelve -1 si se ha producido un error.
*/ 
int bwrite(unsigned int nbloque, const void *buf){
  //__off_t desplazamiento=nbloque*BLOCKSIZE;
  //__off_t puntero=lseek(descriptor,desplazamiento,SEEK_SET);// Nos posicionamos en nbloque. SEEK_SET=mesurado en absoluto. (principio del fichero??)
  __off_t puntero=lseek(descriptor,nbloque*BLOCKSIZE,SEEK_SET);
  if(puntero==-1){
    fprintf(stderr, "[bwrite] Error %d: %s\n", errno, strerror(errno));
    return -1;
  }
  int bytesescritos=write(descriptor,buf,BLOCKSIZE);// Escribimos en el disco.
  if(bytesescritos==-1){// Algún problema escribiendo.
    fprintf(stderr, "[bwrite] Error %d: %s\n", errno, strerror(errno));
    return -1;
  }else if(bytesescritos==BLOCKSIZE){
    return bytesescritos;// Retorna el numero de bytes escritos == BLOCKSIZE.
  }else{
    // Hay error porque bytesescritos!=BOLCKSIZE.
    fprintf(stderr, "[bwrite] Error %d: %s\n", errno, strerror(errno));
    return -1;
  }
}

/*  bread(unsigned int nbloque, void *buf){}

Lee un bloque del dispositivo virtual.

Retorna el numero de bytes leido(=BLOCKSIZE)
Retorna -1(EXIT_FAILURE) si ha ido mal.
*/

int bread(unsigned int nbloque, void *buf){
  // Nos posicionamos en nbloque. SEEK_SET=mesurado en absoluto. (principio del fichero??)
  __off_t puntero=lseek(descriptor,nbloque*BLOCKSIZE,SEEK_SET);
  if(puntero==-1){
    fprintf(stderr, "[bread_puntero] Error %d: %s\n", errno, strerror(errno));
    return -1;
  }
  int bytesleidos=read(descriptor,buf,BLOCKSIZE);// Escribimos en el disco.
  if(bytesleidos<0){// Algún problema escribiendo. Read retorna un número negativo si ha habido error.
    fprintf(stderr, "[bread_bytesLeidos] Error %d: %s\n", errno, strerror(errno));
    return -1;
  }
  if(bytesleidos==BLOCKSIZE){
    return bytesleidos;// Retorna el número de bytes escritos == BLOCKSIZE.
  }
  // Hay error porque bytesleidos!=BOLCKSIZE.
  fprintf(stderr, "[bread_bytesDistintosBlockSize] Error %d: %s\n", errno, strerror(errno));
  return -1;
}
