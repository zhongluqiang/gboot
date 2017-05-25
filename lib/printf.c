#include "vsprintf.h"

unsigned char outbuf[1024];
unsigned char inbuf[1024];

int printf(const char *fmt, ...)
{
	int i;
	
	//1.将变参转化为字符串
	va_list args;
	va_start(args, fmt);
	vsprintf((char*)outbuf, fmt, args);
	va_end(args);
	
	
	//2.打印字符串到串口
	for(i = 0; i < strlen(outbuf); i++)
	{
		putc(outbuf[i]);
	}
	
	return i;
}

int scanf(const char *fmt, ...)
{
	va_list args;
	unsigned char c;
	int i = 0;
	
	//1.获取输入的字符串
	while(1)
	{
		c = getc();
		if(c == 0x0a || c == 0x0d)
		{
			inbuf[i] = '\n';
			break;
		}
		else
		{
			inbuf[i] = c;
			++i;
		}
	}
	
	//2.格式转化
	va_start(args, fmt);
	vsscanf((char *)inbuf, fmt, args);
	va_end(args);
	
	return i;
}