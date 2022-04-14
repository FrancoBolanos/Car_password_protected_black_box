/* Project: Car Black Box
* Author: Franco Jesús Bolaños Gainsborg
* Developed using MPLAB XPRESS IDE and MPLABX IDE
* Description: Car black box password protected interface that allows to save, read and delete events records from EEPROM.
* Each time the gear state changes, the current time, speed and gear state are saved in the EEPROM. Password is read from EEPROM
* Only the last 10 events are saved in EEPROM. The oldest event is replaced by the newest.
*
* Details:
* Microcontroller: PIC18F877A
* Oscillator frequency: 20 MHz
* Potentiometer connected to AN0 controls speed
* RB0: Generate crash state
* RB1 and RB2: Generate gear state change
* RB3 or RB4: Access login screen
* The 4 password digit is a combination of RB3 and RB4. RB5 allows you to go back. If no button is pressed for 3 seconds, system returns to dashboard
* After 3 wrong password attempts, login screen is blocked for 15 minutes
*
* In menu screen: RB3 allows to go up, RB4 allows to go down and RB5 allows to select a logs_option
* In view logs option: RB3 allows to go up, RB4 allows to go down and RB5 allows to return to the dashboard
* In clear logs option: the EEPROM registers designed to hold the events records
* In change password: System asks to enter a password twice. If they match, access password is changed and saved in EEPROM
*/



#include <xc.h>
#include "clcd.h"
#include "adc.h"
#include "timers.h"
#include "keypad.h"
#include "i2c.h"
#include "ds1307.h"
#include "screens.h"
#include "eeprom_data.h"

#define _XTAL_FREQ 20000000
#pragma config WDTE = OFF

#define dashboard_flag 0x00
#define login_flag 0x01
#define warning_flag 0x02
#define blocked_flag 0x03
#define menu_flag 0x04
#define view_logs_flag 0x05
#define clear_logs_flag 0x06
#define change_pwd_flag 0x07
#define change_pwd_flag_2 0x08
#define pwd_changed_flag 0x09
#define pwd_not_changed_flag 0x0A

    //general variables
    unsigned char keypad_value;
    unsigned char screen_flag;
    unsigned char attempts;
    unsigned char logpos;   
    
    //dashboard variables
    unsigned char current_state;
    
    //password variables
    unsigned char password_characters;
    unsigned char entered_keypad;
    unsigned char entered_password;
    unsigned char correct_password;
    unsigned char entered_password_2;
    
    //blocked state variables
    unsigned char output_blocked[7];
    unsigned char rem_time_str[2];
    unsigned char rem_min;
    unsigned char rem_sec;
    unsigned int count_rem_time;
    
    //menu variables
    unsigned char menu_option;
    
    //logs viewer variables
    unsigned char logs_option;
    
    //countdown for changing some screens variables
    unsigned char time_out;
    unsigned int count_out;

void init_config(void){
    //configuration
    init_i2c(100000);   //100K
    config_timer0();
    init_ds1307();
    init_keypad();
    init_clcd();
    init_adc();
    GIE=1;
    
    //initial values
    keypad_value=0;
    screen_flag=dashboard_flag;
    current_state=7;            //ON state
    attempts=3;
    
    count_out=0;
    time_out=0;
    
    count_rem_time=0;
    rem_min=0;
    rem_sec=0;
    
    entered_password=0;
    entered_password_2=0;
    password_characters=0;
    entered_keypad=0;

    output_blocked[2]=' ';
    output_blocked[6]='\0';
    
    menu_option=0;
    logs_option=0;

    //Password is read from EEPROM. If there is no password, temporary password access is RB3,RB3,RB3,RB3
    correct_password=eeprom_read(pswd_address);
    if (correct_password==0xFF){
        correct_password=14;
    }    
    
    //Next event record position is read from EEPROM to ensure replacing the oldest event
    logpos=eeprom_read(logpos_address);
    if (logpos==0xFF){
        logpos=0;
    } else {
        logpos++;
    }
}

void main(void) {
    init_config();
    eeprom_save_data(&logpos,current_state);    // ON state is saved in the EEPROM
    
    while(1){
        keypad_value=read_keyboard(STATE_DETECTION);    //PORTB is read. If a button is pressed it returns the value only once until all buttons are released
        keypad_value=~keypad_value;                     //Buttons have pul-up configuration. It is easier to work with the pull-down equivalent
        for (int j=3000;j--;0);                         //Debounce
        
        //Screens
        //switch statement to decide which screen is showed
        switch (screen_flag){
            case dashboard_flag:
                show_main_screen(current_state);
                break;
            case warning_flag:
                clcd_print("WRONG PASSWORD",LINE1(1));
                clcd_putch(attempts+'0',LINE2(0));    //Shows reamaining attempts
                clcd_print("Attempt remain",LINE2(2));
                break;
            case blocked_flag:
                clcd_print("You are blocked",LINE1(0));
                clcd_print("wait...",LINE2(0));
                if (rem_min>0){
                    num2string(rem_time_str,rem_min);   //remaining time decreased by timer 0 isr
                    output_blocked[0]=rem_time_str[0];
                    output_blocked[1]=rem_time_str[1];
                    output_blocked[3]='m';
                    output_blocked[4]='i';
                    output_blocked[5]='n';
                } else{
                    num2string(rem_time_str,rem_sec);      //remaining time decreased by timer 0 isr
                    output_blocked[0]=rem_time_str[0];
                    output_blocked[1]=rem_time_str[1];
                    output_blocked[3]='s';
                    output_blocked[4]='e';
                    output_blocked[5]='c';
                }
                clcd_print(output_blocked,LINE2(8));
                break;
            case menu_flag:
                if (menu_option==0){
                    clcd_print("* View Logs ",LINE1(0));
                    clcd_print("  Clear Logs ",LINE2(0));
                } else if (menu_option==1){
                    clcd_print("  View Logs ",LINE1(0));
                    clcd_print("* Clear Logs ",LINE2(0));
                } else if (menu_option==2){
                    clcd_print("  Clear Logs",LINE1(0));
                    clcd_print("* Change Pwd  ",LINE2(0));
                }
                break;
            case view_logs_flag:
                clcd_print("# TIME     E  SP",LINE1(0));
                eeprom_show_log(&logs_option);               //Read EEPROM and show selected record
                break;
            case clear_logs_flag:
                clcd_print("Logs cleared",LINE1(1));
                clcd_print("successfully",LINE2(1));
                break;
            case pwd_changed_flag:
                clcd_print("PASSWORD",LINE1(4));
                clcd_print("CHANGED",LINE2(4));
                break;
            case pwd_not_changed_flag:
                clcd_print("PASSWORDS",LINE1(3));
                clcd_print("DOESN'T MATCH",LINE2(1));
                break;
        }
        
        //Screen-dependent tasks

        if (keypad_value & 0b11000 && (screen_flag==dashboard_flag || screen_flag==login_flag)){    //If RB3 or RB4 are pressed
            switch (screen_flag){
                case dashboard_flag:                                //If current screen is dashboard
                    clcd_clear();
                    if (attempts){                                  //If there are remaining attempts
                        entered_password=0;                         //Go to login screen and set necessary parameters
                        password_characters=0;
                        screen_flag=login_flag;
                        start_timer0_time_out(5);                   //Set time to return to dashboard
                        clcd_print("ENTER PASSWORD",LINE1(1));
                        clcd_write(LINE2(6),INST_MODE);
                        clcd_write(0x0F,INST_MODE);
                    } else{
                        screen_flag=blocked_flag;                   //Else, go to block screen
                        start_timer0_time_out(3);                   //Set time to return to dashboard
                    }
                    break;
                case login_flag:                                    //If current screen is login screen
                    entered_keypad=((keypad_value>>3) & 0b11);      //Shifting the read PORTB register and masking it
                    if (entered_keypad & 0b11){                     //If RB3 or RB4 were pressed, add a digit to the password (actually it is a sum with multiplication) 
                        start_timer0_time_out(5);                   //Reset time to return to dashboard
                        entered_password+=entered_keypad*(password_characters+2);
                        password_characters++;
                        clcd_print("*",LINE2(5+password_characters));
                    }
                    if(password_characters==4){                     //If entered password has 4 digits, check the password
                        clcd_write(0x0C,INST_MODE);
                        clcd_clear();
                        if (entered_password!=correct_password){    //If password is incorrect show the remaining attempts
                            attempts--;
                            screen_flag=warning_flag;
                            start_timer0_time_out(3);               //Set time to return to dashboard
                            start_timer0_recover_attempts(15,0);    //Set remaining time to restore the attempts. Login is not blocked, but since there are less than 3 attempts, time starts to run. Time resets to 15 minutes if a new attempt is done
                            if (attempts==0){                       //If there are 0 attempts, system replaces warning screen with block screen
                                screen_flag=blocked_flag;
                            }
                        } else{                                     //If the password is correct, remaining time is set to 0, attempts are restored and system goes to the menu
                            rem_min=0;
                            rem_sec=0;
                            attempts=3;
                            menu_option=0;
                            screen_flag=menu_flag;
                        }
                        password_characters=0;
                        entered_password=0;
                    }
                    break;
            }
        } else if(screen_flag==login_flag && keypad_value & 0b100000){ //If RB5 is pressed while in login screen, system returns to dashboard
            time_out=0;
        } 
        
        //On the menu screen
        else if (screen_flag==menu_flag){
            if (keypad_value & 0b10000){        //Go to the next menu option(go down in the screen)
                if (menu_option<2){
                    menu_option++;
                }
            } else if (keypad_value & 0b1000){  //Go to the previous menu option (go up in the screen)
                if (menu_option>0){
                    menu_option--;
                }   
            } else if (keypad_value & 0b100000){        //Select the current menu option
                clcd_clear();
                switch (menu_option){
                    case 0:                             //If menu option is 0, system goes to view logs screen
                        logs_option=0;
                        screen_flag=view_logs_flag;
                        break;
                    case 1:
                        screen_flag=clear_logs_flag;    //If menu option is 1, system clear the logs records in EEPROM and sets next event record position to 0
                        eeprom_erase_data(&logpos);
                        start_timer0_time_out(3);
                        break;
                    case 2:
                        entered_password=0;             //If menu option is 2, system goes to the change password screen
                        entered_password_2=0;
                        password_characters=0;
                        clcd_print("ENTER PASSWORD",LINE1(1));
                        clcd_write(LINE2(6),INST_MODE);
                        clcd_write(0x0F,INST_MODE);
                        screen_flag=change_pwd_flag;
                        break;
                }
            }
        } 
        
        // View logs screen
        else if (screen_flag==view_logs_flag){
            if (keypad_value & 0b1000){             //Increase the selected record number
                if (logs_option<9){
                    logs_option++;
                }
            } else if (keypad_value & 0b10000){     //Increase the selected record number
                if (logs_option>0){
                    logs_option--;
                }   
            } else if (keypad_value & 0b100000){    //Return to dashboard
                clcd_clear();
                screen_flag=dashboard_flag;
            }
        }
        
        // Change password screen (Follows the same process as on the login screen, but does not return to dashboard and entered password is saved in another variable)
        else if (keypad_value & 0b11000 && screen_flag==change_pwd_flag){
            entered_keypad=((keypad_value>>3) & 0b11);
            if (entered_keypad & 0b11){
                entered_password+=entered_keypad*(password_characters+2);
                password_characters++;
                clcd_print("*",LINE2(5+password_characters));
            } if (password_characters==4){
                password_characters=0;
                clcd_clear();
                clcd_write(0x0C,INST_MODE);
                clcd_print("RE-ENTER PSWD",LINE1(1));
                clcd_write(LINE2(6),INST_MODE);
                clcd_write(0x0F,INST_MODE);
                screen_flag=change_pwd_flag_2;
            }
        } 

        // Change password screen (Password confirmation) (Follows the same process as on the login screen, but does not return to dashboard and entered password is saved in another variable)
        else if (keypad_value & 0b11000 && screen_flag==change_pwd_flag_2){
            entered_keypad=((keypad_value>>3) & 0b11);
            if (entered_keypad & 0b11){
                entered_password_2+=entered_keypad*(password_characters+2);
                password_characters++;
                clcd_print("*",LINE2(5+password_characters));
            } if (password_characters==4){
                clcd_write(0x0C,INST_MODE);
                clcd_clear();
                if (entered_password==entered_password_2){
                    eeprom_write(pswd_address,entered_password);
                    correct_password=entered_password;
                    screen_flag=pwd_changed_flag;
                } else{
                    screen_flag=pwd_not_changed_flag;
                }
                entered_password=0;
                entered_password_2=0;
                password_characters=0;
                start_timer0_time_out(3); //Set time to return to dashboard for next screen
            }
        }
        
        //TImer related tasks
        //Count for transitions

        if (screen_flag==login_flag && time_out==0){            //If current screen is login screen and the count time_out reaches 0, system returns to dashboard
            clcd_write(0x0C,INST_MODE);
            screen_flag=dashboard_flag;
            clcd_clear();
        } else if (screen_flag==warning_flag && time_out==0){   //If current screen is warning screen and the count time_out reaches 0, system returns to dashboard
            entered_password=0;
            password_characters=0;
            clcd_clear();
            clcd_print("ENTER PASSWORD",LINE1(1));
            clcd_write(LINE2(6),INST_MODE);
            clcd_write(0x0F,INST_MODE);
            screen_flag=login_flag;
            start_timer0_time_out(5);
        } else if (screen_flag==blocked_flag && time_out==0){           //If current screen is block screen and the count time_out reaches 0, system returns to dashboard
            screen_flag=dashboard_flag;
            clcd_clear();
        } else if (screen_flag==clear_logs_flag && time_out==0){        //If current screen is clear logs screen and the count time_out reaches 0, system returns to dashboard
            screen_flag=dashboard_flag;
            clcd_clear();
        } else if (screen_flag==pwd_changed_flag && time_out==0){       //If current screen is pwd_changed_flag screen and the count time_out reaches 0, system returns to dashboard
            screen_flag=dashboard_flag;
            clcd_clear();
        } else if (screen_flag==pwd_not_changed_flag && time_out==0){   //If current screen is pwd_changed_flag screen and the count time_out reaches 0, system returns to dashboard
            screen_flag=dashboard_flag;
            clcd_clear();
        }
        

        if (attempts!=3 && rem_sec==0 && rem_min==0){   //Attempts restoration when remaining time is 0
            attempts=3;
        }

        //Screen-independent tasks

        if (keypad_value & 0b10){                       //Swap current gear state for the next gear state in the vector of gear states and save event in EEPROM
            current_state++;
            if (current_state>=6){
                current_state=0;
            }
            eeprom_save_data(&logpos,current_state);
        } else if (keypad_value & 0b100){               //Swap current gear state for the previous gear state in the vector of gear states and save event in EEPROM
            current_state--;
            if (current_state>=6){
                current_state=5;
            }
            eeprom_save_data(&logpos,current_state);
        }   else if (keypad_value & 0b1){               //Generate crash state and save event in EEPROM
            current_state=6;
            eeprom_save_data(&logpos,current_state);
        }
    }
    return;
}






