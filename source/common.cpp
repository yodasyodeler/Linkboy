#include "common.h"

/**************************************************
*	Convert String
*		Converts string 'buf' to int 'num. 'base'
*		determines the base of 'buf'. Base 2 - 16
*		supported.
*
*		return: 
*				false	->	'buf' is not a number
*							\base is not supported
*				true	->	'buf' is a number
*		
**************************************************/
bool convertString(int& num, const char * buf, const int base)
{
	bool re = false;
	int  i	= 0;
	int temp = num;
	num = 0;
	
	if (base >= 2 && base <= 16) {
		re = true;
		while (buf[i] != '\0' && re) {
			num *= base;
			if (buf[i] >= '0' && buf[i] <= '9') 
				num += buf[i] - '0';
			else if (buf[i] >= 'A' && buf[i] <= 'F')
				num += buf[i] - 'A' + 0xA;
			else if (buf[i] >= 'a' && buf[i] <= 'f') 
				num += buf[i] - 'a' + 0xA;
			else {
				re = false;
				num = temp;
			}
			i += 1;
		}
	}

	return re;
}

int lb_strlen(const char* str)
{
	int i = 0;
	while (str[i] != '\0')
		++i;
	return i;
}

void lb_strcpy(char* str1, const char* str2)
{
	while ((*(str1++) = *(str2++)));
}

int lb_strncmp(const char* str1, const char* str2, int n)
{
	int re = 0;
	

	while (!re && n--)
		re = (*(str1++) - *(str2++));

	return re;
}

void lb_strncat(char* str1, const char* str2, int n)
{
	int len = lb_strlen(str1);
	int i = 0;

	while ((str1[len+i++] = *(str2++)) && n > i);
}