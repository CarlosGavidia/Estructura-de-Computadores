#include <stdio.h>
#include "44b.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"
#include "uart.h"
#include "stdlib.h"

#define N 4 //Tamaño del buffer tmrbuffer
#define M 128 //Tamaño del buffer readlineBuf que se pasa como parámetro a la rutina readline

/* Variables para la gestión de la ISR del teclado
 * 
 * Keybuffer: puntero que apuntará al buffer en el que la ISR del teclado debe
 *            almacenar las teclas pulsadas
 * keyCount: variable en el que la ISR del teclado almacenará el número de teclas pulsadas
 */
volatile static char *keyBuffer = NULL;
volatile static int keyCount = 0;

/* Variables para la gestion de la ISR del timer
 * 
 * tmrbuffer: puntero que apuntará al buffer que contendrá los dígitos que la ISR del
 *            timer debe mostrar en el display de 8 segmentos
 * tmrBuffSize: usado por printD8Led para indicar el tamaño del buffer a mostrar
 */
volatile static char *tmrBuffer = NULL;
volatile static int tmrBuffSize = 0;

//Variables globales para la gestión del juego
static char passwd[N];  //Buffer para guardar la clave inicial
static char guess[N];   //Buffer para guardar la segunda clave
char readlineBuf[M];    //Buffer para guardar la linea leída del puerto serie

//Configuración de la uart
struct ulconf uconf = {
	.ired = OFF,
	.par  = NONE,
	.stopb = 0,
	.wordlen = EIGHT,
	.echo = OFF,
	.baud    = 115200,
};

enum state {
	INIT = 0,     //Init:       Inicio del juego
	SPWD = 1,     //Show Pwd:   Mostrar password
	DOGUESS = 2,  //Do guess:   Adivinar contraseña
	SGUESS = 3,   //Show guess: Mostrar el intento
	GOVER = 4     //Game Over:  Mostrar el resultado
};
enum state gstate; //estado/fase del juego 

//COMPLETAR: Declaración adelantada de las ISRs de timer y teclado (las marca como ISRs)
void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

// Función que va guardando las teclas pulsadas
static void push_buffer(char *buffer, int key)
{
	int i;
	for (i=0; i < N-1; i++)
		buffer[i] = buffer[i+1];
	buffer[N-1] = (char) key;
}

void timer_ISR(void)
{
	static int pos = 0; //contador para llevar la cuenta del dígito del buffer que toca mostrar

    //COMPLETAR: Visualizar el dígito en la posición pos del buffer tmrBuffer en el display
		D8Led_digit(tmrBuffer[pos]);

		// Si es el último dígito:
			//      Poner pos a cero,
			//      Parar timer
			//      Dar tmrBuffer valor NULL
		if(pos == tmrBuffSize -1){
			pos = 0;
			tmr_stop(0);
			tmrBuffer = NULL;
		}
		// Si no, se apunta al siguiente dígito a visualizar (pos)
		else{
			pos++;
			*tmrBuffer = tmrBuffer[pos];
		}

		// COMPLETAR: Finalizar correctamente la ISR
		ic_cleanflag(13);
}

void printD8Led(char *buffer, int size)
{
	//Esta rutina prepara el buffer que debe usar timer_ISR (tmrBuffer)
		tmrBuffer = buffer;
		tmrBuffSize = size;

		//COMPLETAR: Arrancar el TIMER0 para que interrumpa SIEMPRE casa segundo
		tmr_update(0);
		tmr_start(0);
		//COMPLETAR: Esperar a que timer_ISR termine (tmrBuffer)
		while(tmrBuffer != NULL){

		}

}

void keyboard_ISR(void)
{
	int key;

		/* Eliminar rebotes de presión */
		Delay(200);

		/* Escaneo de tecla */
		// COMPLETAR
		key= kb_scan();

		if (key != -1) {
			//COMPLETAR:
			//Si la tecla pulsada es F deshabilitar interrupciones por teclado y
			//poner keyBuffer a NULL
			if(key == 0xF){
				keyBuffer = NULL;
				ic_disable(24);
			}

			// Si la tecla no es F guardamos la tecla pulsada en el buffer apuntado
			// por keybuffer mediante la llamada a la rutina push_buffer
			else{
				push_buffer(keyBuffer, key);
				keyCount++;// Actualizamos la cuenta del número de teclas pulsadas

			}



			/* Esperar a que la tecla se suelte, consultando el registro de datos rPDATG */
			while (!(rPDATG & 0x02));
		}

		/* Eliminar rebotes de depresiÃ³n */
		Delay(200);

		//COMPLETAR: Finalizar correctamente la ISR
		ic_enable(24);
		ic_cleanflag(24);
}

int read_kbd(char *buffer)
{
	//Esta rutina prepara el buffer en el que keyboard_ISR almacenará las teclas 
		//pulsadas (keyBuffer) y pone a 0 el contador de teclas pulsadas
		keyBuffer = buffer;
		keyCount = 0;

		//COMPLETAR: Habilitar interrupciones por teclado
		ic_enable(24);
		//COMPLETAR: Esperar a que keyboard_ISR indique que se ha terminado de
		//introducir la clave (keyBuffer)
		while(keyBuffer != NULL){

		}
		//COMPLETAR: Devolver número de teclas pulsadas
		return keyCount;
}

int readline(char *buffer, int size)
{
	int count = 0; //cuenta del número de bytes leidos
	char c;        //variable para almacenar el carácter leído

	if (size == 0)
		return 0;

	// COMPLETAR: Leer caracteres de la uart0 y copiarlos al buffer
	// hasta que llenemos el buffer (size) o el carácter leído sea
	// un retorno de carro '\r'
	// Los caracteres se leen de uno en uno, utilizando la interfaz
	// del módulo uart, definida en el fichero uart.h
 	do{ 
 		uart_getch(0, &c);
 		buffer[count] = c;
		count++;
	}while(count <size && c != '\n');

	return count;
}

static int show_result()
{
	int error = 0;
	int i = 0;
	char buffer[2] = {0};
	int j;
	// COMPLETAR: poner error a 1 si las contraseñas son distintas
	for(j=0; j<4; j++){
		if(passwd[j] == guess[j]){
			i++;
		}
	}
		if (i != 4){
			error = 1;
		}
	// MODIFICAR el código de la parte1 para que además de mostrar A o E en el
	// display de 8 segmentos se envíe por el puerto serie uart0 la cadena "\nCorrecto\n"
	// o "\nError\n" utilizando el interfaz del puerto serie definido en uart.h
	if(error == 0){
		buffer[0] = 0xA;
		buffer[1] = 0xA;
		uart_send_str(UART0, "\nCorrecto\n"); 
	}
	else{
		buffer[0] = 0xE;
		buffer[1] = 0xE;
		uart_send_str(UART0, "\nError\n");
	}

	printD8Led(buffer,2);
	// COMPLETAR: esperar a que la ISR del timer indique que se ha terminado
	while(tmrBuffer != NULL){

	}

	
	// COMPLETAR: Devolver el valor de error para indicar si se ha acertado o no
	return error;
	
}

int setup(void)
{

	D8Led_init();

	/* COMPLETAR: Configuración del timer0 para interrumpir cada segundo */

	tmr_set_prescaler(0, 255);	//      valor de prescalado a 255
	tmr_set_divider(0, 1);		//      valor del divisor 1/4
	tmr_set_count(0, 62500, 62495);	//      valor de cuenta 62500 y cualquier valor de comparacion entre 1 y 62499
	tmr_update(0);				//      actualizar el contador con estos valores (update)
	tmr_set_mode(0,1);			//      poner el contador en modo RELOAD

	/********************************************************************/

	// COMPLETAR: Registramos las ISRs

	pISR_TIMER0    = (unsigned) timer_ISR;
	pISR_EINT1     = (unsigned) keyboard_ISR;

	/* Configuración del controlador de interrupciones*/
	ic_init();

	ic_conf_irq(1, 0);			//		habilitar la línea IRQ en modo vectorizado
	ic_conf_fiq(0);				//		deshabilitar la línea FIQ
	ic_conf_line(13, 0);		//		configurar la línea INT_TIMER0 en modo IRQ
	ic_conf_line(24, 0);		//		configurar la línea INT_EINT1 en modo IRQ
	ic_enable(13);				//		habilitar la línea INT_TIMER0
	ic_enable(24);
	ic_enable(7);//Sabemos que estan hechos en uart.c
	ic_enable(6);//



	/***************************************************/

	/***************************************************/
	//COMPLETAR: CoT)figuración de la uart0
	
		/* Hay que:
		 * 1. inicializar el módulo
		 * 2. Configurar el modo linea de la uart0 usando la variable global uconf
		 * 3. Configurar el modo de recepción (POLL o INT) de la uart0
		 * 4. Configurar el modo de transmisión (POLL o INT) de la uart0
		 */
	uart_init();
	uart_lconf(0 , &uconf);
	uart_conf_rxmode(0 , INT);
	uart_conf_txmode(0 , INT);

	/***************************************************/

	Delay(0);

	/* Inicio del juego */
	gstate = INIT;
	D8Led_digit(12);

	return 0;
}

static char ascii2digit(char c)
{
	char d = -1;

	if ((c >= '0') && (c <= '9'))
		d = c - '0';
	else if ((c >= 'a') && (c <= 'f'))
		d = c - 'a' + 10;
	else if ((c >= 'A') && (c <= 'F'))
		d = c - 'A' + 10;

	return d;
}


int loop(void)
{
	int count; //número de teclas pulsadas
	int error;

	//Máquina de estados

	switch (gstate) {
		case INIT:
			do {
				D8Led_digit(12);		
				     //Llamar a la rutina read_kbd para guardar los 4 dígitos en el buffer passwd
					//Esta rutina devuelve el número de teclas pulsadas.
				count = read_kbd(passwd);
				//Dibujar una E en el display si el número de teclas pulsadas es menor que 4, en este caso hacer un Delay(10000)
				if(count < 4){
					D8Led_digit(14);
					Delay(10000);
				}

			} while (count <4);/*permanecer en el while mientras se hayan pulsado menos de 4 teclas*/

			//COMPLETAR: Pasar al estado siguiente
			gstate = SPWD;
			break;

		case SPWD:

			// COMPLETAR:
			printD8Led(passwd,4);
			
			Delay(10000);
			//COMPLETAR
			gstate = 2;

			break;

		case DOGUESS:
 			Delay(10000);
			do {
				//COMPLETAR:
				/* 
				 * 1. Mandar por el puerto serie uart0 la cadena "Introduzca passwd: "
				 *    usando el interfaz definido en uart.h
				 *
				 * 2. Mostrar una F en el display
				 *
				 * 3. Llamar a la rutina readline para leer una línea del puerto
				 *    serie en el buffer readlineBuf, almacenando en count el
				 *    valor devuelto (número de caracteres leídos)
				 *
				 * 4. Si el último caracter leído es un '\r' decrementamos count
				 *    para no tenerlo en cuenta
				 *
				 * 5. Si count es menor de 4 la clave no es válida, mostramos
				 *    una E (digito 14) en el display de 8 segmentos y esperamos
				 *    1 segundo con Delay.
				 */
				//1
				uart_printf(0, "Introduzca passwd: ");
				//2
				D8Led_digit(15);
				//3
				count = readline(readlineBuf, M);
				//4

				if(readlineBuf[count-1] == '\n'){
					count--;
				}
				//5
				if (count < 4){
					D8Led_digit(14);
					Delay(10000);

				}



			} while (count < 4);

			/* COMPLETAR: debemos copiar los 4 últimos caracteres de readline en
			 * el buffer guess, haciendo la conversión de ascii-hexadecimal a valor 
			 * decimal. Para ello podemos utilizar la función ascii2digit
			 * definida más arriba.
			 */
			guess[0] = ascii2digit(readlineBuf[count-4]);
			guess[1] = ascii2digit(readlineBuf[count-3]);
			guess[2] = ascii2digit(readlineBuf[count-2]);
			guess[3] = ascii2digit(readlineBuf[count-1]); // coje la f o la buena  PROBAR

			//COMPLETAR: Pasar al estado siguiente
			gstate = 3;

			break;

		case SGUESS:
			//COMPLETAR:
			printD8Led(guess, 4);
			Delay(10000);
			//COMPLETAR: Pasar al estado siguiente
			gstate = 4;
			break;

		case GOVER:
			error = show_result();
			Delay(10000);
			//Si he acertado el estado siguiente es INIT sino DOGUESS
			if(error == 0){
				gstate = 0;

			}
			else{
				gstate = 2;
			}
			break;
	}
	return 0;
}

int main(void)
{
	setup();

	while (1) {
		loop();
	}
}
