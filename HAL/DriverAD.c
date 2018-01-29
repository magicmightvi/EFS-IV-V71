/**********************************************************************************
  Copyright (C) 1988-2007, CREAT Company 
  ģ����         : AD����ģ��
  �ļ���         : DriverAD.c
  ����ļ�       : DataStruct.h FuncDef.h Constant.h DataProcess.c ProtectLogic.c SwitchDriver.c
  ʵ�ֹ���       : AD��ʼ������ʼ������˳�������жϺ�����AD�������ݴ�ŵ���Ӧ��������
  ����           : ����һ
  �汾           : 1.0
  
  --------------------------------------------------------------------------------
  ��ע           : -
  --------------------------------------------------------------------------------
  
  �޸ļ�¼       : 
    ��  ��          �汾            �޸���          �޸����� 
    2010/06/01      1.0             ����һ            ����

************************************************************************************/

#ifndef _DRIVER_AD

#define _DRIVER_AD
  #include "..\DataStruct.h"
#endif

//==============================================================================
//  ��������   : InitAdc
//  ��������   : CPU�������г�ʼ�������������������衢������ģ��ĳ�ʼ������ʼ�������������жϣ������ѭ��
//  �������   : <��>
//  �������   ��<��>
//  ����ֵ     : <��>
//  ����˵��   : 
//  ����       ������һ
//==============================================================================
void InitAdc(void)
{
    //P6SEL = 0xFF;                                                // ʹ��ADͨ��
    //P7SEL |= 0xF0;
   // REFCTL0 &= ~REFMSTR;   //5438A��REFģ�飬�������ô�λ��ADC12����ʹ���ڲ��ο���ѹ
    ADC12CTL0 = ADC12ON + ADC12SHT0_3 + ADC12SHT1_3  + ADC12MSC;   // ��AD�����ò�������ʱ�䣺8CLK   Լ2us
     // ����ʱ��4��Ƶ����ʱ��A0.0����������������в��� + ADC12REFON                                                 
      

    ADC12CTL1 = ADC12SSEL_3 + ADC12DIV_3 +ADC12SHP + ADC12SHS_0 + ADC12CONSEQ_1;    //����Ϊ�ֶ����������ö�ʱ������
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




