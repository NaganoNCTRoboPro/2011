#include "classic.h"
#include "rc_classic.h"
#include "../mu2/mu2.h"

void MakeClassicRCData(union classic_data *rcdata, const union wii_classic *data, unsigned char *analog_data)
{
	rcdata->detail.Button.LEFT		= data->detail.Button.Left	^ 1;
	rcdata->detail.Button.DOWN		= data->detail.Button.Down	^ 1;
	rcdata->detail.Button.RIGHT		= data->detail.Button.Right	^ 1;
	rcdata->detail.Button.UP		= data->detail.Button.Up	^ 1;
	rcdata->detail.Button.Y			= data->detail.Button.y		^ 1;
	rcdata->detail.Button.B			= data->detail.Button.b		^ 1;
	rcdata->detail.Button.A			= data->detail.Button.a		^ 1;
	rcdata->detail.Button.X			= data->detail.Button.x		^ 1;
	rcdata->detail.Button.R			= GetAnalogueRS( data )		>>4;
	rcdata->detail.Button.L			= GetAnalogueLS( data )		>>4;
	rcdata->detail.Button.ZR		= data->detail.Button.ZR	^ 1;
	rcdata->detail.Button.ZL		= data->detail.Button.ZL	^ 1;
	rcdata->detail.Button.START		= data->detail.Button.Start ^ 1;
	rcdata->detail.Button.HOME		= data->detail.Button.Home^ 1;
	rcdata->detail.Button.SELECT	= data->detail.Button.Select^ 1;
	rcdata->detail.AnalogL.X		= (   analog_data[0]) & 0x0f;
	rcdata->detail.AnalogL.Y		= (14-analog_data[1]) & 0x0f;
	rcdata->detail.AnalogR.X		= (   analog_data[2]) & 0x0f;	
	rcdata->detail.AnalogR.Y		= (14-analog_data[3]) & 0x0f;
}

void get_classic_analog( const union wii_classic *data, unsigned char *analog_data )
{
	int i;	
	analog_data[0] = (unsigned char)GetAnalogueLeftX( data );
	analog_data[1] = (unsigned char)GetAnalogueLeftY( data );
	analog_data[2] = (unsigned char)GetAnalogueRightX( data );
	analog_data[3] = (unsigned char)GetAnalogueRightY( data );
	for(i=0; i<2; i++){
			 if(analog_data[i]<=10)	analog_data[i] = 0x00;
		else if(analog_data[i]<=11)	analog_data[i] = 0x01;
		else if(analog_data[i]<=13) analog_data[i] = 0x02;
		else if(analog_data[i]<=17) analog_data[i] = 0x03;
		else if(analog_data[i]<=21) analog_data[i] = 0x04;
		else if(analog_data[i]<=25) analog_data[i] = 0x05;
		else if(analog_data[i]<=29) analog_data[i] = 0x06;
		else if(analog_data[i]<=34) analog_data[i] = 0x07;
		else if(analog_data[i]<=38) analog_data[i] = 0x08;
		else if(analog_data[i]<=42) analog_data[i] = 0x09;
		else if(analog_data[i]<=46) analog_data[i] = 0x0A;
		else if(analog_data[i]<=50) analog_data[i] = 0x0B;
		else if(analog_data[i]<=53) analog_data[i] = 0x0C;
		else if(analog_data[i]<=55) analog_data[i] = 0x0D;
		else if(analog_data[i]> 55) analog_data[i] = 0x0E;	
	}
	for(i=2; i<5; i++){
			 if(analog_data[i]<=5)  analog_data[i] = 0x00;
		else if(analog_data[i]<=6)  analog_data[i] = 0x01;
		else if(analog_data[i]<=7)  analog_data[i] = 0x02;
		else if(analog_data[i]<=8)  analog_data[i] = 0x03;
		else if(analog_data[i]<=10) analog_data[i] = 0x04;
		else if(analog_data[i]<=12) analog_data[i] = 0x05;
		else if(analog_data[i]<=14) analog_data[i] = 0x06;
		else if(analog_data[i]<=16) analog_data[i] = 0x07;
		else if(analog_data[i]<=18) analog_data[i] = 0x08;
		else if(analog_data[i]<=20) analog_data[i] = 0x09;
		else if(analog_data[i]<=22) analog_data[i] = 0x0A;
		else if(analog_data[i]<=23) analog_data[i] = 0x0B;
		else if(analog_data[i]<=24) analog_data[i] = 0x0C;
		else if(analog_data[i]<=25) analog_data[i] = 0x0D;
		else if(analog_data[i]> 25) analog_data[i] = 0x0E;
	}
}
