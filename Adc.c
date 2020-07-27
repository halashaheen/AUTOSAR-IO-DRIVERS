/*
 * Adc.c
 *
 *  Created on: Jul 24, 2020
 *  Author: moh1969
 */

#include "Adc.h"

// ex: ADC_CLK(ADC_MOD0,CLK_ENABLE);
void ADC_CLK(ADC_MOD ADC_module,CLK_MODE mode){
    SYSCTL_RCGCADC_R&=~(1<<ADC_module);
    SYSCTL_RCGCADC_R|=(mode<<ADC_module);
}

// ex : uint32_t ADC_ModuleBase=__ADCModuleBase(ADC_MOD0);
uint32_t __ADCModuleBase(ADC_MOD ADC_module){
    if(ADC_module==ADC_MOD0) return ADC0_BASE;
    else if(ADC_module==ADC_MOD1) return ADC1_BASE;
}

//if the trigger source is one of the pwm generators we have to select the pwm module using this function
// ex: __ADCTriggerPwmModule(ADC0_BASE,ADC_SS0,PWM_MOD0);
void __ADCTriggerPwmModule(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,PWM_MOD pwm_module){
    ADC_REG(ADC_module_base,ADC_O_TSSEL)&=~((0x03)<<(4*(2*ADC_sequencerNumber+1)));
    ADC_REG(ADC_module_base,ADC_O_TSSEL)|=(pwm_module<<(4*(2*ADC_sequencerNumber+1)));
}

// ex : __ADCTriggerSet(ADC0_BASE,ADC_SS0,ADC_TRIGGER_PROCESSOR);
void __ADCTriggerSet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber ,uint8_t ADC_triggerSource){

    ADC_REG(ADC_module_base,ADC_O_EMUX)&=~((0x0F)<<(4*ADC_sequencerNumber));
    ADC_REG(ADC_module_base,ADC_O_EMUX)|=((ADC_triggerSource)<<(4*ADC_sequencerNumber));

    if(ADC_triggerSource==ADC_TRIGGER_PWM0||ADC_triggerSource==ADC_TRIGGER_PWM1||
       ADC_triggerSource==ADC_TRIGGER_PWM2||ADC_triggerSource==ADC_TRIGGER_PWM3){
        __ADCTriggerPwmModule(ADC_module_base,ADC_sequencerNumber,PWM_MOD0); }

}



// ex: __ADCPrioritySet(ADC0_BASE,ADC_SS0,ADC_SS_PRI2);
void __ADCPrioritySet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber ,uint8_t ADC_SequencerPriority){
    ADC_REG(ADC_module_base,ADC_O_SSPRI)&=~((0x03)<<(4*ADC_sequencerNumber));
    ADC_REG(ADC_module_base,ADC_O_SSPRI)|=((ADC_SequencerPriority)<<(4*ADC_sequencerNumber));
}


// ex: ADCSampleSequencerConfigure(ADC0_BASE,ADC_SS0,ADC_TRIGGER_PROCESSOR,ADC_SS_PRI2);
void ADCSampleSequencerConfigure(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                 uint8_t ADC_triggerSource,uint8_t ADC_SequencerPriority)
{
    __ADCTriggerSet(ADC_module_base,ADC_sequencerNumber, ADC_triggerSource);
    __ADCPrioritySet(ADC_module_base,ADC_sequencerNumber, ADC_SequencerPriority);
}


// ex: ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample1,(AIN0_PE3|X))
//  X: set or clear each of [ADC_CTL_TS-ADC_CTL_IE-ADC_CTL_END-ADC_CTL_D-ADC_CTL_NONE]

/*
 *if we want (set bit) any of these configurations [ADC_CTL_TS-ADC_CTL_IE-ADC_CTL_END-ADC_CTL_D-ADC_CTL_NONE] we put it ORed in the
 *second parameter of macro ADC_ConfigPin(pin,config), else if we want to clear bit
 *we just dont't write the config we don't want.
 *
 *1-ADC_CTL_TS  ->  0: the input pin analog value is read and converted
 *                  1: the temperature sensor is read and converted
 *
 *2-ADC_CTL_IE  ->  related to interrupt
 *3-ADC_CTL_END ->  0: sampling and conversion continues
 *                  1: the end of the sequence
 *
 *4-ADC_CTL_D   ->  0: single-ended
 *                  1: differential and both pair-pins must be configured
 *
 * ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample0,ADC_ConfigPin(AIN8_PE5,ADC_CTL_TS|ADC_CTL_IE));
 * ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample1,ADC_ConfigPin(AIN10_PB4,ADC_CTL_TS));
 * ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample2,ADC_ConfigPin(AIN0_PE3,ADC_CTL_END));
 *
 * if we choose differential end we have to configure both pins for example
 * ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample0,ADC_ConfigPin(AIN0_PE3,ADC_CTL_D));
 * ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample1,ADC_ConfigPin(AIN1_PE2,ADC_CTL_D));
 * ADCSampleSequencerPinConfigure(ADC0_BASE,ADC_SS0,ADC_Sample2,ADC_ConfigPin(AIN9_PE4,ADC_CTL_END));
 *
 */
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
    ADC_REG(ADC_module_base,ADCSSMUXn_offset)&=~((0x000F)<<(4*ADC_PinSampleOrder));
    ADC_REG(ADC_module_base,ADCSSMUXn_offset)|=((ADC_PIN)<<(4*ADC_PinSampleOrder));

    //PIN configurations
    ADC_REG(ADC_module_base,ADCSSCTLn_offset)&=~((0x000F)<<(4*ADC_PinSampleOrder));
    ADC_REG(ADC_module_base,ADCSSCTLn_offset)|=((ADC_PinConfig&0x0F)<<(4*ADC_PinSampleOrder));

    //Disable digital comparator

    //==> Interrupt ....
    /*
     *
     *
     */
}


// ex: ADCSampleSequencerEnable(ADC0_BASE,ADC_SS0);
void ADCSampleSequencerEnable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber){
    ADC_REG(ADC_module_base,ADC_O_ACTSS)|=(1<<ADC_sequencerNumber);}

// ex: ADCSampleSequencerDisable(ADC0_BASE,ADC_SS0);
void ADCSampleSequencerDisable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber){
    ADC_REG(ADC_module_base,ADC_O_ACTSS)&=~(1<<ADC_sequencerNumber);}

/*
 * #define n 8
 *
 * uint8_t sample=0;
 * uint32_t BufferData[n];
 * uint32_t* BufferPtr=BufferData; //name of array is pointer
 *
 * uint32_t n_samples=ADCSequenceDataGet(ADC0_BASE,ADC_SS0,BufferPtr);
 *
 * for(;sample<n_sample;sample++)
 *  BufferData[sample]=*(BufferPtr+sample);
 *
 */

//absolutely different approach ...

uint32_t ADCSequenceDataGet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,uint32_t *BufferPtr){


    uint32_t ADCSSFIFOn_offset,ADCSSFSTATn_offset;
    if(ADC_sequencerNumber==ADC_SS0){
        ADCSSFIFOn_offset=ADC_O_SSFIFO0;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT0;}

    else if(ADC_sequencerNumber==ADC_SS1){
        ADCSSFIFOn_offset=ADC_O_SSFIFO1;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT1;}

    else if(ADC_sequencerNumber==ADC_SS2){
        ADCSSFIFOn_offset=ADC_O_SSFIFO2;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT2;}

    else if(ADC_sequencerNumber==ADC_SS3){
        ADCSSFIFOn_offset=ADC_O_SSFIFO3;
        ADCSSFSTATn_offset=ADC_O_SSFSTAT3;}

     uint8_t n_samples=0;

     while(!(ADC_REG(ADC_module_base,ADCSSFSTATn_offset)&EMPTY)){
         *BufferPtr++=ADC_REG(ADC_module_base,ADCSSFIFOn_offset);
         n_samples++;
     }
     return n_samples;
}

/*
uint32_t ADCSequenceDataGet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,uint32_t *BufferPtr){

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


     uint8_t RD_Index=ADC_REG(ADC_module_base,ADCSSFSTATn_offset)&read_ptr;
     uint8_t WR_Index=(ADC_REG(ADC_module_base,ADCSSFSTATn_offset)&Write_ptr)>>4;
     uint8_t n_samples=0;

     //pointer=location in FIFO
     BufferPtr=(uint32_t *)(ADCSSFIFOn_offset+RD_Index);

     while(RD_Index!=WR_Index){
         n_samples++;
         if(RD_Index==SS_MAXlimit){RD_Index=0x00;}
         RD_Index++;
     }
     return n_samples;
}
*/
//support both individual ADC sequencer and conncurrent sampling
/*
 * individual sampling
 * --------------------
 * ADC_InitiateConversionBySW(ADC0_BASE,ADC_SS0,individualSamplingMode,individual);
 *
 * concurrent sampling
 * -------------------
 *  ADC_InitiateConversionBySW(ADC0_BASE,ADC_SS0,concurrentSamplingMode,Concurrent_notLast);
 *  ADC_InitiateConversionBySW(ADC1_BASE,ADC_SS2,concurrentSamplingMode,Concurrent_Last);
 *
 *  this makes Both ADC module 1 and module 2 start conversion at the same time
 *
 *
 */
/*
 *
 * inside an ADC module
 *                     - Never all "ss" sample sequencers concurrently sample ,only one at time
 *                     - the order which one executes before the other is based on the priority of the ss
 *
 * ADC module
 *              - individual mode : means the first configured is the first ready to sample
 *              - concurrent mode : means both modules will sample at the same time
 *
 *
 */
void ADC_InitiateConversionBySW(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                ADC_SW_ConversionMode conversionMode,IsLastConcurrentModule flag){

    ADC_REG(ADC_module_base,ADC_O_PSSI)|=(1<<ADC_sequencerNumber); //SSn Initiate

    if(conversionMode==individualSamplingMode)
        ADC_REG(ADC_module_base,ADC_O_PSSI)&=~(1<<SYNCWAIT_BIT);//Sampling begins when a sample sequence has been initiated.
    else if(conversionMode==concurrentSamplingMode)
        ADC_REG(ADC_module_base,ADC_O_PSSI)|=(1<<SYNCWAIT_BIT);//This bit allows the sample sequences to be initiated,
                                                               //but delays sampling until the GSYNC bit is set.
    if(flag==Concurrent_Last)
        ADC_REG(ADC_module_base,ADC_O_PSSI)|=(uint32_t)(1<<GSYNC_BIT);


}



void ADCSequencerProcessorTrigger(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber){

    ADC_REG(ADC_module_base,ADC_O_PSSI)|=((0xFFFF0000)|(1<<ADC_sequencerNumber));
}



