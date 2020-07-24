/*
 * Pwm.h
 *
 *  Created on: Jul 23, 2020
 *      Author: moh1969
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdbool.h>
#include <stdint.h>

// clock registers
#define SYSCTL_RCGCPWM_R        (*((volatile uint32_t *)0x400FE640))
#define SYSCTL_RCC_R            (*((volatile uint32_t *)0x400FE060))

#define SYSCTL_RCC_USEPWMDIV    0x00100000  // Enable PWM Clock Divisor
#define SYSCTL_RCC_PWMDIV_2     0x00000000  // PWM clock /2
#define SYSCTL_RCC_PWMDIV_4     0x00020000  // PWM clock /4
#define SYSCTL_RCC_PWMDIV_8     0x00040000  // PWM clock /8
#define SYSCTL_RCC_PWMDIV_16    0x00060000  // PWM clock /16
#define SYSCTL_RCC_PWMDIV_32    0x00080000  // PWM clock /32
#define SYSCTL_RCC_PWMDIV_64    0x000A0000  // PWM clock /64

typedef enum {
    sys_clk_div2,
    sys_clk_div4,
    sys_clk_div8,
    sys_clk_div16,
    sys_clk_div32,
    sys_clk_div64,
    none

}clkDiv;

typedef enum{
    //PWM MODULE 0
    PWM0_PB6,
    PWM0_PB7,
    PWM0_PB4,
    PWM0_PB5,
    PWM0_PE4,
    PWM0_PE5,
    PWM0_PC4,
    PWM0_PD0 =6,
    PWM0_PC5,
    PWM0_PD1,
    //PWM MODULE 1
    PWM1_PD0,
    PWM1_PD1,
    PWM1_PA6,
    PWM1_PE4 =2,
    PWM1_PA7,
    PWM1_PE5=3,
    PWM1_PF0,
    PWM1_PF1,
    PWM1_PF2,
    PWM1_PF3

}PwmChannelId;

typedef uint8_t PWM_MOD;
#define PWM_MOD0 0x00
#define PWM_MOD1 0x01

typedef uint8_t PWM_GEN;
#define PWM_GEN0 0x00
#define PWM_GEN1 0x01
#define PWM_GEN2 0x02
#define PWM_GEN3 0x03

// AUTOSAR Type Definitions
typedef uint8_t Pwm_ChannelType;
typedef uint16_t Pwm_PeriodType;

typedef enum{
    PWM_HIGH,
    PWM_LOW
}Pwm_OutputStateType;

typedef enum{
    PWM_VARIABLE_PERIOD,
    PWM_FIXED_PERIOD,
    PWM_FIXED_PERIOD_SHIFTED
}Pwm_ChannelClassType;

typedef enum{
    PWM_RISING_EDGE,
    PWM_FALLING_EDGE,
    PWM_BOTH_EDGES
}Pwm_EdgeNotificationType;


//Helper types

typedef uint16_t Pwm_DutyCycle;
#define DutyCycle_100 0x8000
#define DutyCycle_0   0x0000

typedef enum {
    PWM_polarity_neg,
    PWM_polarity_pos
}Pwm_Polarity;

typedef enum{
    Down_counterMode,
    Up_Down_counterMode
}PWM_GeneratorMode;

typedef enum{
    output_disable,
    output_enable
}OutputState;

typedef enum{
    Gen_disable,
    Gen_enable
}GenState;

typedef enum{
    pwm_idle_low,
    pwm_idle_high

}PwmIdleState;

#define uninitialized 0x00
#define initialized   0x01

//AUTOSAR Config struct
typedef struct{

    Pwm_ChannelType PwmChannelId;
    Pwm_PeriodType period; // period = N actual - period =N *clk period - N=clk freq/pwm freq clk - freq =sys clk/prescaler
    Pwm_DutyCycle  dutyCycle; // pos polarity dutycycle= high/total neg polarity dutycycle= low/total
    Pwm_Polarity polarity; // starting output is high or low
    PwmIdleState idleState; // output of pwm when calling Pwm_SetOutputToIdle or Pwm_DeInit
    Pwm_ChannelClassType channelClass;

    PWM_GeneratorMode genMode;
    clkDiv SysClkDiv;
}Pwm_ConfigType;

#define  pwm_reg(module_base,gen_off,reg_off) (*((volatile uint32_t *)(module_base+gen_off+reg_off)))

//PWM registers

// pwm module base address
#define PWM0_BASE               0x40028000  // Pulse Width Modulator (PWM)
#define PWM1_BASE               0x40029000  // Pulse Width Modulator (PWM)

// master registers offsets for each module (one for each module) reg =module_base +this offset
#define PWM_O_CTL               0x00000000  // PWM Master Control
#define PWM_O_SYNC              0x00000004  // PWM Time Base Sync
#define PWM_O_ENABLE            0x00000008  // PWM Output Enable

// registers offset for each generator (one for each generator) reg=module base + generator offset +this offset
#define PWM_O_X_CTL             0x00000000  // Gen Control Reg
#define PWM_O_X_LOAD            0x00000010  // Gen Load Reg
#define PWM_O_X_COUNT           0x00000014  // Gen Counter Reg
#define PWM_O_X_CMPA            0x00000018  // Gen Compare A Reg
#define PWM_O_X_CMPB            0x0000001C  // Gen Compare B Reg
#define PWM_O_X_GENA            0x00000020  // Gen Generator A Ctrl Reg
#define PWM_O_X_GENB            0x00000024  // Gen Generator B Ctrl Reg

// bit fields of generator/counter control register PWM_O_X_CTL
#define PWM_X_CTL_MODE          0x00000002  // Counter Mode
#define PWM_X_CTL_ENABLE        0x00000001  // PWM Block Enable

//zero_offset
#define PWM_ZERO_OFFSET        0x00000000

//offsets of gen in pwm module
#define PWM_GEN_0_OFFSET        0x00000040  // PWM0 base
#define PWM_GEN_1_OFFSET        0x00000080  // PWM1 base
#define PWM_GEN_2_OFFSET        0x000000C0  // PWM2 base
#define PWM_GEN_3_OFFSET        0x00000100  // PWM3 base

//bit fields valuse in PWM_O_X_GENA

//Action for Counter=0
#define PWM_X_GENA_ACTZERO_M    0x00000003  // Action for Counter=0

#define PWM_X_GENA_ACTZERO_NONE 0x00000000  // Do nothing
#define PWM_X_GENA_ACTZERO_INV  0x00000001  // Invert pwmA
#define PWM_X_GENA_ACTZERO_ZERO 0x00000002  // Drive pwmA Low
#define PWM_X_GENA_ACTZERO_ONE  0x00000003  // Drive pwmA High

//Action for Counter=LOAD
#define PWM_X_GENA_ACTLOAD_M    0x0000000C  // Action for Counter=LOAD

#define PWM_X_GENA_ACTLOAD_NONE 0x00000000  // Do nothing
#define PWM_X_GENA_ACTLOAD_INV  0x00000004  // Invert pwmA
#define PWM_X_GENA_ACTLOAD_ZERO 0x00000008  // Drive pwmA Low
#define PWM_X_GENA_ACTLOAD_ONE  0x0000000C  // Drive pwmA High

//Action for Comparator A Up
#define PWM_X_GENA_ACTCMPAU_M   0x00000030  // Action for Comparator A Up

#define PWM_X_GENA_ACTCMPAU_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENA_ACTCMPAU_INV 0x00000010  // Invert pwmA
#define PWM_X_GENA_ACTCMPAU_ZERO                                              \
                                0x00000020  // Drive pwmA Low
#define PWM_X_GENA_ACTCMPAU_ONE 0x00000030  // Drive pwmA High

//Action for Comparator A Down
#define PWM_X_GENA_ACTCMPAD_M   0x000000C0  // Action for Comparator A Down

#define PWM_X_GENA_ACTCMPAD_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENA_ACTCMPAD_INV 0x00000040  // Invert pwmA
#define PWM_X_GENA_ACTCMPAD_ZERO                                              \
                                0x00000080  // Drive pwmA Low
#define PWM_X_GENA_ACTCMPAD_ONE 0x000000C0  // Drive pwmA High

//Action for Comparator B Up
#define PWM_X_GENA_ACTCMPBU_M   0x00000300  // Action for Comparator B Up

#define PWM_X_GENA_ACTCMPBU_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENA_ACTCMPBU_INV 0x00000100  // Invert pwmA
#define PWM_X_GENA_ACTCMPBU_ZERO                                              \
                                0x00000200  // Drive pwmA Low
#define PWM_X_GENA_ACTCMPBU_ONE 0x00000300  // Drive pwmA High

//Action for Comparator B Down
#define PWM_X_GENA_ACTCMPBD_M   0x00000C00  // Action for Comparator B Down

#define PWM_X_GENA_ACTCMPBD_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENA_ACTCMPBD_INV 0x00000400  // Invert pwmA
#define PWM_X_GENA_ACTCMPBD_ZERO                                              \
                                0x00000800  // Drive pwmA Low
#define PWM_X_GENA_ACTCMPBD_ONE 0x00000C00  // Drive pwmA High

//bit fields valuse in PWM_O_X_GENB

//Action for Counter=0
#define PWM_X_GENB_ACTZERO_M    0x00000003  // Action for Counter=0

#define PWM_X_GENB_ACTZERO_NONE 0x00000000  // Do nothing
#define PWM_X_GENB_ACTZERO_INV  0x00000001  // Invert pwmB
#define PWM_X_GENB_ACTZERO_ZERO 0x00000002  // Drive pwmB Low
#define PWM_X_GENB_ACTZERO_ONE  0x00000003  // Drive pwmB High

//Action for Counter=LOAD
#define PWM_X_GENB_ACTLOAD_M    0x0000000C  // Action for Counter=LOAD

#define PWM_X_GENB_ACTLOAD_NONE 0x00000000  // Do nothing
#define PWM_X_GENB_ACTLOAD_INV  0x00000004  // Invert pwmB
#define PWM_X_GENB_ACTLOAD_ZERO 0x00000008  // Drive pwmB Low
#define PWM_X_GENB_ACTLOAD_ONE  0x0000000C  // Drive pwmB High

//Action for Comparator A Up
#define PWM_X_GENB_ACTCMPAU_M   0x00000030  // Action for Comparator A Up

#define PWM_X_GENB_ACTCMPAU_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENB_ACTCMPAU_INV 0x00000010  // Invert pwmB
#define PWM_X_GENB_ACTCMPAU_ZERO                                              \
                                0x00000020  // Drive pwmB Low
#define PWM_X_GENB_ACTCMPAU_ONE 0x00000030  // Drive pwmB High

//Action for Comparator A Down
#define PWM_X_GENB_ACTCMPAD_M   0x000000C0  // Action for Comparator A Down

#define PWM_X_GENB_ACTCMPAD_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENB_ACTCMPAD_INV 0x00000040  // Invert pwmB
#define PWM_X_GENB_ACTCMPAD_ZERO                                              \
                                0x00000080  // Drive pwmB Low
#define PWM_X_GENB_ACTCMPAD_ONE 0x000000C0  // Drive pwmB High

//Action for Comparator B Up
#define PWM_X_GENB_ACTCMPBU_M   0x00000300  // Action for Comparator B Up

#define PWM_X_GENB_ACTCMPBU_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENB_ACTCMPBU_INV 0x00000100  // Invert pwmB
#define PWM_X_GENB_ACTCMPBU_ZERO                                              \
                                0x00000200  // Drive pwmB Low
#define PWM_X_GENB_ACTCMPBU_ONE 0x00000300  // Drive pwmB High

//Action for Comparator B Down
#define PWM_X_GENB_ACTCMPBD_M   0x00000C00  // Action for Comparator B Down

#define PWM_X_GENB_ACTCMPBD_NONE                                              \
                                0x00000000  // Do nothing
#define PWM_X_GENB_ACTCMPBD_INV 0x00000400  // Invert pwmB
#define PWM_X_GENB_ACTCMPBD_ZERO                                              \
                                0x00000800  // Drive pwmB Low
#define PWM_X_GENB_ACTCMPBD_ONE 0x00000C00  // Drive pwmB High


//AUTOSAR APIs

void Pwm_Init ( const Pwm_ConfigType* ConfigPtr );//<==
void Pwm_DeInit ( void );
void Pwm_SetDutyCycle ( Pwm_ChannelType ChannelNumber, uint16_t DutyCycle );//<==
void Pwm_SetPeriodAndDuty ( Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16_t DutyCycle );
void Pwm_SetOutputToIdle ( Pwm_ChannelType ChannelNumber );
Pwm_OutputStateType Pwm_GetOutputState (Pwm_ChannelType ChannelNumber);



//Helper functions
uint32_t __PwmModuleBase(PWM_MOD pwm_module);
uint32_t __PwmGenOffset(PWM_GEN pwm_gen);
void __PwmClkEnable(PWM_MOD pwm_module,PWM_GEN pwm_gen,clkDiv prescaler);
void __PwmClkDisable(PWM_MOD pwm_module);
void __PwmCounterConfig(PWM_MOD pwm_module,PWM_GEN pwm_gen ,PWM_GeneratorMode mode ,Pwm_Polarity polarity,Pwm_DutyCycle dutycycle);
void __PwmPinConfig(Pwm_ChannelType channelId,PWM_GeneratorMode mode ,Pwm_Polarity polarity,Pwm_DutyCycle dutycycle);
void __PwmGenState(PWM_MOD pwm_module,PWM_GEN pwm_gen,GenState state);
void __PwmOutputState(Pwm_ChannelType PwmChannelId,OutputState state);
void __PwmGenPeriodSet(PWM_MOD pwm_module,PWM_GEN pwm_gen,Pwm_PeriodType period);
Pwm_PeriodType __PWMGenPeriodGet(PWM_MOD pwm_module,PWM_GEN pwm_gen);
void __PwmPulseWidthSet(Pwm_ChannelType PwmChannelId,PWM_GeneratorMode mode,uint16_t width);
void __PwmOutputHigh(Pwm_ChannelType PwmChannelId,PWM_GeneratorMode mode);
void __PwmOutputLow(Pwm_ChannelType channelId,PWM_GeneratorMode mode);
#endif /* PWM_H_ */
