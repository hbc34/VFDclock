#include "CSU32P10.h"
//#include "define_data.h"
#include "define_function.h"
#include "my_define.h"

#define SDA PT3_1                           //����SDA����Ӧ��GPIO�ӿڱ��P1.4  
#define SCL PT3_0                           //����SCL����Ӧ��GPIO�ӿڱ��P1.5 
#define SDAEN PT3EN_1                           //����SDA����Ӧ��GPIO�ӿڱ��P1.4  
#define SCLEN PT3EN_0                           //����SCL����Ӧ��GPIO�ӿڱ��P1.5
#define I2C_ADDR 0x3c
#define somenop asm("nop");asm("nop");asm("nop");asm("nop");
//#define I2C_ADDR 0x3c
unsigned char buf0;
unsigned char buf1;
unsigned char i2c_step;


//��delay

/* I2C��ʼ���� */  
void i2c_start()  
{  
	//��ʼ��GPIO�� 
	SDAEN = 1;				//����SDA����Ϊ��� 
	SCLEN = 1;				//����SCL����Ϊ��� 
	SDA = 1;                //����SDAΪ�ߵ�ƽ
	asm("nop");
	SCL = 1;                //����SCLΪ�ߵ�ƽ   
	somenop
	somenop
	SDA = 0;                //SCLΪ�ߵ�ƽʱ��SDA�ɸ߱��  
	somenop
	somenop
	somenop
	somenop
	somenop
	somenop
	SCL = 0;
}  
/* I2C��ֹ���� */  
void i2c_stop()  
{  
	somenop
	SDA = 0;	
	SDAEN = 1;   
	somenop
	SCL = 1; 
	somenop
	somenop
	somenop
	somenop
	somenop
	somenop
	SDA = 1;            //SCL�ߵ�ƽʱ��SDA�ɵͱ��
	somenop	
	SDAEN = 0;
//	SCL = 0;
}  
/*   
I2C��ȡACK�ź�(д����ʱʹ��)  
����ֵ ��0��ʾACK�ź���Ч����0��ʾACK�ź���Ч  
*/  
void i2c_read_ack()  
{   
	SCL = 0;               // SCL���
	SDAEN = 0;				 //����SDA����Ϊ����
	somenop	
	SCL = 1;              // SCL���  
	somenop
	if(SDA)
	{
		i2c_step = 0xff;
		return;
	}
	SCL = 0;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
}  
/* I2C����ACK�ź�(������ʱʹ��) */  
void i2c_send_ack()  
{
	SCL = 0;              // SCL���
	somenop
	SDA = 1;              //����ACK�ź�
	SDAEN = 1;              //����SDA����Ϊ���      
	asm("nop"); 
	asm("nop");
	asm("nop");	
	SCL = 1;             // SCL���  
	somenop
	SCL = 0;
//	SDAEN = 1;
	asm("nop");
	asm("nop");
}  
/* I2C�ֽ�д */ 

void i2c_resume()
{
	GIE = 0;
	delay_1s();
	delay_1s();
	i2c_read_ack();
	i2c_read_ack();
	i2c_read_ack();
	i2c_read_ack();
	i2c_read_ack();
	i2c_read_ack();
	i2c_read_ack();
	i2c_read_ack();
	GIE = 1;
}

void i2c_write_byte(unsigned char b)  
{   
	SDAEN = 1;    //����SDA����Ϊ���  
	SCL = 0;
	for (i2c_step = 8; i2c_step > 0; i2c_step --) 
	{  
		SCL = 0;             // SCL��� 
		asm("nop");
		if( b & (1 << (i2c_step - 1)))
			SDA = 1;
		else
			SDA = 0;
		asm("nop");
		SCL = 1;             // SCL���  
		somenop
		SCL = 0;
	}  
	i2c_read_ack();
}

/* I2C�ֽڶ� */  
void i2c_read_byte()  
{   
	buf1 = 0;
	SDAEN = 0;				//����SDA����Ϊ����
	for(i2c_step = 8; i2c_step > 0; i2c_step --) 
	{  
		SCL = 0;         // SCL���  
		asm("nop");
		asm("nop");
		SCL = 1;
		//i2c_result = (i2c_result << 1) | SDA;
		buf1 = (buf1 << 1) | SDA;
							// SCL���  
//		asm("nop");
//		asm("nop");
		SCL = 0;
	}
	i2c_send_ack();						//��Ŀ���豸����ACK�ź� 
}  
/*  
I2C������  
addr��Ŀ���豸��ַ  
buf����������  
len�������ֽڵĳ���  
*/  
void i2c_read()  
{  
	i2c_start();							//��ʼ��������ʼ����ͨ��  
									//���͵�ַ�����ݶ�д����  
	i2c_write_byte(I2C_ADDR<<1);	//I2C ADDR

	asm("nop");
	if(i2c_step == 0xff)
	{
		i2c_resume();
		return;
	}	

	i2c_write_byte(buf0);			//Reg ADDR

	asm("nop");
	if(i2c_step == 0xff)
	{
		i2c_resume();
		return;
	}	
	//asm("nop");
	i2c_start();//restart

	i2c_write_byte((I2C_ADDR<<1) +1);	//Reg ADDR

	asm("nop");
	if(i2c_step == 0xff)
	{
		i2c_resume();
		return;
	}
	//��������  

	i2c_read_byte();

	i2c_stop();
	//��ֹ��������������ͨ��  
}  
/*  
I2Cд����  
addr��Ŀ���豸��ַ  
buf��д������  
len��д���ֽڵĳ���  
*/  
void i2c_write()  
{  
	i2c_start();							//��ʼ��������ʼ����ͨ��  
	//���͵�ַ�����ݶ�д���� 

	i2c_write_byte(I2C_ADDR<<1); 

	asm("nop");
	if(i2c_step == 0xff)
	{
		i2c_resume();
		return;
	}	
	
	//д������ 

	i2c_write_byte(buf0); 

	asm("nop");
	if(i2c_step == 0xff)
	{
		i2c_resume();
		return;
	}

	i2c_write_byte(buf1);

	asm("nop");
	if(i2c_step == 0xff)
	{
		i2c_resume();
		return;
	}
	i2c_stop();                     //��ֹ��������������ͨ��  
}