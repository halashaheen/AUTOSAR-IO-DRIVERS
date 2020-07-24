/*
 * Adc.c
 *
 *  Created on: Jul 24, 2020
 *  Author: moh1969
 */

#include "Adc.h"

// ex : __ADCTriggerSet(ADC0_BASE,ADC_SS0,ADC_TRIGGER_PROCESSOR);
void __ADCTriggerSet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber ,uint16_t ADC_triggerSource){

    ADC_REG(ADC_module_base,ADC_O_EMUX)&=~((0x0003)<<ADC_sequencerNumber);
    ADC_REG(ADC_module_base,ADC_O_EMUX)|=((ADC_triggerSource)<<ADC_sequencerNumber);
    //for pwm
    /*
    if(ADC_triggerSource==ADC_EMUX_EM3_PWM0||ADC_triggerSource==ADC_EMUX_EM3_PWM1||
       ADC_triggerSource==ADC_EMUX_EM3_PWM2||ADC_triggerSource==ADC_EMUX_EM3_PWM3||

       ADC_triggerSource==ADC_EMUX_EM2_PWM0||ADC_triggerSource==ADC_EMUX_EM2_PWM1||
       ADC_triggerSource==ADC_EMUX_EM2_PWM2||ADC_triggerSource==ADC_EMUX_EM2_PWM3||

       ADC_triggerSource==ADC_EMUX_EM1_PWM0||ADC_triggerSource==ADC_EMUX_EM1_PWM1||
       ADC_triggerSource==ADC_EMUX_EM1_PWM2||ADC_triggerSource==ADC_EMUX_EM1_PWM3||

       ADC_triggerSource==ADC_EMUX_EM0_PWM0||ADC_triggerSource==ADC_EMUX_EM0_PWM1||
       ADC_triggerSource==ADC_EMUX_EM0_PWM2||ADC_triggerSource==ADC_EMUX_EM0_PWM3){}*/

}

// ex: __ADCPrioritySet(ADC0_BASE,ADC_SS0,ADC_SS_PRI2);
void __ADCPrioritySet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber ,uint8_t ADC_SequencerPriority){
    ADC_REG(ADC_module_base,ADC_O_SSPRI)&=~((0x0003)<<(4*ADC_sequencerNumber));
    ADC_REG(ADC_module_base,ADC_O_SSPRI)|=((ADC_SequencerPriority)<<(4*ADC_sequencerNumber));
}


// ex: ADCSampleSequencerConfigure(ADC0_BASE,ADC_SS0,ADC_TRIGGER_PROCESSOR,ADC_SS_PRI2);
void ADCSampleSequencerConfigure(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                   uint16_t ADC_triggerSource, uint8_t ADC_SequencerPriority)
{
    __ADCTriggerSet(ADC_module_base,ADC_sequencerNumber, ADC_triggerSource);
    __ADCPrioritySet(ADC_module_base,ADC_sequencerNumber, ADC_SequencerPriority);
}

// ex: ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample1,(AIN0_PE3|X)
//  X: set or clear each of [ADC_CTL_TS-ADC_CTL_IE-ADC_CTL_END-ADC_CTL_D]
void ADCSampleSequencerPinConfigure(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                    ADC_SampleOrder ADC_PinSampleOrder ,uint8_t ADC_PinConfig)
{
    uint32_t ADCSSMUXn_offset,ADCSSCTLn_offset;
    uint8_t ADC_PIN =(ADC_PinConfig&ADC_PinID)>>4;

    if(ADC_sequencerNumber==ADC_SS0){
        ADCSSMUXn_offset=ADC_O_SSMUX0;
        ADCSSCTLn_offset=ADC_O_SSCTL0;}

    else if(ADC_sequencerNumber==ADC_SS1){
        ADCSSMUXn_offset=ADC_O_SSMUX1;
        ADCSSCTLn_offset=ADC_O_SSCTL1;}

    else if(ADC_sequencerNumber==ADC_SS2){
            ADCSSMUXn_offset=ADC_O_SSMUX2;
            ADCSSCTLn_offset=ADC_O_SSCTL2;}

    else if(ADC_sequencerNumber==ADC_SS3){
            ADCSSMUXn_offset=ADC_O_SSMUX3;
            ADCSSCTLn_offset=ADC_O_SSCTL3;}

    //PIN which is the input source at sample of order ADC_PinSampleOrder
    ADC_REG(ADC_module_base,ADCSSMUXn_offset)&=~((0x000C)<<(4*ADC_PinSampleOrder));
    ADC_REG(ADC_module_base,ADCSSMUXn_offset)|=((ADC_PIN)<<(4*ADC_PinSampleOrder));

    //PIN configurations
    ADC_REG(ADC_module_base,ADCSSCTLn_offset)&=~((0x00FF)<<(4*ADC_PinSampleOrder));
    ADC_REG(ADC_module_base,ADCSSCTLn_offset)|=((ADC_PinConfig)<<(4*ADC_PinSampleOrder));

    //==> Interrupt ....
    /*
     *
     *
     */
}

void ADCSampleSequencerEnable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber){
    ADC_REG(ADC_module_base,ADC_O_ACTSS)|=(1<<ADC_sequencerNumber);}

void ADCSampleSequencerDisable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber){
    ADC_REG(ADC_module_base,ADC_O_ACTSS)&=~(1<<ADC_sequencerNumber);}

int32_t ADCSequenceDataGet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,uint32_t *BufferPtr){

    uint8_t SS_MAXlimit=0;
    uint32_t ADCSSFIFOn_offset,ADCSSFSTATn_offset;
    if(ADC_sequencerNumber==ADC_SS0){
        ADCSSFIFOn_offset=ADC_O_SSFIFO0;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT0;
        SS_MAXlimit=0x07;}

    else if(ADC_sequencerNumber==ADC_SS1){
        ADCSSFIFOn_offset=ADC_O_SSFIFO1;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT1;
        SS_MAXlimit=0x04;}

    else if(ADC_sequencerNumber==ADC_SS2){
        ADCSSFIFOn_offset=ADC_O_SSFIFO2;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT2;
        SS_MAXlimit=0x04;}

    else if(ADC_sequencerNumber==ADC_SS3){
        ADCSSFIFOn_offset=ADC_O_SSFIFO3;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT3;
        SS_MAXlimit=0x01;}

    //==> pointers need rechecking ...
     uint32_t * RD_Ptr=ADC_REG(ADC_module_base,ADCSSFSTATn_offset)&read_ptr;
     uint32_t * WR_Ptr=ADC_REG(ADC_module_base,ADCSSFSTATn_offset)&Write_ptr;
     uint8_t n_samples=0;
     BufferPtr=RD_Ptr;
     while(RD_Ptr!=WR_Ptr){
         n_samples++;
         if(RD_Ptr==SS_MAXlimit){*RD_Ptr=0x00;}
         RD_Ptr++;
     }
     return n_samples;
}

void ADCSequencerProcessorTrigger(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber){
    //==> not sure ....
    ADC_REG(ADC_module_base,ADC_O_PSSI)&=~(1<<ADC_sequencerNumber);
}
