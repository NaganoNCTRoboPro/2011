#include "nunchuk.h"
#include "rc_nunchuk.h"

void MakeNunchukRCData(union nunchuk_data *rcdata,const union wii_nunchuk *data, unsigned char *analog_data)
{
	// Substitutes each controller data for rcdata
	// Button
	rcdata->detail.Button.C				= data->detail.Button.C ^ 1;	//Button C
	rcdata->detail.Button.Z				= data->detail.Button.Z ^ 1;	//Button Z
	//Acceleration
	rcdata->detail.Acceleration.X		= analog_data[0] & 0x07;		//acceleration axis:X
	rcdata->detail.Acceleration.Y		= analog_data[1] & 0x07;		//acceleration axis:Y
	rcdata->detail.Acceleration.Z		= analog_data[2] & 0x03;		//acceleration axis:Z
	// Analogue
	rcdata->detail.Analog.X				= analog_data[3] & 0x0f;		//analog stick axix:X
	rcdata->detail.Analog.Y				=(14-analog_data[4]) & 0x0f;	//analog stick axix:Y
}

void get_nunchuk_analog( const union wii_nunchuk *data , unsigned char *analog_data)
{
	int i;
//Substitutes AnalogStickData or Acceleration For analog_data 	
	analog_data[0] = (unsigned char)GetAnalogueX( data );				//analog stick axix:X
	analog_data[1] = (unsigned char)GetAnalogueY( data );				//analog stick axis:y
	analog_data[2] = (unsigned char)GetXacceleration( data )>>4;		//acceleration axis:X
	analog_data[3] = (unsigned char)GetYacceleration( data )>>4;		//acceleration axis:Y
	analog_data[4] = (unsigned char)GetZacceleration( data )>>4;		//acceleration axis:Z
//analog stick
	for(i=0; i<=1; i++){
		     if(analog_data[i]<= 65) analog_data[i] = 0x00;
		else if(analog_data[i]<= 75) analog_data[i] = 0x01;
		else if(analog_data[i]<= 85) analog_data[i] = 0x02;
		else if(analog_data[i]<= 95) analog_data[i] = 0x03;
		else if(analog_data[i]<=105) analog_data[i] = 0x04;
		else if(analog_data[i]<=115) analog_data[i] = 0x05;
		else if(analog_data[i]<=125) analog_data[i] = 0x06;
		else if(analog_data[i]<=135) analog_data[i] = 0x07;
		else if(analog_data[i]<=190) analog_data[i] = 0x08;
		else if(analog_data[i]<=195) analog_data[i] = 0x09;
		else if(analog_data[i]<=200) analog_data[i] = 0x0A;
		else if(analog_data[i]<=205) analog_data[i] = 0x0B;
		else if(analog_data[i]<=210) analog_data[i] = 0x0C;
		else if(analog_data[i]<=215) analog_data[i] = 0x0D;
		else if(analog_data[i]>=218) analog_data[i] = 0x0E;
	}
//Acceleration
	//axis XandY
	for(i=2;i<=3;i++){
			 if(analog_data[i]<   4) analog_data[i] = 0x00;
		else if(analog_data[i]<=  5) analog_data[i] = 0x01;
		else if(analog_data[i]<=  6) analog_data[i] = 0x02;
		else if(analog_data[i]<=  7) analog_data[i] = 0x03;
		else if(analog_data[i]<=  8) analog_data[i] = 0x04;
		else if(analog_data[i]<=  9) analog_data[i] = 0x05;
		else if(analog_data[i]<= 10) analog_data[i] = 0x06;
		else if(analog_data[i]>  10) analog_data[i] = 0x07;
	}
	//axis Z
	for(i=4;i==4;i++){
			 if(analog_data[i]<   5) analog_data[i] = 0x00;
		else if(analog_data[i]<=  7) analog_data[i] = 0x01;
		else if(analog_data[i]<   9) analog_data[i] = 0x02;
		else if(analog_data[i]>=  9) analog_data[i] = 0x03;
	}
}
