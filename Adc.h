/*
 * Adc.h
 *
 *  Created on: Jul 24, 2020
 *      Author: Hala Shaheen
 */



/*
 * we have analog signal so at certain sample we want to know the corresponding digital value ,
 * so the concept of sample in a point on analg signal converted to digital ,
 * sequence is group of samples
 * sample sequencer is responsible for taking N samples where N is dependent on sample sequencer number,
 * and the end bit [if sample's end of sequence bit is enabled then this is the last sample in sequence
 * meaning that sequence length is end bit+1 ] samples
 *
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdbool.h>
#include <stdint.h>
#include "Pwm.h"

#define SYSCTL_RCGCADC_R        (*((volatile uint32_t *)0x400FE638))

typedef enum{
    ADC_MOD0,
    ADC_MOD1
}ADC_MOD;
typedef enum{
    CLK_DIABLE,
    CLK_ENABLE
}CLK_MODE;

// ADC PinID
#define AIN0_PE3 0x00
#define AIN1_PE2 0x10
#define AIN2_PE1 0x20
#define AIN3_PE0 0x30
#define AIN4_PD3 0x40
#define AIN5_PD2 0x50
#define AIN6_PD1 0x60
#define AIN7_PD0 0x70
#define AIN8_PE5 0x80
#define AIN9_PE4 0x90
#define AIN10_PB4 0xA0
#define AIN11_PB5 0xB0


typedef enum{
    ADC_Sample0,
    ADC_Sample1,
    ADC_Sample2,
    ADC_Sample3,
    ADC_Sample4,
    ADC_Sample5,
    ADC_Sample6,
    ADC_Sample7

}ADC_SampleOrder;

//ADC module base address
#define ADC0_BASE  0x40038000
#define ADC1_BASE  0x40039000

//ADC sequencer numbers
#define ADC_SS0   0x00
#define ADC_SS1   0x01
#define ADC_SS2   0x02
#define ADC_SS3   0x03

//ADC sequencer priority levels
#define ADC_SS_PRI0 0x00  /*HIGHEST*/
#define ADC_SS_PRI1 0x01
#define ADC_SS_PRI2 0x02
#define ADC_SS_PRI3 0x03  /*LOWEST*/


// The following are defines for the ADC register offsets
#define ADC_O_ACTSS             0x00000000  // ADC Active Sample Sequencer
#define ADC_O_EMUX              0x00000014  // ADC Event Multiplexer Select
#define ADC_O_TSSEL             0x0000001C  // ADC Trigger Source Select
#define ADC_O_SSPRI             0x00000020  // ADC Sample Sequencer Priority
#define ADC_O_PSSI              0x00000028  // ADC Processor Sample Sequence Initiate

#define ADC_O_SSMUX0            0x00000040  // ADC Sample Sequence Input Multiplexer Select 0
#define ADC_O_SSCTL0            0x00000044  // ADC Sample Sequence Control 0
#define ADC_O_SSFIFO0           0x00000048  // ADC Sample Sequence Result FIFO 0
#define ADC_O_SSFSTAT0          0x0000004C  // ADC Sample Sequence FIFO 0 Status

#define ADC_O_SSMUX1            0x00000060  // ADC Sample Sequence Input Multiplexer Select 1
#define ADC_O_SSCTL1            0x00000064  // ADC Sample Sequence Control 1
#define ADC_O_SSFIFO1           0x00000068  // ADC Sample Sequence Result FIFO 1
#define ADC_O_SSFSTAT1          0x0000006C  // ADC Sample Sequence FIFO 1 Status

#define ADC_O_SSMUX2            0x00000080  // ADC Sample Sequence Input Multiplexer Select 2
#define ADC_O_SSCTL2            0x00000084  // ADC Sample Sequence Control 2
#define ADC_O_SSFIFO2           0x00000088  // ADC Sample Sequence Result FIFO 2
#define ADC_O_SSFSTAT2          0x0000008C  // ADC Sample Sequence FIFO 2 Status

#define ADC_O_SSMUX3            0x000000A0  // ADC Sample Sequence Input Multiplexer Select 3
#define ADC_O_SSCTL3            0x000000A4  // ADC Sample Sequence Control 3
#define ADC_O_SSFIFO3           0x000000A8  // ADC Sample Sequence Result FIFO 3
#define ADC_O_SSFSTAT3          0x000000AC  // ADC Sample Sequence FIFO 3 Status


//ADC trigger source offsets

#define ADC_TRIGGER_PROCESSOR   0x00000000  // Processor event
#define ADC_TRIGGER_COMP0       0x00000001  // Analog comparator 0 event
#define ADC_TRIGGER_COMP1       0x00000002  // Analog comparator 1 event
#define ADC_TRIGGER_COMP2       0x00000003  // Analog comparator 2 event
#define ADC_TRIGGER_EXTERNAL    0x00000004  // External event
#define ADC_TRIGGER_TIMER       0x00000005  // Timer event
#define ADC_TRIGGER_PWM0        0x00000006  // PWM0 event
#define ADC_TRIGGER_PWM1        0x00000007  // PWM1 event
#define ADC_TRIGGER_PWM2        0x00000008  // PWM2 event
#define ADC_TRIGGER_PWM3        0x00000009  // PWM3 event
#define ADC_TRIGGER_NEVER       0x0000000E  // Never Trigger
#define ADC_TRIGGER_ALWAYS      0x0000000F  // Always event
#define ADC_TRIGGER_PWM_MOD0    0x00000000  // PWM triggers from PWM0
#define ADC_TRIGGER_PWM_MOD1    0x00000010  // PWM triggers from PWM1


#define ADC_ConfigPin(ADC_PIN,CONFIG) ((ADC_PIN<<4)|(CONFIG))
//                                                7...... 4  3  2  1  0
// ADC Pin configurations offsets ADC_PinConfig : ADC_PinID TS IE END D
#define ADC_PinID               0xF0  // PIN_ID
#define ADC_CTL_TS              0x08  // Temperature sensor select
#define ADC_CTL_IE              0x04  // Interrupt enable
#define ADC_CTL_END             0x02  // Sequence end select
#define ADC_CTL_D               0x01  // Differential select
#define ADC_CTL_NONE            0x00  //single ended , not end, disable interrupt, don't ake value of temperature
                                      // and convert the associated pin analog signal

// ADC FIFO Status Offsets
#define read_ptr 0x0F
#define Write_ptr 0xF0
#define EMPTY 0x100
#define FULL  0x1000

// ADC Processor Sample Sequence Initiate (ADC_O_PSSI)
#define SYNCWAIT_BIT 27
#define GSYNC_BIT    31

typedef enum {
    individualSamplingMode,
    concurrentSamplingMode
}ADC_SW_ConversionMode;

typedef enum{
    individual,
    Concurrent_notLast=0,
    Concurrent_Last
}IsLastConcurrentModule;

#define ADC_REG(module_base,reg_offset) (*((volatile uint32_t*) (module_base+reg_offset)))

/*NON-AUTOSAR APIs*/



void ADC_CLK(ADC_MOD ADC_module,CLK_MODE mode);

void ADCSampleSequencerConfigure(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                 uint8_t ADC_triggerSource, uint8_t ADC_SequencerPriority);

void ADCSampleSequencerPinConfigure(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                    ADC_SampleOrder ADC_PinSampleOrder ,uint8_t ADC_PinConfig);

void ADCSampleSequencerEnable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber);
void ADCSampleSequencerDisable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber);

uint32_t ADCSequenceDataGet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                           uint32_t *BufferPtr);


void ADC_InitiateConversionBySW(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                ADC_SW_ConversionMode conversionMode,IsLastConcurrentModule flag);

void ADCSequencerProcessorTrigger(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber);


/*AUTOSAR APIs*/

/*
void Adc_Init ( const Adc_ConfigType* ConfigPtr );
Std_ReturnType Adc_SetupResultBuffer ( Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr );
void Adc_DeInit ( void );


void Adc_StartGroupConversion ( Adc_GroupType Group );
void Adc_StopGroupConversion ( Adc_GroupType Group );
Std_ReturnType Adc_ReadGroup ( Adc_GroupType Group, Adc_ValueGroupType* DataBufferPtr );

void Adc_EnableHardwareTrigger ( Adc_GroupType Group );
void Adc_DisableHardwareTrigger ( Adc_GroupType Group );
void Adc_EnableGroupNotification ( Adc_GroupType Group );
void Adc_DisableGroupNotification ( Adc_GroupType Group );
Adc_StatusType Adc_GetGroupStatus ( Adc_GroupType Group );
Adc_StreamNumSampleType Adc_GetStreamLastPointer ( Adc_GroupType Group, Adc_ValueGroupType** PtrToSamplePtr );
*/

#endif /* ADC_H_ */



