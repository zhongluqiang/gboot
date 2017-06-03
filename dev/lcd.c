
#define GPICON (*(volatile unsigned long*)0x7f008100)
#define GPJCON (*(volatile unsigned long*)0x7f008120)

#define MOFPCON (*(volatile unsigned long*)0x7410800c)
#define SPCON (*(volatile unsigned long*)0x7f0081a0)
#define VIDCON0 (*(volatile unsigned long*)0x77100000)
#define VIDCON1 (*(volatile unsigned long*)0x77100004)
#define VIDTCON0 (*(volatile unsigned long*)0x77100010)
#define VIDTCON1 (*(volatile unsigned long*)0x77100014)
#define VIDTCON2 (*(volatile unsigned long*)0x77100018)
#define WINCON0 (*(volatile unsigned long*)0x77100020)
#define VIDOSD0A  (*(volatile unsigned long*)0x77100040)
#define VIDOSD0B  (*(volatile unsigned long*)0x77100044)
#define VIDOSD0C (*(volatile unsigned long*)0x77100048)
#define VIDW00ADD0B0 (*(volatile unsigned long*)0x771000a0)
#define VIDW00ADD1B0 (*(volatile unsigned long*)0x771000d0)

#define CLKVAL 14
#define VSPW 9
#define VBPD 1
#define HSPW 40
#define HBPD 1
#define VFPD 1
#define HFPD 1
#define LINEVAL 271
#define HOZVAL 479
#define LEFTTOPX 0
#define LEFTTOPY 0
#define RIGHTBOTX 479
#define RIGHTBOTY 271

#define FRAME_BUFFER 0x59000000

extern const unsigned char image_qq[135300];

/*引脚初始化*/
void lcd_port_init(void)
{
	GPICON = 0xaaaaaaaa;
	GPJCON = 0xaaaaaaaa;
}

/*lcd控制寄存器初始化*/
void lcd_control_init(void)
{
	MOFPCON &= (~(0x1 << 3)); /*SEL_BYPASS[3] must be set as '0'*/
	SPCON = 0x01; /*select LCD I/F pin configuration as RGB I/F style*/
	
	VIDCON0 = (CLKVAL << 6) | (0x1 << 4) | (0x3 << 0); /*设置时钟源及VCLK的值，VCLK = HCLK / (CLKVAL + 1)*/
	VIDCON1 = (0x1 << 5) | (0x1 << 6); /*对比屏幕的芯片手册和6410的LCD时序手册，发现HSYNC和VSYNC信号极性需要反转，VDEN不需要*/
	
	VIDTCON0 = (VBPD << 16) | (VFPD << 8) | (VSPW << 0); /*时钟初始化*/
	VIDTCON1 = (HBPD << 16) | (HFPD << 8) | (HSPW << 0);
	VIDTCON2 = (LINEVAL << 11) | (HOZVAL << 0); /*屏幕分辨率初始化*/
	
	WINCON0 = (0x1 << 16) | (0xb << 2) | (0x1 << 0); /*24bpp模式，打开半字交换*/
	
	VIDOSD0A = (LEFTTOPX << 11) | (LEFTTOPY << 0);  /*设置屏幕坐标，坐标（0，0）设置为左上角*/
	VIDOSD0B = (RIGHTBOTX << 11) | (RIGHTBOTY << 0);
	VIDOSD0C = (LINEVAL + 1) * (HOZVAL + 1);
	
	VIDW00ADD0B0 = FRAME_BUFFER;
	VIDW00ADD1B0 = (FRAME_BUFFER + (LINEVAL + 1) * (HOZVAL + 1) * 4); /*注意24bpp模式每个像素占用4字节，而不是3字节，参考芯片手册14.4.4小节关于图像格式的说明*/
}

void lcd_init(void)
{
	lcd_port_init();
	lcd_control_init();
}

void point(int row, int col, int color) 
{
    int red, green, blue;
    unsigned long *point = (unsigned long*)FRAME_BUFFER;
    red = (color>>16) & 0xff;
    green = (color>>8) & 0xff;
    blue = (color>>0) & 0xff;
    *(point + row*480 + col) = (green<<24) | (blue<<16) | red; /*从网上获取的RGB排布方式，非常奇怪*/
}

void paint_bmp(int height, int width, const unsigned char bmp[])
{
    int i,j;
    unsigned char *p = (unsigned char *)bmp;
    int blue, green,red;
    int color;
    
    for(i=0;i<height;i++) 
    {
        for(j=0;j<width;j++)
        {                            
            red = *p++;    /*注意导出图片的时候每像素的颜色排列为红/蓝/绿*/
            blue = *p++;            
            green = *p++;

            color = (red<<16)|(green<<8)|(blue<<0); 
                        
            point(i,j,color);
        }
    }
}


void lcd_test()
{
	paint_bmp(205, 220, image_qq);
}











