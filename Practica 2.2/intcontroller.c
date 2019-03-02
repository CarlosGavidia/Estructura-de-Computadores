/*-------------------------------------------------------------------
**
**  Fichero:


**    intcontroller.c  3/3/2016
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Automática
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las implementación del módulo intcontroller
**
**-----------------------------------------------------------------*/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "intcontroller.h"

void ic_init(void)
{
	/* Configuración por defector del controlador de interrupciones:
	 *    Líneas IRQ y FIQ no habilitadas
	 *    Línea IRQ en modo no vectorizado
	 *    Todo por la línea IRQ
	 *    Todas las interrupciones enmascaradas
	 **/
	rINTMOD = 0x0; // Configura las líneas como de tipo IRQ
	rINTCON = 0x7; // IRQ y FIQ enmascaradas, IRQ en modo no vectorizado
	rINTMSK = ~(0x0); // Enmascara todas las líneas
}

int ic_conf_irq(enum enable st, enum int_vec vec)
{
	//int conf = rINTCON;

	if (st != ENABLE && st != DISABLE)
		return -1;

	if (vec == VEC){
		//COMPLETAR: poner la línea IRQ en modo vectorizado
		rINTCON &=~ (0x1 << 2);
	}
	else{
		rINTCON &=~ (0x1 << 1);
		rINTCON |= (0x1 << 2);
		//COMPLETAR: poner la línea IRQ en modo no vectorizado

	}
	if (st == ENABLE){
		//COMPLETAR: habilitar la línea IRQ
		rINTCON &=~ (0x1 << 1);
	}
	else{
		//COMPLETAR: deshabilitar la línea IRQ
		rINTCON |= (0x1 << 1);
	}
	//rINTCON = conf; //PREGUNTAR
	return 0;
}

int ic_conf_fiq(enum enable st)
{
	int ret = 0;

	if (st == ENABLE){
		rINTCON &=~ (0x1 << 0);
		//COMPLETAR: habilitar la línea FIQ
	}

	else if (st == DISABLE){
		//COMPLETAR: deshabilitar la línea FIQ
		rINTCON |= (0x1 << 0);
	}
	else{
		ret = -1;
	}
	return ret;
}

int ic_conf_line(enum int_line line, enum int_mode mode)
{
	unsigned int bit = INT_BIT(line);

	if (line < 0 || line > 26)
		return -1;

	if (mode != IRQ && mode != FIQ)
		return -1;

	if (mode == IRQ){
		//COMPLETAR: poner la línea line en modo IRQ

		rINTMOD &=~ (0x1 << line);
	}
	else{
		//COMPLETAR: poner la línea line en modo FIQ
		rINTMOD |= (0x1 << line);
	}
	return 0;
}

int ic_enable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	//COMPLETAR: habilitar las interrupciones por la línea line (INT_BIT(line))
	//habilitar también bit global
	rINTMSK &=~ (0x1 << line );
	rINTMSK &=~ (0x1<< 26 );

	return 0;
}

int ic_disable(enum int_line line)
{
	if (line < 0 || line > 26)
		return -1;

	//COMPLETAR: enmascarar las interrupciones por la línea line (INT_BIT(line))
	rINTMSK |= (0x1 << line);

	return 0;
}

int ic_cleanflag(enum int_line line)
{
	int bit;

	if (line < 0 || line > 26)
		return -1;

	bit = INT_BIT(line);

	if (rINTMOD & bit){
		//COMPLETAR: borrar el flag de interrupción correspondiente a la línea line
		//con la línea configurada por FIQ
		rF_ISPC |= (0x1 << line);
	}
	else{
		//COMPLETAR: borrar el flag de interrupción correspondiente a la línea line
		//con la línea configurada por IRQ
		rI_ISPC |= (0x1 << line);
	}
	return 0;
}



