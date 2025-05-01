#define F_CPU 16000000UL  // Frecuencia del reloj del sistema
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>  // Para itoa()
#include "Libreria_ADC/LibreriaADC.h"
#include "Libreria_UART/Libreria_UART.h"
#include "Libreria_Timer1/Libreria_Timer1.h"

// Variables lógicas
volatile char previous_answer_c1 = 0; // Respuesta anterior al menú fue 1
volatile char previous_answer_c2 = 0; // Respuesta anterior al menú fue 2

// Variable para lectura de ADCH
volatile char adc_reading = 0;

// Prototipos de funciones
void DisplayInPORTBD(char data);
void ShowMenu(void);
void setup(void);

// INICIALIZACIÓN - SETUP
void setup(void)
{
	// Desactivar interrupciones
	cli();
		
	// Habilitar salida en PD2 a PD5
	DDRD |= (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5);
	
	// Habilitar salida en PB0 a PB3
	DDRB |= 0X0F;
	
	// Inicializar periféricos de librerías
	setup_adc();
	initTimer1();
	UART_init();
	
	// Establecer canal del ADC
	adc_set_channel(0);
	
	// Activar interrupciones
	sei();           
}

// MAINLOOP
int main(void) {
	// Inicialización
	setup();
	UART_sendString("Hola desde UART!\r\n");
	ShowMenu();

	// Ciclo infinito
	while (1);
}

// RUTINAS DE INTERRUPCION
// INTERRUPCIÓN POR RECEPCIÓN USART
ISR(USART_RX_vect)
{
	// Leer carácter recibido
	char data = UDR0;     
	UART_sendString("Caracter Ingresado: ");
	UART_sendChar(data);
	UART_sendString("\r\n");
	
	// Rutina en caso de que ya se haya respondido el caso 1 - Dejar de mostrar números
	if(previous_answer_c1)
	{
		// Dejar de mostrar los valores del ADC y deshabilitar bandera previous_answer_c1 
		previous_answer_c1 = 0x00;
		UART_sendString("Deteniendo el envío de datos del ADC... \r\n");
		UART_sendString("\r\n");
		ShowMenu();
	}
	
	// Rutina en caseo de que ya se haya respondido el caso 2
	else if(previous_answer_c2)
	{
		// Mostrar el caracter ingresado y deshabilitar bandera previous_answer_c2
		previous_answer_c2 = 0x00;
		DisplayInPORTBD(data);
		UART_sendString("Mostrando el caracter puertos D y B... \r\n");
		UART_sendString("\r\n");
		ShowMenu();
	}
	
	else{
		switch(data)
		{
			// CASO "1" - Leer potenciómetro
			case 0x31:
			UART_sendString("Opción 1 seleccionada - Leer Potenciómetro\r\n");
			UART_sendString("Presione cualquier caracter para detener el muestreo");
			previous_answer_c1 = 0x01;
			ADCSRA |= (1 << ADSC); // Iniciar conversión
			break;
			
			// CASO "2" - Mostrar ASCII
			case 0x32:
			UART_sendString("Opción 2 seleccionada - Mostrar ASCII\r\n");
			UART_sendString("Ingrese el caracter que quiere mostrar.");
			previous_answer_c2 = 0x01; // Habilitamos la bandera de previous_answer
			break;
			
			// CASO DEFAULT - Error
			default:
			UART_sendString("La opción seleccionada no es válida\r\n");
			ShowMenu();
			break;
			
		}
	}
	
	UART_sendString("\r\n");
}

// Interrupción del ADC - Lectura de canal 0
ISR(ADC_vect)
{
	adc_reading = ADCH;
	ADCSRA |= (1 << ADSC);					// Iniciar conversión
}


// OVERFLOW DE TIMER1 - Mostrar valores de ADC
ISR(TIMER1_OVF_vect)
{
	if(previous_answer_c1)
	{
		char buffer[5];                          // Suficiente para "FF" + '\0'
		itoa(adc_reading, buffer, 16);          // Convierte adc_reading a string en base 16 (hexadecimal)

		// Asegurar 2 dígitos agregando '0' delante si es menor que 0x10
		if (adc_reading < 0x10) {
			UART_sendChar('0');                 // Agrega cero a la izquierda
		}
		UART_sendString(buffer);                // Envia el número
		UART_sendChar(' ');                     // Espacio separador
	}
	TCNT1 = 34286; // Reiniciar el contador para mantener el periodo
}

// Mostrar datos en PD2-PD5 y PB0-PB3
void DisplayInPORTBD(char data)
{
	// Limpiar los bits PD2 a PD5 de PORTD, sin afectar PD0 y PD1
	PORTD &= 0x03;  // 0x03 es 00000011, no afecta a PD2-PD5

	// Mostrar el nibble bajo de `data` (los 4 bits más bajos) en PORTD (PD2-PD5)
	PORTD |= (data & 0x0F) << 2;  // Escribir solo el nibble bajo (los 4 bits más bajos)

	// Mostrar el nibble alto de `data` (los 4 bits más altos) en PORTB (PB0-PB3)
	PORTB = (data & 0xF0) >> 4;  // Escribir el nibble alto en PB0-PB3
}

// MOSTRAR MENÚ
void ShowMenu(void)
{
	UART_sendString("MENÚ - LABORATORIO 6\r\n");
	UART_sendString("Escoja una opción\r\n");
	UART_sendString("1. Leer Potenciómetro\r\n");
	UART_sendString("2. Enviar ASCII\r\n");
}

