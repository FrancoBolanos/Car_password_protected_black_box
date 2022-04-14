/* Project: Car Black Box
 * Author: Franco Jesús Bolaños Gainsborg
 * File description: Timer 0 overflow interruption. It reduces the count for the time_out count (for transitinons) and the remaining_time count (composed by rem_min and rem_sec, which restores the attempts)
 */


#ifndef ISR_H
#define	ISR_H

void __interrupt() isr(void);

#endif	/* ISR_H */

