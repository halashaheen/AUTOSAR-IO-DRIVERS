/*
 * Adc.h
 *
 *  Created on: Jul 24, 2020
 *      Author: moh1969
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum{
    ADC_MOD0,
    ADC_MOD1
}ADC_MOD;

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


//                                                7...... 4  3  2  1  0
// ADC Pin configurations offsets ADC_PinConfig : ADC_PinID TS IE END D
#define ADC_PinID               0xF0  // PIN_ID
#define ADC_CTL_TS              0x08  // Temperature sensor select
#define ADC_CTL_IE              0x04  // Interrupt enable
#define ADC_CTL_END             0x02  // Sequence end select
#define ADC_CTL_D               0x01  // Differential select

// ADC FIFO Status Offsets
#define read_ptr 0x0F
#define Write_ptr 0xF0
#define EMPTY 0x100
#define FULL  0x800

#define ADC_REG(module_base,reg_offset) (*((volatile uint32_t*) (module_base+reg_offset)))

/*NON-AUTOSAR APIs*/


//==> clock functions ....

void ADCSampleSequencerConfigure(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                 uint16_t ADC_triggerSource, uint8_t ADC_SequencerPriority);

void ADCSampleSequencerPinConfigure(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                                    ADC_SampleOrder ADC_PinSampleOrder ,uint8_t ADC_PinConfig);

void ADCSampleSequencerEnable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber);
void ADCSampleSequencerDisable(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber);

int32_t ADCSequenceDataGet(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber,
                           uint32_t *BufferPtr);

void ADCSequencerProcessorTrigger(uint32_t ADC_module_base, uint8_t ADC_sequencerNumber);


#endif /* ADC_H_ */
