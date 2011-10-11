#include "uart/uart.h"
#include "mu2/mu2_common.h"
#include "mu2/mu2_data.h"
#include "classic/classic.h"
#include "classic/rc_classic.h"

void setup();
void set_controller();
void get_value(unsigned char *);
void get_classic_analog(const union wii_classic *, unsigned char *);
void MakeClassicRCData(union classic_data *rcdata,const union wii_classic *data, unsigned char *);
void wait();
void battery_check();

int classic(int mode,unsigned char *analog_data)
{
	int i; 

	union wii_classic data;										//receive data for wii classic
	union classic_data rcdata;									//send data for MU2

 	while(1){
		set_controller();										//init controller				
		get_value( data.buf );									//get data for controller
		get_classic_analog(&data,analog_data);					//get data for analog stick
		MakeClassicRCData(&rcdata,&data,analog_data);		
		if(mode==0)	MU2_SendData(rcdata.buf,CLASSIC_DATA_LENGTH);//Send Data for MU2
		if(mode==1)	for(i=0;i<CLASSIC_DATA_LENGTH;i++){
						MU2_TxChar(rcdata.buf[i]);				//Send Data for Serial
						}
		wait(45);												//wait(45ms)
		battery_check();										//battery check
	}
    return 0;
}
