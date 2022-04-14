//Provided by Emertxe

#include <xc.h>
#include "keypad.h"
#include "clcd.h"
#include "timers.h"

void init_keypad(void){
        /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES; //TRISB = TRISB | 0x3F
}

unsigned char read_keyboard(unsigned char mode){
    static unsigned char once;
    
    if (mode == LEVEL_DETECTION)
    {
        return KEYPAD_PORT & INPUT_LINES; //PORTB & 0x3F
    }
    else
    {
        if (((KEYPAD_PORT & INPUT_LINES) != ALL_RELEASED) && once)
        {
            once = 0;
            return (KEYPAD_PORT & INPUT_LINES);
        }
        else if ((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED)
        {
            once = 1;
        }
    }
    
    return ALL_RELEASED;
}