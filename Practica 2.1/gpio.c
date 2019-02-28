#include "44b.h"
#include "gpio.h"

/* Port B interface implementation */
int portB_conf(int pin, enum port_mode mode)
{
	int ret = 0;
	if (pin < 0 || pin > 10)
		return -1;

	if (mode == SIGOUT){
		//SIGOUT== ENTRADA ' 1 '
		//COMPLETAR: configurar el puerto B para que pin funcione en modo SIGOUT
		rPCONB=rPCONB|(0X1<<pin);
	}
	else if (mode == OUTPUT){
		//OUTPUT==SALIDA '0'
		//COMPLETAR: configurar el puerto B para que pin funcione en modo OUTPUT
		rPCONB=rPCONB&~(0X1<<pin);
	}
	else
		ret = -1;

	return ret;
}
int portB_write(int pin, enum digital val)
{
	if (pin < 0 || pin > 10)
		return -1;

	if (val < 0 || val > 1)
		return -1;

	if (val)
		//COMPLETAR: configurar para que el led (indicado por pin) se apague
		rPDATB = rPDATB|(0X1<<pin);
	else
		//COMPLETAR: configurar para que el led (indicado por pin) se encienda
		rPDATB = rPDATB &~(0x1<<pin);
	return 0;
}

/* Port G interface implementation */
int portG_conf(int pin, enum port_mode mode)
{
	int pos  = pin*2;

	if (pin < 0 || pin > 7)
		return -1;

	switch (mode) {
		case INPUT:
			//poner en rPCONG 00
			//COMPLETAR: configurar el puerto G para que pin funcione en modo INPUT
			rPCONG=rPCONG &~(0X1<<pos|0x1<<pos+1);
			break;
		case OUTPUT:
			//poner en rPCONG 01
			//COMPLETAR: configurar el puerto G para que pin funcione en modo OUTPUT
				rPCONG=rPCONG &~(0x1<<pos+1);
				rPCONG=rPCONG |(0X1<<pos);
			break;
		case SIGOUT:
			//Poner en rPCONG 10
			//COMPLETAR: configurar el puerto G para que pin funcione en modo SIGOUT
				rPCONG=rPCONG |(0X1<<pos+1);
				rPCONG=rPCONG &~(0x1<<pos);
			break;
		case EINT:
			//Poner en rPCONG 11
			//COMPLETAR: configurar el puerto G para que pin funcione en modo EINT
				rPCONG=rPCONG |((0x1<<pos)|(0X1<<pos+1));
			break;
		default:
			return -1;
	}

	return 0;
}
int portG_eint_trig(int pin, enum trigger trig)
{
	int pos = pin*4;

	if (pin < 0 || pin > 7)
		return -1;

	switch (trig) {
		case LLOW:
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por nivel bajo
			rEXTINT=rEXTINT &~(0X07<<pos);
			break;
		case LHIGH:
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por nivel alto
			rEXTINT=rEXTINT &~(0x07<<pos);
			rEXTINT=rEXTINT |(0X1<<pos);
			break;
		case FALLING:
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por flanco de bajada
//SERIA UN 0X2<<pos o 0x10<<pos
			rEXTINT=rEXTINT &~(0x07<<pos);
			rEXTINT=rEXTINT |(0X2<<pos);
			break;
		case RISING:
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por flanco de subida
			rEXTINT=rEXTINT &~(0x7<<pos);
			rEXTINT=rEXTINT |(0X4<<pos);
			break;
		case EDGE:
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por cualquier flanco
			rEXTINT=rEXTINT &~(0x7<<pos);
			rEXTINT=rEXTINT |(0X6<<pos);
			break;
		default:
			return -1;
	}

	return 0;
}
int portG_write(int pin, enum digital val)
{
	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1;

	if (val < 0 || val > 1)
		return -1;

	if ((rPCONG & (0x3 << pos)) != (0x1 << pos))
		return -1;

	if (val)
		//COMPLETAR: escribir en el registro de datos del puerto G (en pin) un 1
		rPDATG=rPDATG |(0X1<<pin);
	else
		//COMPLETAR: escribir en el registro de datos del puerto G (en pin) un 0
		rPDATG=rPDATG &~(0X1<<pin);
	return 0;
}
int portG_read(int pin, enum digital* val)
{
	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1;

	if (rPCONG & (0x3 << pos))
		return -1;
	if(rPDATG &(0x1<<pin))//COMPLETAR:true si está a 1 en rDATG el pin indicado por el parámetro pin)
		*val=HIGH;
	else
		*val=LOW;

	return 0;
}
int portG_conf_pup(int pin, enum enable st)
{
	if (pin < 0 || pin > 7)
		return -1;

	if (st != ENABLE && st != DISABLE)
		return -1;

	if (st == ENABLE)
		//PONER '0' EN EL PIN CORRESPONDIENTE PARA ACTIVAR
		//COMPLETAR: activar la resistencia de pull-up en pin
		rPUPG=rPUPG &~(0X1<<pin);
	else
		//PONER '1' EN EL PIN CORRESPONDIENTE PARA DESACTIVAR
		//COMPLETAR: desactivar la resistencia de pull-up en pin
		rPUPG=rPUPG |(0X1<<pin);
	return 0;
}

