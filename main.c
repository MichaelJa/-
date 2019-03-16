#include <reg52.h>
#include <stdlib.h>

#define MAX 100 //��������ֵ
#define MIN 20   //�������Сֵ

bit flag500ms=0;         //500ms��ʱ��־
bit flag1s = 0;          //1s��ʱ��־
unsigned int number=0;  //�����ⲿ����
unsigned int number2=0; //���ݼ���ֵ


unsigned char RBUFF = 0; //��UART����������
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

//���������
unsigned char random(unsigned int seed);
//18B20����
void ConfigTimer1();//1s�ж�
unsigned char IntToString(unsigned char *str, int dat);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);
//LCD����
extern void InitLcd1602();
extern LcdWriteCmd(unsigned char dat);  //����
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
//PWM����
sbit PWMOUT = P2^1; //�������
sbit PWMOUT2=P2^0;
//sbit CES=P1^7;
unsigned char HighRH=0;
unsigned char HighRL=0;
unsigned char LowRH=0;
unsigned char LowRL=0;

extern void ConfigPWM(unsigned int fr, unsigned char dc);
extern void ClosePWM();
//UART����
extern void ConfigUART();
void delay(unsigned char n);

unsigned char PWM=MIN;     //PWM��ʼ��
//����
float numfloat =0;         //�ٶ�

void main()
{

    unsigned char numChar =0; //�ٶ���������
    unsigned char numxiao =0;

    bit res;
    int temp;        //��ȡ���ĵ�ǰ�¶�ֵ
    int intT, decT;  //�¶�ֵ��������С������
    int intT2=0;
    unsigned char len;
    unsigned char str[12];          
    unsigned char mode = 0;  //���ģʽѡ��λ
    PWMOUT=0;                //������ų�ʼ��
    PWMOUT2=0;


    
    InitLcd1602();     //��ʼ��Һ��
    ConfigUART();  //���ò�����Ϊ9600
    ConfigTimer1();  //���ö�ʱ��1       1s 500ms
    Start18B20();      //����DS18B20

    RBUFF=6;
//    CES=1;

    EX0=1;
    IT0=1;
    EA = 1;            //�����ж�
    while(1)
    {

//���ٲ���--------------------------------------------------------
    ET1=1;
    numfloat=number2/20;
    numChar=(unsigned char)numfloat;
    numxiao=(int)(numfloat*10)%10;

    len = IntToString(str, (int)numChar); //��������ת��Ϊ�ַ���
    str[len++] = '.';             //���С����
//    decT = (decT*10) / 16;        //�����Ƶ�С������ת��Ϊ1λʮ����λ
    str[len++] =numxiao + '0';      //ʮ����С��λ��ת��ΪASCII�ַ�
    while (len < 5)               //�ÿո��뵽6���ַ�����
    {
        str[len++] = ' ';
    }
    str[len] = '\0';              //����ַ���������
    LcdShowStr(0, 1, "V=");
    LcdShowStr(3, 1, str);        //��ʾ��Һ������
//---------------------------------------------------------


//        RBUFF=5;
        mode=RBUFF;
        switch (mode)
        {
            case 0://����
            LcdWriteCmd(0x01);
            LcdShowStr(0,0,"Mode: accelerate");//��ʾģʽ
            
            if(PWM>=MAX)
            {
                PWM=MAX;
                LcdWriteCmd(0x01);
                LcdShowStr(9,1,"Fastest!");
            }else PWM+=10;
            ConfigPWM(2000, PWM);
            RBUFF=5;
            break;
            case 1://����
            LcdWriteCmd(0x01);
            LcdShowStr(0,0,"Mode: decelerate");//��ʾģʽ
            if((PWM<=MIN)||(PWM>MAX))
            {
                PWM=MIN;
                LcdWriteCmd(0x01);
                LcdShowStr(9,1,"Stop");
            }else PWM-=10;
            ConfigPWM(2000, PWM);
            RBUFF=5;
            break;
            case 2://��Ȼ��
            LcdWriteCmd(0x01);
            LcdShowStr(0,0,"Mode:NaturalWind");
            case 10:            //��Ȼ��
            ET1=1;   //  ���ö�ʱ��1��ʱ1s
            if(flag500ms)
            {
                PWM=random(((unsigned int)TH0<<8)|TL0);
                ConfigPWM(100,PWM);
                flag500ms=0;
            }
            RBUFF=10;
            break;
            case 3://�¶��Զ������ٶ�
            ET1=1;
//            LcdShowStr(8,0,"Mode:T=");
            LcdShowStr(0,0,"Temperature:");
            LcdShowStr(9,1,"T=");


            if (flag1s)  //ÿ�����һ���¶�
            {
                flag1s = 0;
                res = Get18B20Temp(&temp);  //��ȡ��ǰ�¶�
                if (res)                    //��ȡ�ɹ�ʱ��ˢ�µ�ǰ�¶���ʾ
                {
                    intT = temp >> 4;             //������¶�ֵ��������                   
                    decT = temp & 0xF;            //������¶�ֵС������
                    len = IntToString(str, intT); //��������ת��Ϊ�ַ���
                    str[len++] = '.';             //���С����
                    decT = (decT*10) / 16;        //�����Ƶ�С������ת��Ϊ1λʮ����λ
                    str[len++] = decT + '0';      //ʮ����С��λ��ת��ΪASCII�ַ�
                    while (len < 6)               //�ÿո��뵽6���ַ�����
                    {
                        str[len++] = ' ';
                    }
                    str[len] = '\0';              //����ַ���������
                    LcdShowStr(11, 1, str);        //��ʾ��Һ������
                    Start18B20();               //����������һ��ת��
                }
                else                        //��ȡʧ��ʱ����ʾ������Ϣ
                {
                    LcdWriteCmd(0x01);
                    LcdShowStr(0, 1, "error!");
                }
                if(intT2!=0)   //�ж��Ƿ��ǵ�һ�β���
                {
                    if(intT2>intT)
                    PWM-=20*(intT2-intT);
                    if(intT2<intT)
                    PWM+=20*(intT-intT2);
                }
                intT2=intT;                 //�����������������Ƚ�
                ConfigPWM(100,PWM);
                RBUFF=3;
            }
            break;
            case 4://�رյ綯��
            LcdWriteCmd(0x01);
            LcdShowStr(0,0,"close!");
            ClosePWM();
            PWMOUT=0;
            PWM=MIN;
            RBUFF=5;
            break;
            case 5://hold
            break;
            case 6://������
            LcdShowStr(0,0,"ceshi!");
            ClosePWM();
            PWMOUT=1;
            PWM=MAX;
            RBUFF=5;
            break; 
            default:
            LcdShowStr(0,0,"Please Input:");                                       
        }

    }

}
void ConfigTimer1() //����������ʱ��1
{
    TMOD&=0X0F;
    TMOD|=0X10;
    TH1=0x48;
    TL1=0xFF;
    ET1=0;
    TR1=1;
}

/* ������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //���Ϊ����������ȡ����ֵ������ָ������Ӹ���
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //��ת��Ϊ��λ��ǰ��ʮ��������
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i����ֵ������Ч�ַ��ĸ���
    while (i-- > 0)   //������ֵת��ΪASCII�뷴�򿽱�������ָ����
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //����ַ���������
    
    return len;   //�����ַ�������
}
//����MIN��MAX�������
unsigned char random(unsigned int seed)
{
    unsigned char value;
    srand(seed);
    value=rand()%(MAX+1-MIN)+MIN;
    return value;
}

//��ʱ��1    1s  500ms
void Interrupttime1() interrupt 3
{
    static unsigned char cnt=0;
    TH1=0x48;
    TL1=0xFF;
    cnt++;
    if(cnt==10)//500ms  
    {
        flag500ms=1;
    }
    if(cnt>=20) //500ms  1s
    {
        cnt=0;
        flag500ms=1;
        flag1s=1;
        number2=number;
        number=0;
    }


	
}
//PWM��ʱ��0
void InterruptTimer0() interrupt 1
{
    if (PWMOUT == 1)  //��ǰ���Ϊ�ߵ�ƽʱ��װ�ص͵�ƽֵ������͵�ƽ
    {
        TH0 = LowRH;
        TL0 = LowRL;
        PWMOUT = 0;
    }
    else              //��ǰ���Ϊ�͵�ƽʱ��װ�ظߵ�ƽֵ������ߵ�ƽ
    {
        TH0 = HighRH;
        TL0 = HighRL;
        PWMOUT = 1;
    }
}
//��ʱ��2���������ʷ�����
void InterruptUART() interrupt 4
{
    EA=0;
//    if(EXF2==1)
//    {
//        number++;
//        EXF2=0;    
//    }
    if(RI)
	{
		RI = 0;
		RBUFF = SBUF;
//        SBUF=SBUF;
	}
//    if(TI)
//    {
//        TI=0;  
//    }
    EA=1;
}

void interruptwai0() interrupt 0
{
    number++;
}

void delay(unsigned char n)
{
    unsigned char i=0,j=0;
    for(j=0;j<n;j++)
    for(i=0;i<120;i++);
}


