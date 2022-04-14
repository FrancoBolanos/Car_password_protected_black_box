/* Project: Car Black Box
 * Author: Franco Jesús Bolaños Gainsborg
 * File description: Functions for writing, clearing and reading data from EEPROM
 */

#include <xc.h>
#include "eeprom_data.h"
#include "screens.h"
#include "clcd.h"

void eeprom_save_data(unsigned char* eeprom_pos, unsigned char current_state){ //Saves data in the selected EEPROM position
    unsigned char data[10];
    get_current_data(data, current_state);                      //Gets the current state
    
    if (*eeprom_pos==10){
            *eeprom_pos=0;
    }
    eeprom_write(logpos_address, *eeprom_pos);                  //Saves the next EEPROM position for the next boot of the system
    for (unsigned char i=0; i<10; i++){                         //Saves data in 10 consecutive EEPROM register
        eeprom_write(logdata_address(*eeprom_pos)+i,data[i]);
    }
    *eeprom_pos+=1;                                             //Saves the next EEPROM position for the next call of the function
}

void eeprom_erase_data(unsigned char* eeprom_pos){              //replace the values ​​of registers designated to store events information with 0xFF
    for (unsigned char i=2;i<102;i++){
        eeprom_write(i,0xFF);
    }
    *eeprom_pos=0;
}

void eeprom_show_log(unsigned char* selected_option){            //Reads data from EEPROM according to selected_option value and shows it in CLCD
    unsigned char output[16];
    unsigned char selected_log[10];
    unsigned char index=0;
    unsigned char key;
    
    output[1]=' ';
    output[4]=':';
    output[7]=':';
    output[10]=' ';
    output[13]=' ';
    
        
    index=logdata_address(*selected_option);             //Obtains the address of the firs register related to the "selected_option" event
    
    key=eeprom_read(index);                             //Reads the first EEPROM register related to the selected event
    if (key==0xFF && *selected_option==0){                  //If the selected option is 0 and the read value is 0xFF, screen shows "No logs"
        clcd_print("No logs",LINE2(2));
    } else if (key==0xFF && *selected_option>0){            //Else if the selected option is more than 0 and the read value is 0xFF system returns to the previous value
        *selected_option-=1;
    } else{
        for (unsigned char i=0;i<10;i++){                   //Else reads the registers related to that event and showS them in the CLCD
            selected_log[i]=eeprom_read(index+i);
        }

        output[0]=*selected_option+'0';
        output[2]=selected_log[0];
        output[3]=selected_log[1];
        output[5]=selected_log[2];
        output[6]=selected_log[3];
        output[8]=selected_log[4];
        output[9]=selected_log[5];
        output[11]=selected_log[6];
        output[12]=selected_log[7];
        output[14]=selected_log[8];
        output[15]=selected_log[9];

        clcd_print(output,LINE2(0));
    }
}