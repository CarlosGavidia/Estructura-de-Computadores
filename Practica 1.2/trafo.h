/*-----------------------------------------------------------------
**
**  Fichero:
**    trafo.h  10/6/2014
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Autom�tica
**    Facultad de Inform�tica. Universidad Complutense de Madrid
**
**  Prop�sito:
**    Contiene las declaraciones de los prototipos de funciones
**    usadas por el programa principal
**
**  Notas de dise�o:
**
**---------------------------------------------------------------*/

#ifndef _TRAFO_H
#define _TRAFO_H

#include "types.h"


void RGB2GrayMatrix(pixelRGB orig[], unsigned char dest[], int nfilas, int ncols);

extern char rgb2gray(pixelRGB* pixel);
//unsigned char rgb2gray(pixelRGB* pixel); /*metodo en C*/

//void Gray2BinaryMatrix(unsigned char orig[], unsigned char dest[], unsigned char umbral, int nfilas, int ncols);

extern void apply_gaussian(unsigned char im1[], unsigned char im2[], int width, int height);
//void apply_gaussian(unsigned char im1[], unsigned char im2[], int width, int height);/* metodo en C*/

void apply_sobel(unsigned char im1[], unsigned char im2[], int width, int height);

extern void MaximoGris(unsigned char imagenGris[], int N, int M, maximo imagenMax[]);

#endif
