#include <reg52.h>

void ConfigUART();

void ConfigUART()
{
//    PCON &=0x7F;
    SCON=0x50;
    T2CON  = 0x34;  //���ô���Ϊģʽ1
    RCAP2H=0xFF;
    RCAP2L =0xDC;     //��ֵ��������ֵ
    TH2=0xFF;
    TL2=0xDC;
//    TR2=1;

    ES  = 1;       //ʹ�ܴ����ж�
}


