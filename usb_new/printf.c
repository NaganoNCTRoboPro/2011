#include <stdarg.h>

#ifndef	NULL
	#define NULL ((void *)0)
#endif

static int (*__printf_output)(char) = NULL;

#define OUTPUT(c)	(*__printf_output)(c);

#define NUM_BUF_SIZE	32

#define SIGN_PLUS		0
#define SIGN_MINUS		1

#define FLAG_ALEFT		1
#define FLAG_SIGN		2
#define FLAG_SPACE		4
#define FLAG_ZERO		8
#define FLAG_ALT		16

#define LENGTH_DEFAULT	0
#define LENGTH_LONG		1
#define LENGTH_SHORT	2
#define LENGTH_CHAR		3

#define TYPE_UNKNOWN	0
#define TYPE_DEC_S		1
#define TYPE_DEC_U		2
#define TYPE_HEX_LC		3
#define TYPE_HEX_UC		4
#define TYPE_OCT		5
#define TYPE_BIN		6
#define TYPE_CHAR		7
#define TYPE_STRING		8
#define TYPE_POINTER	9
#define TYPE_PERCENT	10
#define TYPE_OUTNUM		11


void set_printf_output(int (*fp)(char))
{
	__printf_output = (int (*)(char))fp;
}

int __printf(const char *format, ...)
{
	va_list ap;
	int i, cnt, tmp, *int_p;
	char num_buf[NUM_BUF_SIZE];		// 数字->文字変換用バッファ
	char *char_p, char_tmp;
	unsigned long ul_tmp;
	unsigned int output_cnt = 0;

	struct{
		unsigned char sign;
		union{
			signed long s;
			unsigned long u;
		}value;
	}number;
	unsigned short flag;	// フラグ
	int width;				// 最小フィールド幅
	unsigned char length;	// 長さ修飾子
	unsigned char type;		// 変換指定子
	unsigned char radix;	// 基数
	
	if( __printf_output == NULL )	return -1;

	va_start(ap, format);
	
	while( *format ){
		if( *format != '%' ){
			OUTPUT(*format);	output_cnt++;
			
			format++;
		}else{
			format++;	// skip '%'
			///////////// flag analysis //////////////
			flag = 0;	// clear all flags
			while(1){
				switch(*format){
					case '-': flag |= FLAG_ALEFT;		break;
					case '+': flag |= FLAG_SIGN;		break;
					case ' ': flag |= FLAG_SPACE;		break;
					case '0': flag |= FLAG_ZERO;		break;
					case '#': flag |= FLAG_ALT;			break;
					default	: goto flag_analysis_end;
				}
				format++;
			}
			flag_analysis_end:
			///////////// flag analysis //////////////
			///////////// width analysis //////////////
			cnt = 0;
			while( *format >= '0' && *format <= '9' ){
				cnt++;
				format++;
			}
			width = 0;
			tmp = 1;
			for( i=cnt; i!=0; i-- ){
				format--;
				width += tmp*(*format - '0');
				tmp*=10;
			}
			format += cnt;
			///////////// width analysis //////////////
			///////////// length analysis //////////////
			switch(*format){
				case 'h':
					if( *(format+1) == 'h' ){
						// charcter length
						length = LENGTH_CHAR;
						format++;
					}else{
						// short length
						length = LENGTH_SHORT;
					}
					format++;
					break;
				case 'l':
					// long length
					length = LENGTH_LONG;
					format++;
					break;
				default:
					// default length
					length = LENGTH_DEFAULT;
			}
			///////////// length analysis //////////////
			///////////// type analysis //////////////
			type = TYPE_UNKNOWN;
			switch(*format){
				case 'd':	// int
				case 'i':
					type = TYPE_DEC_S;
					radix = 10;
					goto output_number_data;
				case 'u':	// unsigned int
					type = TYPE_DEC_U;
					radix = 10;
					goto output_number_data;
				case 'x':	// hexadecimal(lower-case)
					type = TYPE_HEX_LC;
					radix = 16;
					goto output_number_data;
				case 'X':	// hexadecimal(upper-case)
					type = TYPE_HEX_UC;
					radix = 16;
					goto output_number_data;
				case 'o':	// octal
					type = TYPE_OCT;
					radix = 8;
					goto output_number_data;
				case 'b':	// binary
					type = TYPE_BIN;
					radix = 2;
					goto output_number_data;
				case 's':	// character string
					type = TYPE_STRING;
					char_p = va_arg(ap, char *);

					char_tmp = (flag & FLAG_ZERO) ? '0' : ' ';

					for( cnt=0; *(char_p + cnt) ; cnt++ );		// 文字列長カウント

					tmp = (width>cnt) ? (width-cnt) : (0);
					if( flag & FLAG_ALEFT ){
						while(*char_p){ OUTPUT(*(char_p++));	output_cnt++; }
						while( tmp-- ){ OUTPUT(' ');			output_cnt++; }
					}else{
						while( tmp-- ){ OUTPUT(char_tmp);	output_cnt++; }
						while(*char_p){ OUTPUT(*(char_p++)); output_cnt++; }
					}
					break;
				case 'p':	// pointer (void *)
					type = TYPE_POINTER;
					radix = 16;
					flag |= FLAG_ALT;		// enable alternate form '0x'
					goto output_number_data;
				case 'c':	// charcter
					type = TYPE_CHAR;
				case '%':	// literal '%'
					type = (type == TYPE_UNKNOWN) ? TYPE_PERCENT : type;
					tmp = (width>0) ? (width-1) : (0);
					if( flag & FLAG_ALEFT ){
						OUTPUT((type == TYPE_CHAR) ? ((char)( va_arg(ap, int) )) : '%');
						output_cnt++;
						while( tmp-- ){ OUTPUT(' ');	output_cnt++; }
					}else{
						char_tmp = (flag & FLAG_ZERO) ? '0' : ' ';
						while( tmp-- ){ OUTPUT(char_tmp);	output_cnt++; }
						OUTPUT((type == TYPE_CHAR) ? ((char)( va_arg(ap, int) )) : '%');
						output_cnt++;
					}
					break;
				case 'n':
					type = TYPE_OUTNUM;
					int_p = va_arg(ap, int *);
					*int_p = output_cnt;
					break;
				default:
					format--;
			}
			///////////// type analysis //////////////
			finish_output_number_data:
			format++;
		}
	}
	va_end(ap);

	return output_cnt;

output_number_data:
	switch( type ){
		case TYPE_DEC_S:
			// signed number
			switch(length){
				case LENGTH_DEFAULT:	number.value.s = (signed long)( va_arg(ap, signed int) );		break;
				case LENGTH_LONG:		number.value.s = (signed long)( va_arg(ap, signed long) );		break;
//				case LENGTH_SHORT:		number.value.s = (signed long)( va_arg(ap, signed short) );		break;
				case LENGTH_SHORT:		number.value.s = (signed long)( va_arg(ap, signed int) );		break;
//				case LENGTH_CHAR:		number.value.s = (signed long)( va_arg(ap, signed char) );		break;
				case LENGTH_CHAR:		number.value.s = (signed long)( va_arg(ap, signed int) );		break;
			}
			if( number.value.s < 0 ){
				number.sign = SIGN_MINUS;
				number.value.u = (unsigned long)( (-1)*number.value.s );
			}else{
				number.sign = SIGN_PLUS;
			}
			break;
		case TYPE_POINTER:
			number.value.u = (unsigned long)( va_arg(ap, void *) );
			number.sign = SIGN_PLUS;
			flag &= (FLAG_ALEFT | FLAG_ZERO | FLAG_ALT);	// ignore ' '(SPACE) and '+'(SIGN) flags
			break;
		default:
			// unsigned number
			switch(length){
				case LENGTH_DEFAULT:	number.value.u = (unsigned long)( va_arg(ap, unsigned int) );		break;
				case LENGTH_LONG:		number.value.u = (unsigned long)( va_arg(ap, unsigned long) );		break;
//				case LENGTH_SHORT:		number.value.u = (unsigned long)( va_arg(ap, unsigned short) );		break;
				case LENGTH_SHORT:		number.value.u = (unsigned long)( va_arg(ap, unsigned int) );		break;
//				case LENGTH_CHAR:		number.value.u = (unsigned long)( va_arg(ap, unsigned char) );		break;
				case LENGTH_CHAR:		number.value.u = (unsigned long)( va_arg(ap, unsigned int) );		break;
			}
			number.sign = SIGN_PLUS;

			flag &= (FLAG_ALEFT | FLAG_ZERO | FLAG_ALT);	// ignore ' '(SPACE) and '+'(SIGN) flags
	}
	flag = (flag & FLAG_SIGN) ? (flag & ~FLAG_SPACE) : flag;	// ignore '+'(SIGN) when ' '(SPACE) flag is active
	flag = (flag & FLAG_ALEFT) ? (flag & ~FLAG_ZERO) : flag;	// ignore '0'(ZERO) when '-'(ALEFT) flag is active

	//////////////// number -> string //////////////////
	i = NUM_BUF_SIZE;
	char_tmp = (type == TYPE_HEX_UC) ? (55) : (87);
	ul_tmp = number.value.u;
	do{
		i--;
		num_buf[i] = ul_tmp % radix;
		num_buf[i] += (num_buf[i]>9) ? char_tmp : '0';

		ul_tmp /= radix;
	}while( ul_tmp!=0 && i!=0 );
	//////////////// number -> string //////////////////
	
	///////////////// output number ///////////////////

	char_tmp = (number.sign == SIGN_PLUS) ? ((flag & FLAG_SIGN) ? '+' : ((flag & FLAG_SPACE) ? ' ' : '*' )) : '-';	// Dummy '*'

	tmp = (NUM_BUF_SIZE-i) + ((char_tmp !='*') ? (1) : (0));
	
	char_p = "";
	if( (flag & FLAG_ALT) && (number.value.u != 0) ){
		switch(type){
			case TYPE_POINTER:
			case TYPE_HEX_LC:
				char_p = "0x";
				tmp+=2;
				break;
			case TYPE_HEX_UC:
				char_p = "0X";
				tmp+=2;
				break;
			case TYPE_OCT:
				char_p = "0";
				tmp+=1;
				break;
			case TYPE_BIN:
				char_p = "0b";
				tmp+=2;
		}
	}

	tmp = (tmp < width) ? (width - tmp) : 0;
	if( flag & FLAG_ALEFT ){
		if(char_tmp != '*'){ OUTPUT(char_tmp);	output_cnt++; }
		while(*char_p){ OUTPUT(*(char_p++));	output_cnt++; }	// output alternate form
		for(; i<NUM_BUF_SIZE; i++){ OUTPUT(num_buf[i]);	output_cnt++; }
		while(tmp--){ OUTPUT(' ');	output_cnt++; }
	}else{
		if( flag & FLAG_ZERO ){
			if(char_tmp != '*'){ OUTPUT(char_tmp);	output_cnt++; }
			while(*char_p){ OUTPUT(*(char_p++));	output_cnt++; }	// output alternate form
			while(tmp--){ OUTPUT('0');				output_cnt++; }
		}else{
			while(tmp--){ OUTPUT(' ');				output_cnt++; }
			while(*char_p){ OUTPUT(*(char_p++));		output_cnt++; }	// output alternate form
			if(char_tmp != '*'){ OUTPUT(char_tmp);	output_cnt++; }
		}
		for(; i<NUM_BUF_SIZE; i++){ OUTPUT(num_buf[i]);	output_cnt++; }
	}
	///////////////// output number ///////////////////
	
	goto finish_output_number_data;
}
