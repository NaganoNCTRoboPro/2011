#ifndef __NUNCHUK_H__
#define __NUNCHUK_H__


union wii_nunchuk {
	struct {
		struct {
			unsigned short X:8;
			unsigned short Y:8;
		} __attribute__ ((packed)) Analogue;

		struct {
			unsigned char X:8;
			unsigned char Y:8;
			unsigned char Z:8;
		} __attribute__ ((packed)) Acceleration;

		struct {
			unsigned char Z:1;
			unsigned char C:1;
			unsigned char  :6;

		} __attribute__ ((packed)) Button;
	} __attribute__ ((packed)) detail;
	unsigned char buf[6];
};
inline unsigned int GetAnalogueX( const union wii_nunchuk *data )
{
	return data->detail.Analogue.X;
}

inline unsigned int GetAnalogueY( const union wii_nunchuk *data )
{
	return data->detail.Analogue.Y;
}

inline unsigned int GetXacceleration( const union wii_nunchuk *data )
{
	return data->detail.Acceleration.X;
	
}

inline unsigned int GetYacceleration( const union wii_nunchuk *data )
{
	return data->detail.Acceleration.Y;
}

inline unsigned int GetZacceleration( const union wii_nunchuk *data )
{
	return data->detail.Acceleration.Z;
}

#endif
