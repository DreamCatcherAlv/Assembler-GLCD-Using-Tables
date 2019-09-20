; Subrutina encargada del desbloqueo, se irán llamando a las secuencias en orden.
; estas comparan el registro encargado y en caso de estar con el valor correcto
; se aumentará el valor y saldrá de la subrutina, de lo contrario saldrá sin realizar 
; ningún cambio
SRDesbloqueo:
	cpi regDesbloqueo,0
	breq SRDesbloqueoAumento
	ret

SRDesbloqueo1:
	cpi regDesbloqueo,1
	breq SRDesbloqueoAumento
	ret

SRDesbloqueo2:
	cpi regDesbloqueo,2
	breq SRDesbloqueoAumento
	ret
SRDesbloqueo3:
	cpi regDesbloqueo,3
	breq SRDesbloqueoAumento
	ret

SRDesbloqueoAumento:
	inc regDesbloqueo
	ret

; Subrutina encargada de leer el número de veces que se ha pulsado
; un número en la pantalla. 

SRContador:
	inc regVecesPulsado

	; Comprueba el valor del registro de desbloqueo, si es correcto
	; llama a la etiqueta desbloquear.

	cpi regDesbloqueo,4
	breq SRDesbloquear

	; Comprueba el valor del registro de pulsaciones, en caso de haber
	; pulsado 4 veces saltará a la etiqueta Fallo

	cpi regVecesPulsado,4
	breq SRFallo
	ret

; Subrutina de desbloqueo

SRDesbloquear:

	; Pinta en la GLCD la imagen de desbloqueo

	rcall SRDesbloqueado
	rcall RetardoLargo

	; Setea los valores de las cuentas a 0

	ldi regDesbloqueo,0
	ldi regVecesPulsado,0
	ret

; Subrutina de fallo

SRFallo:
	; Pinta en la GLCD la imagen de failed

	rcall Fallado
	rcall RetardoLargo
	
	; Setea los valores de las cuentas a 0

	ldi regDesbloqueo,0
	ldi regVecesPulsado,0
	ret

;Comentar aquí

SRNumeros:
	ldi ZH,HIGH(Numeros*2)
	ldi ZL,LOW(Numeros*2)
	rcall SRPintar
	ret

SRLimpiar:
	
	ldi ZH,HIGH(Blanco*2)     ;El puntero tendrá la dirección de la tabla para el menú 
	ldi ZL,LOW(Blanco*2)
	rcall SRPintar
	ret

Fallado:
	ldi ZH,HIGH(Failed*2)     ;El puntero tendrá la dirección de la tabla para el menú 
	ldi ZL,LOW(Failed*2)
	rcall SRPintar
	rcall RetardoLargo
	ret

SRDesbloqueado:
	
	ldi ZH,HIGH(Unlocked*2)     ;El puntero tendrá la dirección de la tabla para el menú 
	ldi ZL,LOW(Unlocked*2)
	rcall SRPintar
	rcall RetardoLargo
	ret

; Rutina para enviar imágenes a la pantalla

SRPintar:
	; Guarda los registros en la pila

	push regUsoGeneral
	push r20 
	push r21 
	
	; Asociamos los valores a los registros

	ldi r21, 64  
	ldi r20, 0

Return: 
	; Pintamos la pantalla izquierda

	rcall IZQ
	rcall Dibujar  
	
	; Pintamos la pantalla derecha

	rcall DER  
	rcall Dibujar  
	
	; Incrementamos el valor del registro asociado al eje vertical
	; y comparamos, si superamos el numero de páginas se empieza de 0

	inc r20   
	cpi r20,8
	brne Return 
	pop r21   
	pop r20 
	pop regUsoGeneral
	ret

; Configura la GLCD para empezar a escribir en la pantalla izquierda

IZQ:   

	; Guardamos los valores en la pila
	push regUsoGeneral
	push r20 

	rcall SRPantallaIzquierdaOn
	ldi regUsoGeneral,0b01000000 
	out PORTC, regUsoGeneral
	rcall SRModoConfig	
	ldi regUsoGeneral,0b10111000 
	or regUsoGeneral,r20 
	out PORTC, regUsoGeneral
	rcall SRModoConfig
	
	; Recuperamos los valores de la pila
	pop r20    
	pop regUsoGeneral
	ret


DER:   
	; Guardamos los valores en la pila

	push regUsoGeneral
	push r20 
	rcall SRPantallaDerechaOn
	ldi regUsoGeneral,0b01000000 
	out PORTC, regUsoGeneral
	rcall SRModoConfig
	ldi regUsoGeneral,0b10111000 
	or regUsoGeneral,r20 
	out PORTC, regUsoGeneral;R20 indicará la página a dibujar 
	rcall SRModoConfig
	pop r20  ;Se recupera los valores de los registros 
	pop regUsoGeneral
	ret

Dibujar:;Subrutina de apoyo para dibujar imagenes, una vez ajustado las direcciones y la    ;pantalla activada se procede a rellenar la página de los 64 pixeles. 
	push r0 ;Se guarda los valores de los registros en la pila 
	push r21 
ReDibujar: 
	lpm;Carga a R0 el valor de la dirección apuntada por el puntero. 
	out PORTC, r0 
	rcall Envio  ;Y se muestra por pantalla 
	adiw ZL,1  ;Se aumenta la dirección donde apunta el puntero 
	dec r21    
	brne ReDibujar  ;Cuando se haya dibujado 64 direcciones Y se terminará el bucle 
	pop r21 ;Se recupera los valores de los registros 
	pop r0 
	ret



SRPantallaIzquierdaOn:

	cbi PORTB,0;
	sbi PORTB,1
	ret

SRPantallaDerechaOn:

	sbi PORTB,0
	cbi PORTB,1
	ret

SRModoConfig:

	cbi PORTA,2
	cbi PORTA,3
	sbi PORTD,6
	rcall Retardo100us
	cbi PORTD,6
	rcall Retardo100us

	ret

Envio:

	sbi PORTA,2
	cbi PORTA,3
	sbi PORTD,6
	rcall Retardo100us
	cbi PORTD,6
	rcall Retardo100us
	ret

Leer:
	push regUsoGeneral

	ldi regUsoGeneral,0	;PC todo salida
	out DDRC, regUsoGeneral
	sbi PORTA,2
	sbi PORTA,3
	sbi PORTD,6
	rcall Retardo100us
	cbi PORTD,6
	rcall Retardo100us
	sbi PORTD,6
	rcall Retardo100us
	in r28, PINC
	cbi PORTD,6
	rcall Retardo100us
	ldi regUsoGeneral,255	;PC todo salida
	out DDRC, regUsoGeneral

	pop regUsoGeneral

	ret
RetardoLargo:
	ldi r25, 255
bucleL1:
	dec r25
	ldi r26, 255
	rcall Retardo100ms
	rcall Retardo100ms
	subi r25, 0
	brne bucleL2
	ret
bucleL2:
	ldi r27, 255
	rcall Retardo100ms
	subi r26, 0
	breq bucleL1
	dec r26
bucleL3:
	dec r27
	subi r27,0
	breq bucleL2
	rcall Retardo100ms
	rjmp bucleL3

Retardo100us:
	ldi r25, 2
bucle1100us:
	dec r25
	ldi r26, 10
	subi r25, 0
	brne bucle2100us
	ret
bucle2100us:
	ldi r27, 1
	subi r26, 0
	breq bucle1100us
	dec r26
bucle3100us:
	dec r27
	subi r27,0
	breq bucle2100us
	rjmp bucle3100us

Retardo4ms:
	ldi r25, 2
bucle14ms:
	dec r25
	ldi r26, 3
	subi r25, 0
	brne bucle24ms
	ret
bucle24ms:
	ldi r27, 255
	subi r26, 0
	breq bucle14ms
	dec r26
bucle34ms:
	dec r27
	subi r27,0
	breq bucle24ms
	rjmp bucle34ms

Retardo100ms:
	ldi r25, 200
bucle1100ms:
	dec r25
	ldi r26, 10
	subi r25, 0
	brne bucle2100ms
	ret
bucle2100ms:
	ldi r27, 255
	subi r26, 0
	breq bucle1100ms
	dec r26
bucle3100ms:
	dec r27
	subi r27,0
	breq bucle2100ms
	rjmp bucle3100ms