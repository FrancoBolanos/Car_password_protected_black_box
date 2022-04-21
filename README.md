# Car_password_protected_black_box
Hex file is included  
The program can be simulated downloading https://sourceforge.net/projects/picsim/  
-Select PicGenios board  
-Select oscillator frequency: 20 MHz  
-Select microcontroller: PIC6F877A  
-Load the .hex file  
  
Project: Car Black Box  
Author: Franco Jesús Bolaños Gainsborg  
Developed using MPLAB XPRESS IDE and MPLABX IDE  
Description: Car black box password protected interface that allows to save, read and delete events records from EEPROM.   Each time the gear state changes, the current time, speed and gear state are saved in the EEPROM. Password is read from EEPROM. Only the last 10 events are saved in EEPROM. The oldest event is replaced by the newest.  
  
Details:  
Microcontroller: PIC18F877A  
Oscilator frquency: 20 MHz  
Potentiometer connected to AN0 controls speed  
RB0: Generate crash state  
RB1 and RB2: Generate gear state change  
RB3 or RB4: Access login screen  
4 password digit is a combination of RB3 and RB4. RB5 allows to go back. If no button is pressed for 3 seconds, system returns to dashboard. After 3 wrong password attempts, login screen is blocked for 15 minutes.  

In menu screen: RB3 allows to go up, RB4 allows to go down and RB5 allows to select a logs_option  
In view logs option: RB3 allows to go up, RB4 allows to go down and RB5 allows to return to the dashboard  
In clear logs option: the EEPROM registers designed to hold the events records  
In change password: System asks to enter a password twice. If they match, access password is changed and saved in EEPROM  
