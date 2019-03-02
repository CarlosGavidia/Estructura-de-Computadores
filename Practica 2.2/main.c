#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

struct RLstat {
	int moving;
	int speed;
	int direction;
	int position;
};

static struct RLstat RL = {
	.moving = 0,
	.speed = 5,
	.direction = 0,
	.position = 0,
};

void timer_ISR(void) __attribute__ ((interrupt ("IRQ")));
void button_ISR(void) __attribute__ ((interrupt ("IRQ")));
void keyboard_ISR(void) __attribute__ ((interrupt ("IRQ")));

void timer_ISR(void)
{
	//COMPLETAR:  copiar el código hecho en el apartado 1
	if(RL.direction==1){// si el giro es horario position se incrementa si su valor es <5 si no escribe un 0
			if(RL.position < 5){
				RL.position++;
			}
			else
				RL.position =0;

	}
	else if(RL.direction ==0){// si el giro es antihorario position se decrementa si su valor es >0 si no se escribe un 5
		if(RL.position > 0)
			RL.position--;
		else
			RL.position=5;

	}

	D8Led_segment(RL.position);

	//Borrar el flag de interrupción correspondiente a la línea TIMER0
	ic_cleanflag(INT_TIMER0);
	}

void button_ISR(void)
{
	unsigned int whicheint = rEXTINTPND;
	unsigned int buttons = (whicheint >> 2) & 0x3;

	//COMPLETAR:  copiar el código hecho en el apartado 1
	if(buttons == BUT1){
			led1_switch();
			if(RL.direction == 0){
				RL.direction = 1;
			}
			else {
				RL.direction = 0;
			}
		}

		else if(buttons == BUT2){
			led2_switch();
					if(RL.moving == 0){
						RL.moving = 1;
						tmr_update(0);
						tmr_start(TIMER0);
					}
					else {
						RL.moving = 0;
						tmr_stop(TIMER0);
					}

		}


	// eliminamos rebotes
	Delay(2000);
	// borramos el flag en extintpnd
	//COMPLETAR: copiar el código hecho en el apartado 1
	if(rEXTINTPND & 0x4){

			rEXTINTPND |=  (0x1 << 2);

		}
		else if(rEXTINTPND & 0x8){

			rEXTINTPND |=  (0x1 << 3);
		}

	//Borrar el flag de interrupción correspondiente a la línea EINT4567
	ic_cleanflag(INT_EINT4567);
}

void keyboard_ISR(void)
{
	int key;

	/* Eliminar rebotes de presión */
	Delay(200);
	
	/* Escaneo de tecla */
	key = kb_scan();

	if (key != -1) {
		/* Visualizacion en el display */
		//COMPLETAR: mostrar la tecla en el display utilizando el interfaz
		//definido en D8Led.h
		D8Led_digit(key);

		switch (key) {
			case 0:
				//COMPLETAR: poner en timer0 divisor 1/8 y contador 62500
				tmr_set_divider(0, 2);
				tmr_set_count(0, 62500, 62495);	//      valor de cuenta 62500 y cualquier valor de comparacion entre 1 y 62499
				tmr_update(0);
				break;
			case 1:
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 31250
				tmr_set_divider(0, 2);
				tmr_set_count(0, 31250, 31245);	//      valor de cuenta 32150 y cualquier valor de comparacion entre 1 y 62499
				tmr_update(0);
				break;
			case 2:
				//COMPLETAR: poner en timer0 timer divisor 1/8 y contador 15625
				tmr_set_divider(0, 2);
				tmr_set_count(0, 15625, 15620);	//      valor de cuenta 15625 y cualquier valor de comparacion entre 1 y 62499
				tmr_update(0);
				break;
			case 3:
				//COMPLETAR: poner en timer0 timer divisor 1/4 y contador 15625
				tmr_set_divider(0, 1);
				tmr_set_count(0, 15625, 15620);	//      valor de cuenta 62500 y cualquier valor de comparacion entre 1 y 62499
				tmr_update(0);
				break;
			default:
				break;
		}
		
		/* Esperar a que la tecla se suelte, consultando el registro de datos */		
	//	while (COMPLETAR: true si está pulsada la tecla (leer del registro rPDATG));
		//while(rPDATG & (0x0 << key)){
		while((rPDATG & 0x2)==0){
		}
	}

    /* Eliminar rebotes de depresión */
    Delay(200);
     
    /* Borrar interrupciones pendientes */
	//COMPLETAR
    rI_ISPC =~ 0x0;
    rEXTINTPND=~ 0x0;
}


int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_segment(RL.position);

	/* Port G: configuración para generación de interrupciones externas,
	 *         botones y teclado
	 **/

	//COMPLETAR: utilizando el interfaz para el puerto G definido en gpio.h
	//configurar los pines 1, 6 y 7 del puerto G para poder generar interrupciones
	//externas por flanco de bajada por ellos y activar las correspondientes
	//resistencias de pull-up.

	//copiar lo hecho para 6 y 7 en el apartado 1

	portG_conf(6,EINT);
	portG_conf(7,EINT);
	portG_eint_trig(6,FALLING);
	portG_eint_trig(7,FALLING);
	portG_conf_pup(6,ENABLE);
	portG_conf_pup(7,ENABLE);

	//Añadir lo mismo para el pin 1
	portG_conf(1,EINT);
	portG_eint_trig(1,FALLING);
	portG_conf_pup(1,ENABLE);
	/********************************************************************/

	/* Configuración del timer */

	//COMPLETAR: copiar el código hecho en el apartado 1
	tmr_set_prescaler(0, 255);	//      valor de prescalado a 255
	tmr_set_divider(0, 2);		//      valor del divisor 1/8
	tmr_set_count(0, 62500, 62495);	//      valor de cuenta 62500 y cualquier valor de comparacion entre 1 y 62499
	tmr_update(0);				//      actualizar el contador con estos valores (update)
	tmr_set_mode(0,1);			//      poner el contador en modo RELOAD
	tmr_stop(0);				//      dejar el contador parado

	if (RL.moving)
		tmr_start(TIMER0);
	/***************************/

	// Registramos las ISRs


	pISR_TIMER0    = (unsigned) timer_ISR;//(rINTPND|=(0x1 << 	13));//(rI_ISPR &=~ (0x0 << 13));		//timer_ISR();	//COMPLETAR: registrar la RTI del timer
	pISR_EINT4567  = (unsigned) button_ISR;//(rINTPND|=(0x1 << 	21));//button_ISR();	//COMPLETAR: registrar la RTI de los botones
	pISR_EINT1     = (unsigned) keyboard_ISR;//(rINTPND |=(0x1 <<  24));//keyboard_ISR();	//COMPLETAR: registrar la RTI del teclado
	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo vectorizado y registramos una ISR para
	 * la línea IRQ
	 * Configuramos el timer 0 en modo IRQ y habilitamos esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la habilitamos
	 * Configuramos la línea EINT1 en modo IRQ y la habilitamos
	 */

	ic_init();
	//COMPLETAR: utilizando el interfaz definido en intcontroller.h
	ic_conf_irq(1, 0);			//		habilitar la línea IRQ en modo vectorizado
	ic_conf_fiq(0);				//		deshabilitar la línea FIQ
	ic_conf_line(13, 0);		//		configurar la línea INT_TIMER0 en modo IRQ
	ic_conf_line(21, 0);		//		configurar la línea INT_EINT4567 en modo IRQ
	ic_conf_line(24, 0);		//		configurar la línea INT_EINT1 en modo IRQ
	ic_enable(13);				//		habilitar la línea INT_TIMER0
	ic_enable(21);				//		habilitar la línea INT_EINT4567
	ic_enable(24);				//		habilitar la línea INT_EINT1



	/***************************************************/

	Delay(0);
	return 0;
}


int main(void)
{
	setup();

	while (1) {

	}
}
