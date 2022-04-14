/* Project: Car Black Box
 * Author: Franco Jesús Bolaños Gainsborg
 * File description: Timer 0 overflow interruption configuration, minutes and seconds countdown initialization function for recovering attempts using timer 0 and 
 * seconds countdown initialization function for screen transition using timer 0.
 * Go to isr.c to read how the counts work
 */

#ifndef TIMERS_H
#define	TIMERS_H
    
void config_timer0(void);
void start_timer0_recover_attempts(unsigned char, unsigned char);
void start_timer0_time_out(unsigned char);

#endif

