#define GPNCON *((volatile unsigned long*)0x7f008830)

void button_init()
{
	GPNCON = (0x2) | (0x2 << 10);  //ʹ�õ�1���͵�6����������ӦEINT0��EINT1
	
	/*����ʹ�õ�1���͵�2�������������жϣ�
	��Ϊ���Ƕ�Ӧ���жϺ�����ͬ�ģ�����EINT0*/
}