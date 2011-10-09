#include <avr/io.h>
#include <avr/wdt.h>
#include "uart/uart.h"
#include "mu2/mu2.h"
#include "classic/classic.h"
#include "classic/rc_classic.h"

void set_controller();
void get_value(unsigned char *);
void get_classic_analog(const union wii_classic *, unsigned char *);
void MakeClassicRCData(union classic_data *rcdata,const union wii_classic *data, unsigned char *);
void wait(uint16_t);
void battery_check();

int classic(int mode,unsigned char *analog_data)
{
	union wii_classic data;										//receive data for wii classic
	union classic_data rcdata,tmp;									//send data for MU2
	uint8_t i,j;

	set_controller();										//init controller				
	get_value(data.buf);									//get data for controller
	get_classic_analog(&data,analog_data);					//get data for analog stick
	MakeClassicRCData(&rcdata,&data,analog_data);		

 	while(1){
		
		if(mode==0)	{
				for(i=0;i<2;i++){
				MU2_SendData(rcdata.buf,CLASSIC_DATA_LENGTH);//Send Data for MU2
				}
			}
		if(mode==1)	MU2_SendDataBus(rcdata.buf,CLASSIC_DATA_LENGTH);//Send Data for Serial

		for(i=0;i<CLASSIC_DATA_LENGTH;i++){
			tmp.buf[i] = rcdata.buf[i];
		}
		for(j=0;j<3;j++){
			set_controller();										//init controller				
			get_value(data.buf);									//get data for controller
			get_classic_analog(&data,analog_data);					//get data for analog stick
			MakeClassicRCData(&rcdata,&data,analog_data);
			for(i=0;i<CLASSIC_DATA_LENGTH;i++){
				if(tmp.buf[i] != rcdata.buf[i]) break;
			}
			wait(45);
		}												//wait(45ms)
		if(rcdata.detail.Button.START&&rcdata.detail.Button.SELECT) battery_check();//battery check
	}
    return 0;
}
