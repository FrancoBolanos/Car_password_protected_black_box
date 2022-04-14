/* Project: Car Black Box
 * Author: Franco Jesús Bolaños Gainsborg
 * File description: Functions for writing, clearing and reading data from EEPROM
 */

#ifndef EEPROM_DATA_H
#define	EEPROM_DATA_H

#define pswd_address 0x00
#define logpos_address 0x01
#define logdata_address(x) (0x02+10*x)


void eeprom_save_data(unsigned char*, unsigned char);
void eeprom_erase_data(unsigned char*);
void eeprom_show_log(unsigned char*);

#endif	/* EEPROM_DATA_H */

