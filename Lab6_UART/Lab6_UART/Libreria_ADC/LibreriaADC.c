/*
 * LibreriaADC.c
 *
 * Created: 11/04/2025 13:14:17
 *  Author: mario
 */ 

#include "LibreriaADC.h"

void setup_adc(void)
{
	ADMUX |= (1 << ADLAR) | (1 << REFS0);   // Ajuste a la izquierda (ADLAR), Referencia AVCC (REFS0)
	ADCSRA |= (1 << ADEN) | (1 << ADIE);    // Habilitar ADC y habilitar interrupción ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // Prescaler de 64
	ADCSRA |= (1 << ADSC);					// Iniciar conversión
}

void adc_set_channel(unsigned char channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Limpiar bits de canal y asignar nuevo canal
}
