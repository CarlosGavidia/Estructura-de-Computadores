/*********************************************************************************************
* Fichero:		timer.c


* Descrip:		funciones de control del timer0 del s3c44b0x
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "timer.h"

int tmr_set_prescaler(int p, int  value)
{
	int offset = p*4;
		if (p==1 || p==3 || p==5)
			offset=offset-4;

	value &= 0xFF;

	if (p < 0 || p > 5)
		return -1;

	rTCFG0 &=~ (value << offset);
	rTCFG0 |= (value << offset);


	//COMPLETAR: escribir el valor value a partir de la posición offset en el
	//registro rTCFG0, para establecer el valor de pre-escalado del módulo p
	//value es el valor de prescalado y p es el número del TIMER


	return 0;
}

int tmr_set_divider(int d, enum tmr_div div)
{
	int ret = 0;
	int pos = d*4;

	if ((d < 0 || d > 5) ||
			(div == D1_32 && d > 3) ||
			(div == EXTCLK && d != 5) ||
			(div == TCLK && d != 4))
		return -1;

	if (div == EXTCLK || div == TCLK)
		div = 4;

	//COMPLETAR: escribir el valor div a partir de la posición pos en el
	//registro rTCFG1 para establecer el valor para el divisor d
	rTCFG1 &=~(0x7 << pos);
	rTCFG1 |= (div << pos);

	return 0;
}

int tmr_set_count(enum tmr_timer t, int count, int cmp)
{
	int err = 0;
	switch (t) {
		case TIMER0:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer0 (rTCNTB0 y
			//rTCMPB0)
			rTCNTB0 = count;
			rTCMPB0 = cmp;

			 break;
		case TIMER1:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer1 (rTCNTB1 y
			//rTCMPB1)
			rTCNTB1 = count;
			rTCMPB1 = cmp;
			 break;
		case TIMER2:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer2 (rTCNTB2 y
			//rTCMPB2)
			rTCNTB2 = count;
			rTCMPB2 = cmp;
			 break;
		case TIMER3:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparaciónn cmp en los registros de buffer del timer3 (rTCNTB3 y
			//rTCMPB3)
			rTCNTB3 = count;
			rTCMPB3 = cmp;
			 break;
		case TIMER4:
			//COMPLETAR: establecer el valor de cuenta count y el valor de
			//comparación cmp en los registros de buffer del timer4 (rTCNTB4 y
			//rTCMPB4)
			rTCNTB4 = count;
			rTCMPB4 = cmp;
			 break;
		case TIMER5:
			//COMPLETAR: establecer el valor de cuenta count
			//en el registro de buffer del timer5 (rTCNTB5)
			rTCNTB5 = count;

			 break;
		default:
			err = -1;
	}

	return err;
}

int tmr_update(enum tmr_timer t)
{
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	//COMPLETAR: Dar el valor adecuado al bit manual update del registro rTCON
	//a partir de la posición pos (segundo bit a partir de esta posición)
	// primero para actualizar los registros rTCNTBn y rTCMPBn
	//después para ponerlo en modo no operación
	//deben ser stores distintos, lo hacemos con sentencias C distintas


	rTCON |= (0x1 << pos+1);
	rTCON &=~( 0x1 << pos+1);

	return 0;
}

int tmr_set_mode(enum tmr_timer t, enum tmr_mode mode)
{
	int err = 0;
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	if (mode == ONE_SHOT)
		//COMPLETAR: poner a 0 el bit autoreload de rTCON a partir de la posición pos (es
		//el cuarto bit a partir de esa posición)
		rTCON &=~ (0x1 << pos+3);


	else if (mode == RELOAD){
		//COMPLETAR: poner a 1 el bit autoreload a partir de la posición pos (es
		//el cuarto bit a partir de esa posición)
		rTCON |= (0x1 << pos +3);
	}
	else
		err = -1;

	return err;
}

int tmr_start(enum tmr_timer t)
{
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	//COMPLETAR: Dar el valor adecuado al bit de start para arrancar el TIMER a partir de la posición pos en el
	//registro rTCON (es el primer bit del grupo)
	rTCON |= (0x1 << pos);

	return 0;
}

int tmr_stop(enum tmr_timer t)
{
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if (t < 0 || t > 5)
		return -1;

	//COMPLETAR:  Dar el valor adecuado al bit de start para parar el TIMER a partir de la posición pos en el
	//registro rTCON (es el primer bit del grupo)
	rTCON &=~ (0x1 << pos);

	return 0;
}

int tmr_isrunning(enum tmr_timer t)
{
	int ret = 0;
	int pos = t*4;
	if (t > 0)
		pos += 4;

	if ((t >= 0) && (t <= 5)
			&& (rTCON & (0x1 << pos)))
		ret = 1;

	return ret;
}

