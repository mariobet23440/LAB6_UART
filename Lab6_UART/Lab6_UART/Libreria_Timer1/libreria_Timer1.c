/*
 * libreria_PWM1.c
 *
 * Created: 10/04/2025 23:18:58
 *  Author: mario
 */ 

#include "libreria_Timer1.h"

// INICIALIZAR TIMER1 (Lo usaremos por precisión)
void initTimer1(void)
{
	TCCR1A = 0; // Modo normal
	TCCR1B = 0;

	TCNT1 = 34286; // Cargar preload (65536 - 31250)

	// Prescaler = 256 ? CS12 = 1
	TCCR1B |= (1 << CS12);

	// Habilitar interrupción por overflow
	TIMSK1 |= (1 << TOIE1);
}


