#define GPNCON *((volatile unsigned long*)0x7f008830)

void button_init()
{
	GPNCON = (0x2) | (0x2 << 10);  //ʹ��ǰ������������ӦEINT0��EINT5
	
	/*����ʹ��EINT0��EINT1�������жϣ���ΪEIN0��EINT1��Ӧ���ж�����ͬ�ģ����ǹ���һ���ж�������ַ�������޷��ֱ����*/
}