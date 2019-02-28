#include <stdio.h>
#include "44b.h"
#include "button.h"
#include "leds.h"
#include "utils.h"
#include "D8Led.h"
#include "intcontroller.h"
#include "timer.h"
#include "gpio.h"

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


void timer_ISR(void)
{
	//COMPLETAR: cada vez que el TIMER0 interrumpe el led rotante se mueve
	// si el giro es horario position se incrementa si su valor es <5 si no escribe un 0
	// si el giro es antihorario position se decrementa si su valor es >0 si no se escribe un 5
	if(RL.direction==1){
			if(RL.position==5){
				RL.position=0;
			}
			else{
				RL.position++;
			}
	}
	else{
		if(RL.position==0){
				RL.position=5;
			}
		else{
			RL.position--;
		}

	}
	D8Led_segment(RL.position);

}

void button_ISR(void)
{
	unsigned int whichint = rEXTINTPND;
	unsigned int buttons = (whichint >> 2) & 0x3;

	//COMPLETAR: BUT1 cambia el estado de LED1 y cambia dirección de
	//movimiento del led rotante
	if(buttons==BUT1){
			RL.direction = ~RL.direction;
	}

	//COMPLETAR: BUT2 cambia el estado de LED2
	//activa movimiento de led rotante si parado
	//o lo para si está en movimiento (actuar sobre rutinas del TIMER)
	if(buttons==BUT2){
		if(tmr_isrunning(TIMER0)){
			tmr_stop(TIMER0);
		}else{
			tmr_start(TIMER0);
		}
			RL.moving = ~RL.moving;
	}
	// eliminamos rebotes
	Delay(2000);
	// borramos el flag en extintpnd
	rEXTINTPND =rEXTINTPND &~(0X01<<buttons);
			//COMPLETAR: debemos borrar las peticiones de interrupción en
		         //EXTINTPND correspondientes a los pulsadores pulsados

}

int setup(void)
{
	leds_init();
	D8Led_init();
	D8Led_segment(RL.position);

	/* Port G: configuración para generación de interrupciones externas */
	//COMPLETAR: utilizando el interfaz para el puerto G definido en gpio.h
	//configurar los pines 6 y 7 del puerto G para poder generar interrupciones
	//externas por flanco de bajada por ellos y activar las correspondientes
	//resistencias de pull-up.

		portG_conf(6,EINT);
		portG_conf(7,EINT);
		portG_eint_trig(6,FALLING);
		portG_eint_trig(7,FALLING);
		portG_conf_pup(6,ENABLE);
		portG_conf_pup(7,ENABLE);

	/********************************************************************/

	/* Configuracion del timer */

	//COMPLETAR: utilizando el interfaz para los timers definido en timer.h
	//configurar el timer 0: 
	//      valor de prescalado a 255
	//      valor del divisor 1/8
	//      valor de cuenta 62500 y cualquier valor de comparacion entre 1 y 62499
	//      actualizar el contador con estos valores (update)
	//      poner el contador en modo RELOAD
	//      dejar el contador parado

		tmr_set_prescaler(TIMER0,255);
		tmr_set_divider(TIMER0,D1_8);
		tmr_set_count(TIMER0,62500,1);
		tmr_update(TIMER0);
		tmr_set_mode(TIMER0,RELOAD);
		tmr_stop(TIMER0);
	if (RL.moving)
		tmr_start(TIMER0);
	/***************************/


	/* Configuración del controlador de interrupciones
	 * Habilitamos la línea IRQ, en modo no vectorizado
	 * y registramos una ISR para la línea IRQ
	 * Configuramos el timer 0 en modo IRQ y habilitamos
	 * esta línea
	 * Configuramos la línea EINT4567 en modo IRQ y la
	 * habilitamos
	 */

	ic_init();
	//COMPLETAR: utilizando el interfaz definido en intcontroller.h
	//		habilitar la línea IRQ en modo no vectorizado
	//		deshabilitar la línea FIQ
	//		configurar la línea INT_TIMER0 en modo IRQ
	//		configurar la línea INT_EINT4567 en modo IRQ
	//		habilitar la línea INT_TIMER0
	//		habilitar la línea INT_EINT4567


	/***************************************************/
		ic_conf_irq(ENABLE,NOVEC);
		ic_conf_fiq(DISABLE);
		ic_conf_line(INT_TIMER0,IRQ);
		ic_conf_line(INT_EINT4567,IRQ);
		ic_enable(INT_TIMER0);
		ic_enable(INT_EINT4567);
	Delay(0);
	return 0;
}



int main(void)
{
	setup();
}
