/* Project: Car Black Box
 * Author: Franco Jesús Bolaños Gainsborg
 * File description: Timer 0 overflow interruption. It reduces the count for the time_out count (for transitinons) and the remaining_time count (composed by rem_min and rem_sec, which restores the attempts)
 */


#include <xc.h>
#include "isr.h"
#include "clcd.h"

extern unsigned int count_rem_time;
extern unsigned char rem_min;
extern unsigned char rem_sec;
extern unsigned char time_out;
extern unsigned int count_out;

__interrupt(low_priority) void isr(void){
    TMR0=131;                               //Sets Timer 0 count register to 131
    if (TMR0IF==1){                         //If Timer 0 overflow generated the interruption:
        TMR0IF=0;                           //Timer 0 overflow interrpt flag is cleared

        if (rem_sec!=0 || rem_min!=0){      //If rem_sec or rem_min are different from 0
            count_rem_time++;                   //Increase count_rem_time
            if (count_rem_time==635){           //If count_rem_time is 635, it is set to 0
                count_rem_time=0;
                if (rem_sec==0){                   //If rem_sec=0: decrease rem_min if it is different from 0 and set rem_sec to 60
                    if (rem_min!=0){
                        rem_min--;
                        rem_sec=60;
                    }
                } else{
                    rem_sec--;                     //else: decrease rem_sec
                }
            }
        } if (time_out!=0){                  //If rem_sec or rem_min are different from 0
            count_out++;                        //Increase count_out
            if (count_out==635){                //If count_out is 635, it is set to 0 and time_out is decreased
                count_out=0;
                time_out--;
            }
        }
        if (time_out==0 && rem_sec==0 && rem_min==0){   //Disable Timer 0 overflow interruption if the counts controlled by Timer 0 are 0
            TMR0IE=0;
        }
    }
}
