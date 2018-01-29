/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  模块名         : AD驱动模块
  文件名         : DriverAD.c
  相关文件       : DataStruct.h FuncDef.h Constant.h DataProcess.c ProtectLogic.c SwitchDriver.c
  实现功能       : AD初始化，初始化采样顺序。利用中断函数把AD采样数据存放到对应缓冲区。
  作者           : 林中一
  版本           : 1.0
  
  --------------------------------------------------------------------------------
  备注           : -
  --------------------------------------------------------------------------------
  
  修改记录       : 
    日  期          版本            修改人          修改内容 
    2010/06/01      1.0             林中一            创建

************************************************************************************/

#ifndef _DRIVER_AD

#define _DRIVER_AD
  #include "..\DataStruct.h"
#endif

//==============================================================================
//  函数名称   : InitAdc
//  功能描述   : CPU开机所有初始化工作，包括各个外设、参数、模块的初始化，初始化结束后启动中断，进入大循环
//  输入参数   : <无>
//  输出参数   ：<无>
//  返回值     : <无>
//  其他说明   : 
//  作者       ：林中一
//==============================================================================
void InitAdc(void)
{
    //P6SEL = 0xFF;                                                // 使能AD通道
    //P7SEL |= 0xF0;
   // REFCTL0 &= ~REFMSTR;   //5438A有REF模块，必须设置此位，ADC12才能使用内部参考电压
    ADC12CTL0 = ADC12ON + ADC12SHT0_3 + ADC12SHT1_3  + ADC12MSC;   // 打开AD，设置采样保持时间：8CLK   约2us
     // 采样时钟4分频、定时器A0.0输出启动、单次序列采样 + ADC12REFON                                                 
      

    ADC12CTL1 = ADC12SSEL_3 + ADC12DIV_3 +ADC12SHP + ADC12SHS_0 + ADC12CONSEQ_1;    //更改为手动启动，不用定时器启动
    //ADC12CTL2 = ADC12RES_2 + ADC12REFOUT;  //+ ADC12REFBURST

    ADC12MCTL0 = ADC12INCH_4 + ADC12SREF_2;                 // Vr+ = AVCC, channel = A6	I0
    ADC12MCTL1 = ADC12INCH_5 + ADC12SREF_2; 		    // Vr+ = AVCC, channel = A14 U0 
    ADC12MCTL2 = ADC12INCH_6 + ADC12SREF_2;                //UA
    ADC12MCTL3 = ADC12INCH_7 + ADC12SREF_2;                //UB
    ADC12MCTL4 = ADC12INCH_12 + ADC12SREF_2;//+ ADC12EOS; // Vr+ = VeREF+ (ext), channel = A12 Com
    ADC12MCTL5 = ADC12INCH_13 + ADC12SREF_2;// + ADC12EOS; //UPt
    ADC12MCTL6 = ADC12INCH_14 + ADC12SREF_2 + ADC12EOS; //UCap	
    //ADC12IE = ADC12IFG9;                                       // Enable ADC12IFG.9
    ADC12CTL0 |= ADC12ENC;                                  // Enable conversions
    //ADC12CTL0 |= ADC12SC;                                   // Start convn - software trigger
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{

}




