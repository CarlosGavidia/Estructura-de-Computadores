//Copiar código completo de la práctica 2
//Copiar código completo de la práctica 2
#include "44b.h"
#include "gpio.h"

/* Port B interface implementation */

int portB_conf(int pin, enum port_mode mode)
{
	int ret = 0;
	if (pin < 0 || pin > 10){
		return -1;
	}
	else{
	if (mode == SIGOUT){
		//rPCONB &=~ (0x1 << pin);
		rPCONB |= (0x1 << pin);
	}
		//COMPLETAR: configurar el puerto B para que pin funcione en modo SIGOUT
	else if (mode == OUTPUT){
		rPCONB &=~ (0x1 << pin);

	}
		//COMPLETAR: configurar el puerto B para que pin funcione en modo OUTPUT
	else{
		ret = -1;
	}
	return ret;
	}
}

int portB_write(int pin, enum digital val)
{
	if (pin < 0 || pin > 10)
		return -1;

	if (val < 0 || val > 1)
		return -1;

	if (val){
		//rPDATB &=~(0x1 << pin);
		rPDATB |= (0x1 << pin);

		//COMPLETAR: configurar para que el led (indicado por pin) se apague
	}
	else{
		rPDATB &=~ (0x1 << pin); //Creo que es así porque tiene un bit por linea
		//COMPLETAR: configurar para que el led (indicado por pin) se encienda

	}
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
			rPCONG &=~ (0x3 << pos);
			//COMPLETAR: configurar el puerto G para que pin funcione en modo INPUT


			break;
		case OUTPUT:
			//rPCONG &=~ (0x3 << pos);
			rPCONG |= (0x1 << pos);
			//COMPLETAR: configurar el puerto G para que pin funcione en modo OUTPUT


			break;
		case SIGOUT:
			rPCONG &=~ (0x3 << pos);
			rPCONG |= (0x2 << pos);
			//COMPLETAR: configurar el puerto G para que pin funcione en modo SIGOUT


			break;
		case EINT:
			rPCONG &=~ (0x3 << pos);
			rPCONG |= (0x3 << pos);
			//COMPLETAR: configurar el puerto G para que pin funcione en modo EINT


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
			rEXTINT &=~ (0x7 << pos);//000

			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por nivel bajo


			break;
		case LHIGH:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x1 << pos);//001
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por nivel alto


			break;
		case FALLING:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x2 << pos);//01x
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por flanco de bajada


			break;
		case RISING:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x4 << pos); //10x
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por flanco de subida


			break;
		case EDGE:
			rEXTINT &=~ (0x7 << pos);
			rEXTINT |= (0x6 << pos); //11x
			// COMPLETAR: configurar el puerto G en pin para detectar la interrupción
			// por cualquier flanco


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

	if (val){
		//rPDATG &=~ (0x1 << pos);
		rPDATG |= (0x1 << pos); //COMPLETAR: escribir en el registro de datos del puerto G (en pin) un 1
	}
	else
		rPDATG &=~ (0x1 << pos);
		//COMPLETAR: escribir en el registro de datos del puerto G (en pin) un 0


	return 0;
}

int portG_read(int pin, enum digital* val)
{
	int pos = pin*2;

	if (pin < 0 || pin > 7)
		return -1;

	if (rPCONG & (0x3 << pos))
		return -1;



	if(rPDATG & (0x3 << pos))//COMPLETAR:true si está a 1 en rDATG el pin indicado por el parámetro pin)
		*val=HIGH;
	else
		*val=LOW;

	return 0;
}

int portG_conf_pup(int pin, enum enable st)
{
	if (pin < 0 || pin > 7){
		return -1;
	}
	if (st != ENABLE && st != DISABLE){
		return -1;
	}
	//COMPLETAR: activar la resistencia de pull-up en pin
	if (st == ENABLE){
		rPUPG &=~(0x1 << pin);

	}
	else{
		rPUPG |= (0x1 << pin);
		//COMPLETAR: desactivar la resistencia de pull-up en pin
	}

	return 0;
}
