
	.equ COEF1, 3483
	.equ COEF2, 11718
	.equ COEF3, 1183
	.equ COEF4, 16384

	.text

	.global rgb2gray
	.global div16384
	.global apply_gaussian
	.global MaximoGris

rgb2gray:
	push {r4,r5,r6,r7,lr}

	mov r1, r0 // se guarda el puntero que le llega por parametro
	ldr r5, =COEF1// se cargan las constantes
	ldr r6,=COEF2
	ldr r7,=COEF3
	ldrb r2, [r1] // se carga el dato(b ya que es char y solo cupa un bit)
	mul r2,r5,r2 //COEF1*pixel->R
	add r1,r1,#1 // sesuma uno a la direccion para acceder al siguiente elemento.
	ldrb r3, [r1]
	mul r3,r6,r3 //COEF2*pixel->R
	add r1, r1,#1
	ldrb r4, [r1]
	mul r4,r7,r4 //COEF3*pixel->R
	add r0,r2,r3 // se suma lo que nos ha dado al multiplicar
	add r0, r0,r4

	bl div16384 // se llama para dividir

	pop {r4,r5,r6,r7,pc}

div16384:

	mov r1 ,#0 // cociente=0
	while:cmp r0, #COEF4 //se guarda 16384
			blt fin_while // cuando sea menor sale
			sub r0, r0, #COEF4 // va restando lo que le pasas por parametro(numerador) menos la constante
			add r1, r1, #1 // se suma 1 para el contador
			b while

		fin_while: // en r1 esta el cociente y se mete en r0 para devolverlo
		 mov r0,r1


	mov pc,lr

apply_gaussian:
	push {r4,r5,r6,r7,lr}

	mov r4,#0// i=0

	for1:cmp r4,r3 // i < height
		bge fin_for1
		mov r5,#0 // j=0

		for2:cmp r5,r2//j < width
			bge fin_for2

			push {r0-r5}//para que no se pierdan los datos en esos registros que hemos tocado antes
			mov r1,r2// en r1 = width
			mov r2,r3 // en r2 = height
			mov r3,r4 // en r3 = i

			push {r5}// para que r5 no se modifique al llamar a gaussian
			bl gaussian
			pop {r5}

			mov r6,r0// el entero que devuelve esa funcion se guarda en r6
			pop {r0-r5}//recupera los registros

			mla r7,r4,r2,r5//i * width + j
			strb r6,[r1,r7]// se guarda en la direccion im2[], en la posicion de i * width + j, y se guarda el registro r6(solo 1 bit ya que es char y ocupa un espacio)
			add r5,r5,#1 // j++
			b for2

			fin_for2:
			add r4,r4,#1//i++
			b for1

		fin_for1:
		pop {r4,r5,r6,r7,lr}
		mov pc,lr

//MaximoGris(imagenGris,N,M,imagenMax);
MaximoGris:
	push {r4-r9}
	//r0 imagen gris
	//r1 filas
	//r2columna
	//r3 direccion de imagenMax

	mov r5, #0 //i=0
	mov r9, #0 //columna max
	forMax1:cmp r5,r1 // i < filas
		bge fin_forMax1
		mov r4, #0 // max=0
		mov r6,#0 // j=0

		forMax2:cmp r6,r2//j < columnas
			bge fin_forMax2

			mla r7,r5,r2,r6//i * width + j
			ldrb r8,[r0,r7]
			cmp r8,r4
			ble incrementoJ
			mov r4,r8
			mov r9, r6
			b incrementoJ

		incrementoJ:
			add r6,r6,#1 // j++
			b forMax2

		fin_forMax2:
			str r9,[r3]
			strb r4,[r3,#4]
			add r3,r3,#8
			add r5,r5,#1//i++
			b forMax1

	fin_forMax1:
	pop {r4-r9}
	mov pc,lr


.end


