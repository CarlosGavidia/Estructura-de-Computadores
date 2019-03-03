#include <stdio.h>
#include "44b.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

#define N 4

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
		rI_ISPC =~ 0x0;//Igual solo hay que poner el bit 13, que es el timer 0
	}
	// Si no, se apunta al siguiente dígito a visualizar (pos)
	else{
		pos++;
		*tmrBuffer = tmrBuffer[pos];
	}

	// COMPLETAR: Finalizar correctamente la ISR
	rI_ISPC =~ 0x0;//Igual solo hay que poner el bit 13, que es el timer 0
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
		//timer_ISR();
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
			rI_ISPC =~ 0x0;
			keyBuffer = NULL;
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
	rINTMSK &=~ (0x0 << 24);// enmascara la eint 1
	rI_ISPC =~ 0x0;
}

int read_kbd(char *buffer)
{
	//Esta rutina prepara el buffer en el que keyboard_ISR almacenará las teclas 
	//pulsadas (keyBuffer) y pone a 0 el contador de teclas pulsadas
	keyBuffer = buffer;
	keyCount = 0;

	//COMPLETAR: Habilitar interrupciones por teclado
	rINTPND |= (0x1 << 24);
	//COMPLETAR: Esperar a que keyboard_ISR indique que se ha terminado de
	//introducir la clave (keyBuffer)
	while(keyBuffer != NULL){
		//keyboard_ISR();//¿??¿?¿
	}
	//COMPLETAR: Devolver número de teclas pulsadas
	return keyCount;
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

	// COMPLETAR
	// Hay que visualizar el resultado durante 2s.
	// Si se ha acertado tenemos que mostrar una A y si no una E
	// Como en printD8Led haremos que la ISR del timer muestre un buffer con dos
	// caracteres A o dos caracteres E (eso durará 2s)
		if(error == 0){
			buffer[0] = 0xA;
			buffer[1] = 0xA;
		}
		else{
			buffer[0] = 0xE;
			buffer[1] = 0xE;
		}

		printD8Led(buffer,2);
	// COMPLETAR: esperar a que la ISR del timer indique que se ha terminado
		while(tmrBuffer != NULL){
				//timer_ISR();
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

	/***************************/
	/* COMPLETAR: Port G-configuración para generación de interrupciones externas
	 *         por teclado
	 **/
	portG_conf(1,EINT);
	portG_eint_trig(1,FALLING);// doy por hecho que es flanco de bajada
	portG_conf_pup(1,ENABLE); // activo resistencia

	/********************************************************************/

	// COMPLETAR: Registramos las ISRs
	pISR_TIMER0    = (unsigned) timer_ISR;
	pISR_EINT1     = (unsigned) keyboard_ISR;

	/* Configuración del controlador de interrupciones*/


	ic_init();

	 /* Habilitamos la línea IRQ, en modo vectorizado y registramos una ISR para
		 * la línea IRQ
		 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
		 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
		 */
	ic_conf_irq(1, 0);			//		habilitar la línea IRQ en modo vectorizado
	ic_conf_fiq(0);				//		deshabilitar la línea FIQ
	ic_conf_line(13, 0);		//		configurar la línea INT_TIMER0 en modo IRQ
	ic_conf_line(24, 0);		//		configurar la línea INT_EINT1 en modo IRQ
	ic_enable(13);				//		habilitar la línea INT_TIMER0
	ic_enable(24);				//		habilitar la línea INT_EINT1
	/***************************************************/

	Delay(0);

	/* Inicio del juego */
	gstate = INIT;
	D8Led_digit(12);

	return 0;
}


int loop(void)
{
	int count; //número de teclas pulsadas
	int error;

	//Máquina de estados

	switch (gstate) {
		case INIT:
			do {
				//COMPLETAR:
    			//Visualizar una C en el display
				D8Led_digit(12);		//ESTA O PRIND8LED
     			//Llamar a la rutina read_kbd para guardar los 4 dígitos en el buffer passwd
				//Esta rutina devuelve el número de teclas pulsadas.
				count = read_kbd(passwd);
				//Dibujar una E en el display si el número de teclas pulsadas es menor que 4, en este caso hacer un Delay(10000)
				if(count < 4){
					D8Led_digit(14);  //PRINTD8LED??
					Delay(10000);
				}

			} while (count <4);/*permanecer en el while mientras se hayan pulsado menos de 4 teclas*/

			//COMPLETAR: Pasar al estado siguiente
				gstate = SPWD;
			break;

		case SPWD:

			// COMPLETAR:
			// Visualizar en el display los 4 dígitos del buffer passwd, para
			// ello llamar a la rutina printD8Led
			printD8Led(passwd,4);
			Delay(10000);
			//COMPLETAR: Pasar al estado siguiente
			gstate = 2;
			break;

		case DOGUESS:
			Delay(10000);

			do {
				//COMPLETAR:
				//Visualizar en el display una F
				D8Led_digit(15);
				//Llamar a la rutina read_kbd para guardar los 4 dígitos en el buffer guess
				//Esta rutina devuelve el número de teclas pulsadas.
				count = read_kbd(guess);

				//Dibujar una E en el display si el número de teclas pulsadas es menor que 4, en este caso hacer un Delay(10000)
				if(count < 4){  //// cuando metemos 3 y F nos lo cuenta y nos muestra el error como si fuesen 4 teclas;
					D8Led_digit(14);  //PRINTD8LED??
					Delay(10000);
				}
			} while (count <4);/*permanecer en el while mientras se hayan pulsado menos de 4 teclas*/

			//COMPLETAR: Pasar al estado siguiente
			gstate = 3;
			break;

		case SGUESS:
			//COMPLETAR:
			//Visualizar en el display los 4 dígitos del buffer guess, 
			//para ello llamar a la rutina printD8Led
			printD8Led(guess, 4);
			Delay(10000);
			//COMPLETAR: Pasar al estado siguiente
			gstate = 4;
			break;

		case GOVER:
			//COMPLETAR:
			//Mostrar el mensaje de acierto o error con show_result()
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
