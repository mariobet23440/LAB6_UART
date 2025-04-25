#define F_CPU 16000000UL  // Frecuencia del reloj del sistema
#include <avr/io.h>
#include <avr/interrupt.h>

// Inicialización UART a 9600 baudios
void UART_init(void) {
	// Configura TX como salida, RX como entrada
	DDRD |= (1 << PD1);   // TX
	DDRD &= ~(1 << PD0);  // RX

	// Configura baud rate a 9600 (F_CPU = 16 MHz)
	// UBRR = (F_CPU / (16 * BAUD)) - 1 = 103
	UBRR0 = 103;

	// Habilita recepción, transmisión e interrupción de recepción
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// 8 bits, 1 bit de stop, sin paridad
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Enviar un carácter
void UART_sendChar(char c) {
	while (!(UCSR0A & (1 << UDRE0)));  // Espera buffer libre
	UDR0 = c;
}

// Enviar una cadena de texto
void UART_sendString(const char* str) {
	while (*str) 
	{
		UART_sendChar(*str++);
	}
}

// Enviar una cadena de texto (Pero con un arreglo de caracteres)
void UART_sendString_ALT(char str[]) {
	while (*str)
	{
		UART_sendChar(*str++);
	}
}

void DisplayInPORTBD(char data)
{
	// Limpiar los bits PD2 a PD5 de PORTD, sin afectar PD0 y PD1
	PORTD &= 0x03;  // 0x03 es 00000011, no afecta a PD2-PD5

	// Mostrar el nibble bajo de `data` (los 4 bits más bajos) en PORTD (PD2-PD5)
	PORTD |= (data & 0x0F) << 2;  // Escribir solo el nibble bajo (los 4 bits más bajos)

	// Mostrar el nibble alto de `data` (los 4 bits más altos) en PORTB (PB0-PB3)
	PORTB = (data & 0xF0) >> 4;  // Escribir el nibble alto en PB0-PB3
}

void ShowMenu(void)
{
	UART_sendString("Escoja una opción\r\n");
	UART_sendString("1. Leer Potenciómetro\r\n");
	UART_sendString("2. Enviar ASCII\r\n");
}


// Interrupción al recibir carácter
ISR(USART_RX_vect) 
{
	char data = UDR0;     // Leer carácter recibido
	UART_sendString("Caracter Ingresado: ");
	UART_sendChar(data);
	UART_sendString("\r\n");
	switch(data)
	{
		case 0x31:
			UART_sendString("Opción 1 seleccionada - Leer Potenciómetro\r\n");
			break;
			
		case 0x32:
			UART_sendString("Opción 2 seleccionada - Mostrar ASCII\r\n");
			DisplayInPORTBD(data);
			break;
		
		default:
			UART_sendString("La opción seleccionada no es válida\r\n");
			break;
		
	}
	
	ShowMenu();
}

// Siempre vivirá en tus recuer

void setup(void)
{
	// Habilitar salida en PD2 a PD5
	DDRD |= (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) | (1 << PORTD5);
	
	// Habilitar salida en PB0 a PB3
	DDRB |= 0X0F;
}

int main(void) {
	cli();           // Desactiva interrupciones
	setup();
	UART_init();     // Inicializa UART
	sei();           // Activa interrupciones

	UART_sendString_ALT("Hola desde UART!\r\n");
	ShowMenu();

	while (1) {
		// Todo se maneja por interrupción
	}
}
