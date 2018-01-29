#define DEV_I2C_GLOBALS
#include  "..\DataStruct.h"

#define  I2CSDA             BIT1 
#define  I2CSCL             BIT2
#define  I2CSDA_SET_1       P9OUT |=  I2CSDA
#define  I2CSDA_SET_0       P9OUT &= (~I2CSDA)
#define  I2CSCL_SET_1       P9OUT |=  I2CSCL
#define  I2CSCL_SET_0       P9OUT &= (~I2CSCL)
#define  I2CSDA_INPUT_IN    P9IN & I2CSDA


#define Input    0
#define Output   1
/*
0 : input
1 : outpou*/
void I2C_Pins_DIR_Setting (INT8U SDADIR )
{
    
    if(SDADIR == 0)               //input
    {
        P9OUT &= ~I2CSDA;; 
        P9DIR &= ~I2CSDA;;                       
    }
    else
    {
        P9DIR  |= I2CSDA;  //Both output
    }
}

void I2C_start(void)
{
    I2CSDA_SET_1;__delay_cycles(80);   
    I2CSCL_SET_1;__delay_cycles(80);    
    I2CSDA_SET_0;__delay_cycles(80);    
    I2CSCL_SET_0;__delay_cycles(80);    
}
void I2C_stop(void)
{ 
   I2CSCL_SET_0;__delay_cycles(20);
   I2CSDA_SET_0;__delay_cycles(60);
   I2CSCL_SET_1;__delay_cycles(50);
   I2CSDA_SET_1;__delay_cycles(20);
}       
void I2C_sendack(void)
{     
   I2CSDA_SET_0;__delay_cycles(80);
   I2CSCL_SET_1;__delay_cycles(80);
   I2CSCL_SET_0;__delay_cycles(80);
   I2CSDA_SET_1;__delay_cycles(20);
}
void I2C_sendnotack(void)   
{ 
   I2CSDA_SET_1;__delay_cycles(60);
   I2CSCL_SET_1;__delay_cycles(60);
   I2CSCL_SET_0;__delay_cycles(20);
}   

void I2C_senddata (INT8U I_data )
{
    INT8U count;
    
    for(count = 0; count < 8; count++)
    {
        I2CSCL_SET_0;__delay_cycles(20);
        if(I_data & 0x80)
        {
            I2CSDA_SET_1;__delay_cycles(20);
        }
        else
        {
            I2CSDA_SET_0;__delay_cycles(20);
        }
        __delay_cycles(60);
        I2CSCL_SET_1;__delay_cycles(60);
        I_data <<= 1;        
    }
    __delay_cycles(10);
}

INT8U I2C_receivedata( void )
{	 
    INT8U count,readbyte = 0;

    I2C_Pins_DIR_Setting(Input);
    I2CSCL_SET_0; __delay_cycles(60);
    for(count = 0; count < 8; count++)
    {
        readbyte = (readbyte << 1); __delay_cycles(20);
        I2CSCL_SET_1; __delay_cycles(60);
        if(I2CSDA_INPUT_IN)
        {
            readbyte |= 0x01;__delay_cycles(20);
        }
        else
        {
            readbyte &= 0xFE;__delay_cycles(20);
        }
        __delay_cycles(60);
        I2CSCL_SET_0;__delay_cycles(60);
    }
    I2CSCL_SET_0;__delay_cycles(60);
    I2C_Pins_DIR_Setting(Output);
    __delay_cycles(20);
    
    return readbyte;
}

//**************************************************
//  I2C应答函数，等待从器件接受方的应答
//****************************************************
INT8U I2C_ack ( void )
{    
    static INT16U errortime = 65500;
    
    I2CSCL_SET_0;
	__delay_cycles(20);
    I2CSDA_SET_1;
	__delay_cycles(20);
    I2C_Pins_DIR_Setting(Input);

    I2CSCL_SET_1;

    while(I2CSDA_INPUT_IN)
    {
        FEED_WATCH_DOG();
		//WatchDogClose();
		__delay_cycles(30);
        errortime--;
        __delay_cycles(20);
        if ( !errortime )
        {
            I2C_Pins_DIR_Setting(Output);  //Set SDA output
            I2C_stop();
            return ( FALSE );
        }
        __delay_cycles(20);
    }
    I2C_Pins_DIR_Setting(Output);;  //Set SDA output
    __delay_cycles(20);
    I2CSCL_SET_0;
    return ( TRUE );
}

