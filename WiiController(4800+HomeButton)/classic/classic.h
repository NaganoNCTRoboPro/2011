#ifndef __CLASSIC_H__
#define __CLASSIC_H__


union wii_classic {
	struct {
		struct {
			unsigned long LeftX:6;
			unsigned long RightX_3:2;

			unsigned long LeftY:6;
			unsigned long RightX_2:2;


			unsigned long RightY:5;
			unsigned long LS_2:2;
			unsigned long RightX_1:1;

			unsigned long RS:5;
			unsigned long LS_1:3;

		} __attribute__ ((packed)) Analogue;
		struct {
			unsigned short :1;
			unsigned short RS:1;
			unsigned short Start:1;
			unsigned short Home:1;
			unsigned short Select:1;
			unsigned short LS:1;
			unsigned short Down:1;
			unsigned short Right:1;

			unsigned short Up:1;
			unsigned short Left:1;
			unsigned short ZR:1;
			unsigned short x:1;
			unsigned short a:1;
			unsigned short y:1;
			unsigned short b:1;
			unsigned short ZL:1;
		} __attribute__ ((packed)) Button;
	} __attribute__ ((packed)) detail;
	unsigned char buf[6];
};
inline unsigned int GetAnalogueLeftX( const union wii_classic *data )
{
	return data->detail.Analogue.LeftX;
}

inline unsigned int GetAnalogueLeftY( const union wii_classic *data )
{
	return data->detail.Analogue.LeftY;
}

inline unsigned int GetAnalogueRightX( const union wii_classic *data )
{
	return (data->detail.Analogue.RightX_3 << 3) | (data->detail.Analogue.RightX_2 << 1) | (data->detail.Analogue.RightX_1);
	
}

inline unsigned int GetAnalogueRightY( const union wii_classic *data )
{
	return data->detail.Analogue.RightY;
}

inline unsigned int GetAnalogueLS( const union wii_classic *data )
{
	return (data->detail.Analogue.LS_2 << 3) | (data->detail.Analogue.LS_1);
}

inline unsigned int GetAnalogueRS( const union wii_classic *data )
{
	return data->detail.Analogue.RS;
}

#endif
