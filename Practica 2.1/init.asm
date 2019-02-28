/*-----------------------------------------------------------------
**
**  Fichero:
**    init.asm  10/6/2014
**
**    Estructura de Computadores
**    Dpto. de Arquitectura de Computadores y Automática
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Arranca un programa en C
**
**  Notas de diseño:
**
**---------------------------------------------------------------*/

    .extern main
	.global start
	.extern button_ISR
	.extern timer_ISR
	.extern ic_cleanflag
	
	/* Tabla en la que escribir las direcciones de las ISRs */
	.equ _ISR_STARTADDRESS, 0xc7fff00
	.equ HandleIRQ, _ISR_STARTADDRESS+4*6

	/* Constantes utiles para la rutina ISR_FIQ */
	.equ I_ISPR, 0x01E00020
	.equ I_ISPC, 0x01E00024

	/*
	** Modos de operación
	*/
	.equ MODEMASK, 0x1f		/* Para selección de M[4:0] del CPSR */
	.equ USRMODE,  0x10
	.equ FIQMODE,  0x11
	.equ IRQMODE,  0x12
	.equ SVCMODE,  0x13
	.equ ABTMODE,  0x17
	.equ UNDMODE,  0x1b
	.equ SYSMODE,  0x1f

	/*
	** Direcciones de las bases de las pilas del sistema 
	*/
	.equ USRSTACK, 0xc7ff000   	
	.equ SVCSTACK, 0xc7ff100
	.equ UNDSTACK, 0xc7ff200
	.equ ABTSTACK, 0xc7ff300
	.equ IRQSTACK, 0xc7ff400
	.equ FIQSTACK, 0xc7ff500

	/*
	** Registro de máscara de interrupción
	*/
	.equ rINTMSK,    0x1e0000c
	.equ rI_ISPC,    0x1e00024
	.equ rF_ISPC,    0x1e0003c
	.equ rEXTINTPND, 0x1d20054

start:

	/* Pasa a modo supervisor */
    mrs	r0, cpsr
    bic	r0, r0, #MODEMASK
    orr	r1, r0, #SVCMODE
    msr	cpsr_c, r1 

	/* Inicialización de la sección bss a 0, estándar C */
    ldr	    r3, =Image_ZI_Base
	ldr	    r1, =Image_ZI_Limit	/* Top of zero init segment */
    mov	    r2, #0
L0:
    cmp	    r3, r1	    		/* Zero init */
    strcc   r2, [r3], #4
    bcc	    L0
	/****************************************************/

	/* Desde modo SVC inicializa los SP de todos los modos de ejecución privilegiados */
    bl InitStacks

	ldr r0, =rEXTINTPND
	ldr r1, =0xff
	str r1, [r0]
	ldr r0, =rI_ISPC
	ldr r1, =0x1fffffff
	str r1, [r0]
	ldr r0, =rF_ISPC
	ldr r1, =0x1fffffff
	str r1, [r0]

	/* Enmascara interrupciones */
	ldr r0, =rINTMSK
	ldr r1, =0x1fffffff
    str r1, [r0]

	/* Habilita línea IRQ y FIQ del CPSR */
	mrs r0, cpsr
	bic r0, r0, #0xC0
	msr cpsr_c, r0
	
	/* Añade dirección irq_ISR en la tabla de direcciones a RTIs*/
	ldr r0,=irq_ISR
	ldr r1,=HandleIRQ
	str r0,[r1]

	/* Desde modo SVC cambia a modo USR e inicializa el SP_usr */
	mrs r0, cpsr
	bic r0, r0, #MODEMASK
	orr r1, r0, #USRMODE  
	msr cpsr_c, r1
	ldr sp, =USRSTACK

    mov fp, #0

    bl main

End:
    B End

InitStacks:
	mrs r0, cpsr
	bic r0, r0, #MODEMASK

	orr r1, r0, #UNDMODE  /* desde modo SVC cambia a modo UND e inicializa el SP_und */
	msr cpsr_c, r1    
	ldr sp, =UNDSTACK

	orr r1, r0, #ABTMODE  /* desde modo UND cambia a modo ABT e inicializa el SP_abt */
	msr cpsr_c, r1 
	ldr sp, =ABTSTACK

	orr r1, r0, #IRQMODE  /* desde modo ABT cambia a modo IRQ e inicializa el SP_abt */
	msr cpsr_c, r1
	ldr sp, =IRQSTACK

	orr r1, r0, #FIQMODE  /* desde modo IRQ cambia a modo FIQ e inicializa el SP_fiq */
	msr cpsr_c, r1
	ldr sp, =FIQSTACK

	orr r1, r0, #SVCMODE  /* desde modo FIQ cambia a modo SVC e inicializa el SP_svc */
	msr cpsr_c, r1
	ldr sp, =SVCSTACK
	
    mov pc, lr

//irq_ISR:
	//COMPLETAR: debemos ver si la interrupción que debemos atender (bit) es la
	//de la línea INT_TIMER0 and INT_EINT4567. Si es la del timer se invocará la
	//función timer_ISR y después se borrará el flag de interrupción utilizando
	//el interfaz definido en intcontroller.h. Si es la de EINT4567 se invocará
	//la función button_ISR y se borrará el flag correspondiente utilizando el
	//mismo interfaz.
	//Recordad que en el prólogo hay que guardar todos los registros usados+lr si es no hoja*/
	//Antes de la llamada a otra función hay que guardar los registros de R0 a R3



	// tengo que comprobar que si el timer o el pulsador  han interrumpido. se mira en el registro
	// ISPR.Si el bit que codifica la linea de interrupvione sta a 1,hay una peticion. Vamos a la
	//subrutina que trata esa inerrupcion, y luego borramos los flags.

irq_ISR:
		PUSH {R4,R5,R6,R7,R8,LR}
		MOV R4,#0			     	@R4=21 es el contador para comparar con el pulsador(r6)
		LDR R7,=I_ISPR
		LDR R7,[R7]
	    AND R5,R7,#0X00200000;	@MSK para el pulsador
	    AND R6,R7,#0X00002000;	@MSK para el timer
	    CMP R5,R4					@PRIMER IF PARA EL PULSADOR
		BEQ else1					@salta si son iguales
		PUSH {R0,R1,R2,R3}
		BL button_ISR				@Slata a la subturina button
		POP {R0,R1,R2,R3}
		PUSH {R0,R1,R2,R3}
		MOV R0,#21					@pasar param a la subrutina ic_cleanflag
		BL ic_cleanflag				@limpiar los flags de button
		POP {R0,R1,R2,R3}
		else1:CMP R4, R6
			 BEQ else2               @salta sin son iguales
		     PUSH {R0,R1,R2,R3}
		  	 BL timer_ISR			@Salta a la subturina button
		     POP {R0,R1,R2,R3}
			 PUSH {R0,R1,R2,R3}
			 MOV R0,#13				@pasar param a la subrutina ic_cleanflag
			 BL ic_cleanflag		@limpiar los flags de button
			 POP {R0,R1,R2,R3}
			else2:	 POP {R4,R5,R6,R7, R8, LR}		@Restauramos
			         MOV PC, LR					@Volvemos al programa principal
	.end

