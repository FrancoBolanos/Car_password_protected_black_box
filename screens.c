/* Project: Car Black Box
 * Author: Franco Jesús Bolaños Gainsborg
 * File description: Functions to get the current data, show the current data on the dashboard and convert from char to string
 */

#include <xc.h>
#include "adc.h"
#include "clcd.h"
#include "keypad.h"
#include "ds1307.h"
#include "screens.h"
#include "timers.h"

unsigned char state_array[]="GNGRG1G2G3G4C ON";

void get_current_data(unsigned char* current_data, unsigned char current_state){    //Reads the Real Time clock, the ADC and the current state to modfy a passed by reference vector with these values that 
    unsigned char clock_reg[3];
    unsigned char speed;
    unsigned char speed_str[2];
    unsigned char current_mins;
    unsigned char current_secs;
    
    get_time(clock_reg);
    speed=read_adc()/10.3;
    num2string(speed_str,speed);
        
    current_data[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    current_data[1] = (clock_reg[0] & 0x0F) + '0';
    current_data[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    current_data[3] = (clock_reg[1] & 0x0F) + '0';
    current_data[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    current_data[5] = (clock_reg[2] & 0x0F) + '0';
    current_data[6] = state_array[current_state*2];
    current_data[7] = state_array[current_state*2+1];
    current_data[8] = speed_str[0];
    current_data[9] = speed_str[1];
}

void show_main_screen(unsigned char current_state){     //Shows the dashboard in the CLCD
    unsigned char output[15];
    unsigned char data[10];
    get_current_data(data, current_state);
    
    output[0] = data[0];
    output[1] = data[1];
    output[2] = ':';
    output[3] = data[2];
    output[4] = data[3];
    output[5] = ':';
    output[6] = data[4];
    output[7] = data[5];
    output[8] = ' ';
    output[9] = data[6];
    output[10] = data[7];
    output[11] = ' ';
    output[12] = data[8];
    output[13] = data[9];
    output[14] = '\0';
    
    clcd_print("TIME     E  SP",LINE1(2));
    clcd_print(output,LINE2(2));
}

void num2string(unsigned char* num_char, unsigned char num){ //Converts a char value to a string value
  unsigned char dec=(num/10)+'0';
  unsigned char uni=(num%10)+'0';
  *num_char++=dec;
  *num_char=uni;
}