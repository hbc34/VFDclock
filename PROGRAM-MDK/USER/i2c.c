#include "CSU32P10.h"
//#include "define_data.h"
#include "define_function.h"
#include "my_define.h"

#define SDA PT3_1                           //定义SDA所对应的GPIO接口编号P1.4  
#define SCL PT3_0                           //定义SCL所对应的GPIO接口编号P1.5 
#define SDAEN PT3EN_1                           //定义SDA所对应的GPIO接口编号P1.4  
#define SCLEN PT3EN_0                           //定义SCL所对应的GPIO接口编号P1.5
#define I2C_ADDR 0x3c
#define somenop asm("nop");asm("nop");asm("nop");asm("nop");
//#define I2C_ADDR 0x3c
unsigned char buf0;
unsigned char buf1;
unsigned char i2c_step;


//短delay

/* I2C起始条件 */  
void i2c_start()  
{  
	//初始化GPIO口 
	SDAEN = 1;				//设置SDA方向为输出 
	SCLEN = 1;				//设置SCL方向为输出 
	SDA = 1;                //设置SDA为高电平
	asm("nop");
	SCL = 1;                //设置SCL为高电平   
	somenop
	somenop
	SDA = 0;                //SCL为高电平时，SDA由高变低  
	somenop
	somenop
	somenop
	somenop
	somenop
	somenop
	SCL = 0;
}  
/* I2C终止条件 */  
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
	SDA = 1;            //SCL高电平时，SDA由低变高
	somenop	
	SDAEN = 0;
//	SCL = 0;
}  
/*   
I2C读取ACK信号(写数据时使用)  
返回值 ：0表示ACK信号有效；非0表示ACK信号无效  
*/  
void i2c_read_ack()  
{   
	SCL = 0;               // SCL变低
	SDAEN = 0;				 //设置SDA方向为输入
	somenop	
	SCL = 1;              // SCL变高  
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
/* I2C发出ACK信号(读数据时使用) */  
void i2c_send_ack()  
{
	SCL = 0;              // SCL变低
	somenop
	SDA = 1;              //发出ACK信号
	SDAEN = 1;              //设置SDA方向为输出      
	asm("nop"); 
	asm("nop");
	asm("nop");	
	SCL = 1;             // SCL变高  
	somenop
	SCL = 0;
//	SDAEN = 1;
	asm("nop");
	asm("nop");
}  
/* I2C字节写 */ 

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
	SDAEN = 1;    //设置SDA方向为输出  
	SCL = 0;
	for (i2c_step = 8; i2c_step > 0; i2c_step --) 
	{  
		SCL = 0;             // SCL变低 
		asm("nop");
		if( b & (1 << (i2c_step - 1)))
			SDA = 1;
		else
			SDA = 0;
		asm("nop");
		SCL = 1;             // SCL变高  
		somenop
		SCL = 0;
	}  
	i2c_read_ack();
}

/* I2C字节读 */  
void i2c_read_byte()  
{   
	buf1 = 0;
	SDAEN = 0;				//设置SDA方向为输入
	for(i2c_step = 8; i2c_step > 0; i2c_step --) 
	{  
		SCL = 0;         // SCL变低  
		asm("nop");
		asm("nop");
		SCL = 1;
		//i2c_result = (i2c_result << 1) | SDA;
		buf1 = (buf1 << 1) | SDA;
							// SCL变高  
//		asm("nop");
//		asm("nop");
		SCL = 0;
	}
	i2c_send_ack();						//向目标设备发送ACK信号 
}  
/*  
I2C读操作  
addr：目标设备地址  
buf：读缓冲区  
len：读入字节的长度  
*/  
void i2c_read()  
{  
	i2c_start();							//起始条件，开始数据通信  
									//发送地址和数据读写方向  
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
	//读入数据  

	i2c_read_byte();

	i2c_stop();
	//终止条件，结束数据通信  
}  
/*  
I2C写操作  
addr：目标设备地址  
buf：写缓冲区  
len：写入字节的长度  
*/  
void i2c_write()  
{  
	i2c_start();							//起始条件，开始数据通信  
	//发送地址和数据读写方向 

	i2c_write_byte(I2C_ADDR<<1); 

	asm("nop");
	if(i2c_step == 0xff)
	{
		i2c_resume();
		return;
	}	
	
	//写入数据 

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
	i2c_stop();                     //终止条件，结束数据通信  
}