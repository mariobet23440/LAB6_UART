/*
 * EjemploUSART.c
 *
 * Created: 9/04/2025 16:54:47
 * Author : mario
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// FUNCIONES DE USART

void initUART() {
    // Paso 1: Configurar pines PD1 (TX) como salida y PD0 (RX) como entrada
    DDRD |= (1 << DDD1);   // TX (PD1) como salida
    DDRD &= ~(1 << DDD0);  // RX (PD0) como entrada

    // Paso 2: UCSR0A
    UCSR0A = 0;

    // Paso 3: Habilitar recepción, transmisión y la interrupción por recepción
    UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

    // Paso 4: Configurar 8 bits de datos, 1 bit de parada, sin paridad
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    // Paso 5: Configurar baud rate a 9600 (para F_CPU = 16MHz -> UBRR0 = 103)
    UBRR0 = 103;
}

void writeChar(char caracter) {
    while (!(UCSR0A & (1 << UDRE0)));  // Esperar a que el buffer esté listo
    UDR0 = caracter;                   // Enviar carácter
}

void writeString(char* texto) {
    for (unsigned int i = 0; texto[i] != '\0'; i++) {
        writeChar(texto[i]);
    }
}

// SETUP Y LOOP PRINCIPAL

void setup(void) {
    cli();       // Desactivar interrupciones globales
    initUART();  // Inicializar USART
    sei();       // Activar interrupciones globales
}

int main(void) {
    setup();
    writeString("HOLA\r\n");

    while (1);  // Loop principal (espera pasiva)
}

// INTERRUPCIÓN AL RECIBIR DATOS

ISR(USART_RX_vect) {
    char recibido = UDR0;  // Leer carácter recibido
    writeString("Texto:");
    writeChar(recibido);   // Reenviar el carácter recibido
    writeChar('\r');
    writeChar('\n');
}
