/*
 * Pwm.c
 *
 *  Created on: Jul 23, 2020
 *      Author: moh1969
 */
#include "Pwm.h"

uint8_t init_status[16]={uninitialized};
Pwm_ConfigType * configptr[16];


uint32_t __PwmModuleBase(PWM_MOD pwm_module)
{
    if(pwm_module==PWM_MOD0) return PWM0_BASE;
    else return PWM1_BASE;
}

uint32_t __PwmGenOffset(PWM_GEN pwm_gen)
{
    if(pwm_gen==0) return PWM_GEN_0_OFFSET;
    else if(pwm_gen==1) return PWM_GEN_1_OFFSET;
    else if(pwm_gen==2) return PWM_GEN_2_OFFSET;
    else  return PWM_GEN_3_OFFSET;

}

void __PwmClkEnable(PWM_MOD pwm_module,PWM_GEN pwm_gen,clkDiv prescaler)
{
    SYSCTL_RCGCPWM_R|= (1<<pwm_module);

    if(prescaler==none){SYSCTL_RCC_R &=~(SYSCTL_RCC_USEPWMDIV);}
    else{
        SYSCTL_RCC_R|=SYSCTL_RCC_USEPWMDIV;
        SYSCTL_RCC_R|=prescaler<<17;
    }

}
void __PwmClkDisable(PWM_MOD pwm_module)
{
    SYSCTL_RCGCPWM_R &= ~(1<<pwm_module);
}

// helper functions ....

/*
 * input : module number [PWM_MOD0,PWM_MOD1],generator number in this module [PWM_GEN0,PWM_GEN1,PWM_GEN2,PWM_GEN3]and mode[Up_Down_counterMode,Down_counterMode]
 * function responsible for setting mode and the shape of pwm independent of which pin because
 * there is 1 generator in module and both generator pins must have same mode and therefore shape
 */

void __PwmCounterConfig(PWM_MOD pwm_module,PWM_GEN pwm_gen ,PWM_GeneratorMode mode ,Pwm_Polarity polarity,Pwm_DutyCycle dutycycle){

    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
    uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);

    pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)&=~((PWM_X_GENA_ACTZERO_M)|(PWM_X_GENA_ACTLOAD_M)|(PWM_X_GENA_ACTCMPAU_M)|(PWM_X_GENA_ACTCMPAD_M)|(PWM_X_GENA_ACTCMPBU_M)|(PWM_X_GENA_ACTCMPBD_M));
    pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)&=~((PWM_X_GENB_ACTZERO_M)|(PWM_X_GENB_ACTLOAD_M)|(PWM_X_GENB_ACTCMPAU_M)|(PWM_X_GENB_ACTCMPAD_M)|(PWM_X_GENB_ACTCMPBU_M)|(PWM_X_GENB_ACTCMPBD_M));
    if(mode==Up_Down_counterMode){
        pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)|=(mode<<1);

        if(polarity==PWM_polarity_pos){
            if(dutycycle==DutyCycle_0){
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTZERO_ZERO);
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTZERO_ZERO);}
            else if(dutycycle==DutyCycle_100){
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTZERO_ONE);
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTZERO_ONE);}
            else{
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=((PWM_X_GENA_ACTCMPAU_ONE)|(PWM_X_GENA_ACTCMPAD_ZERO));
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=((PWM_X_GENB_ACTCMPBU_ONE)|(PWM_X_GENB_ACTCMPBD_ZERO));}
        }

        else if(polarity==PWM_polarity_neg){
            if(dutycycle==DutyCycle_0){
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTZERO_ONE);
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTZERO_ONE);}
            else if(dutycycle==DutyCycle_100){
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTZERO_ZERO);
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTZERO_ZERO);}
            else{
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=((PWM_X_GENA_ACTCMPAU_ZERO)|(PWM_X_GENA_ACTCMPAD_ONE));
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=((PWM_X_GENB_ACTCMPBU_ZERO)|(PWM_X_GENB_ACTCMPBD_ONE));}
        }

    }
    else if(mode==Down_counterMode){
        pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)&=~(mode<<1);

        if(polarity==PWM_polarity_pos){
            if(dutycycle==DutyCycle_0){
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTLOAD_ZERO);
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTLOAD_ZERO);}
            else if(dutycycle==DutyCycle_100){
                 pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTLOAD_ONE);
                 pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTLOAD_ONE);}
            else{
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=((PWM_X_GENA_ACTLOAD_ONE)|(PWM_X_GENA_ACTCMPAD_ZERO));
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=((PWM_X_GENB_ACTLOAD_ONE)|(PWM_X_GENB_ACTCMPBD_ZERO));}
        }
        else if(polarity==PWM_polarity_neg){
            if(dutycycle==DutyCycle_0){
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTLOAD_ONE);
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTLOAD_ONE);}
            else if(dutycycle==DutyCycle_100){
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=(PWM_X_GENA_ACTLOAD_ZERO);
                pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=(PWM_X_GENB_ACTLOAD_ZERO);}
            else{
            pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENA)|=((PWM_X_GENA_ACTLOAD_ZERO)|(PWM_X_GENA_ACTCMPAD_ONE));
            pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_GENB)|=((PWM_X_GENB_ACTLOAD_ZERO)|(PWM_X_GENB_ACTCMPBD_ONE));}
        }
    }
}

void __PwmPinConfig(Pwm_ChannelType channelId,PWM_GeneratorMode mode ,Pwm_Polarity polarity,Pwm_DutyCycle dutycycle){

        PWM_MOD pwm_module=(channelId/8); //determine whether module 0 or 1
        PWM_GEN pwm_gen =(channelId/2);    //determine whether gen 0,1,2,3
        uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
        uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);
        uint32_t pwm_GenX_offset;

        if(channelId%2==0)//pwmA
            pwm_GenX_offset=PWM_O_X_GENA;
        else //pwmB
            pwm_GenX_offset=PWM_O_X_GENB;

        pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)&=~((PWM_X_GENA_ACTZERO_M)|(PWM_X_GENA_ACTLOAD_M)|(PWM_X_GENA_ACTCMPAU_M)|(PWM_X_GENA_ACTCMPAD_M)|(PWM_X_GENA_ACTCMPBU_M)|(PWM_X_GENA_ACTCMPBD_M));

        if(mode==Up_Down_counterMode){
            pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)|=(mode<<1);

            if(polarity==PWM_polarity_pos){
                if(dutycycle==DutyCycle_0)
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTZERO_ZERO);
                else if(dutycycle==DutyCycle_100)
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTZERO_ONE);
                else
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=((PWM_X_GENA_ACTCMPAU_ONE)|(PWM_X_GENA_ACTCMPAD_ZERO));
            }
            else if(polarity==PWM_polarity_neg){
                if(dutycycle==DutyCycle_0)
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTZERO_ONE);
                else if(dutycycle==DutyCycle_100)
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTZERO_ZERO);
                else
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=((PWM_X_GENA_ACTCMPAU_ZERO)|(PWM_X_GENA_ACTCMPAD_ONE));
            }
        }
        else if(mode==Down_counterMode){
            pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)&=~(mode<<1);

            if(polarity==PWM_polarity_pos){
                if(dutycycle==DutyCycle_0)
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTLOAD_ZERO);
                else if(dutycycle==DutyCycle_100)
                     pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTLOAD_ONE);
                else
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=((PWM_X_GENA_ACTLOAD_ONE)|(PWM_X_GENA_ACTCMPAD_ZERO));
            }
            else if(polarity==PWM_polarity_neg){
                if(dutycycle==DutyCycle_0)
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTLOAD_ONE);
                else if(dutycycle==DutyCycle_100)
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=(PWM_X_GENA_ACTLOAD_ZERO);
                else
                    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=((PWM_X_GENA_ACTLOAD_ZERO)|(PWM_X_GENA_ACTCMPAD_ONE));
            }
        }
}

/* enable counter/generator in certain module (counter starts counting)*/
void __PwmGenState(PWM_MOD pwm_module,PWM_GEN pwm_gen,GenState state){
    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
    uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);
    pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)&=~(PWM_X_CTL_ENABLE);
    pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)|=state;
}

/* enable pwm channel output (pwm signal is now ouput on pin) */
void __PwmOutputState(Pwm_ChannelType PwmChannelId,OutputState state){

    PWM_MOD pwm_module=(PwmChannelId/8);  //determine whether module 0 or 1
    uint8_t PwmPin_No=(PwmChannelId%8);   //its no. in module 0 ---7

    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);

    pwm_reg(pwm_mod_base,PWM_ZERO_OFFSET,PWM_O_ENABLE)|=(state<<PwmPin_No);

}

// load reg =freq clk/freq pwm =N ticks and the input parameter period =N ticks but actual period of pwm =N ticks*clk period
void __PwmPeriodSet(PWM_MOD pwm_module,PWM_GEN pwm_gen,Pwm_PeriodType period){
    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
    uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);
    PWM_GeneratorMode mode=(PWM_GeneratorMode)((pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)&PWM_X_CTL_MODE)>>1);
    if(mode == Up_Down_counterMode){
        pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_LOAD)=period/2;
    }
    else if(mode==Down_counterMode){
        pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_LOAD)=period-1; //period -1

    }

}
Pwm_PeriodType __PWMGenPeriodGet(PWM_MOD pwm_module,PWM_GEN pwm_gen){
    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
    uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);
    PWM_GeneratorMode mode=(PWM_GeneratorMode)((pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CTL)&PWM_X_CTL_MODE)>>1);
    if(mode ==Up_Down_counterMode ){
            return pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_LOAD)*2;
     }
     else if(mode==Down_counterMode){
            return pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_LOAD)+1; //period -1

        }
}
void __PwmPulseWidth(Pwm_ChannelType PwmChannelId,PWM_GeneratorMode mode,uint16_t width){
    if(mode==Up_Down_counterMode) width/=2;
    PWM_MOD pwm_module=(PwmChannelId/8); //determine whether module 0 or 1
    PWM_GEN pwm_gen =(PwmChannelId/2);    //determine whether gen 0,1,2,3

    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
    uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);
    uint32_t cmp=pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_LOAD)-width;

    if(PwmChannelId%2==0){
        //pwmA
        pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CMPA)=cmp;
    }
    else{
        //pwmB
        pwm_reg(pwm_mod_base,pwm_gen_offset,PWM_O_X_CMPB)=cmp;
    }


}


void __PwmOutputHigh(Pwm_ChannelType channelId,PWM_GeneratorMode mode){
    PWM_MOD pwm_module=(channelId/8); //determine whether module 0 or 1
    PWM_GEN pwm_gen =(channelId/2);    //determine whether gen 0,1,2,3

    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
    uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);
    uint32_t pwm_GenX_offset;

    if(channelId%2==0)//pwmA
        pwm_GenX_offset=PWM_O_X_GENA;

    else//pwmB
        pwm_GenX_offset=PWM_O_X_GENB;

    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)&=~((PWM_X_GENA_ACTZERO_M)|(PWM_X_GENA_ACTLOAD_M)|(PWM_X_GENA_ACTCMPAU_M)|(PWM_X_GENA_ACTCMPAD_M)|(PWM_X_GENA_ACTCMPBU_M)|(PWM_X_GENA_ACTCMPBD_M));

    if(mode==Up_Down_counterMode)
        pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=PWM_X_GENA_ACTZERO_ONE;
    else if(mode==Down_counterMode)
        pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=PWM_X_GENA_ACTLOAD_ONE;
}

void __PwmOutputLow(Pwm_ChannelType channelId,PWM_GeneratorMode mode){
    PWM_MOD pwm_module=(channelId/8); //determine whether module 0 or 1
    PWM_GEN pwm_gen =(channelId/2);    //determine whether gen 0,1,2,3

    uint32_t pwm_mod_base = __PwmModuleBase(pwm_module);
    uint32_t pwm_gen_offset=__PwmGenOffset(pwm_gen);
    uint32_t pwm_GenX_offset;

    if(channelId%2==0)//pwmA
        pwm_GenX_offset=PWM_O_X_GENA;

    else//pwmB
        pwm_GenX_offset=PWM_O_X_GENB;

    pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)&=~((PWM_X_GENA_ACTZERO_M)|(PWM_X_GENA_ACTLOAD_M)|(PWM_X_GENA_ACTCMPAU_M)|(PWM_X_GENA_ACTCMPAD_M)|(PWM_X_GENA_ACTCMPBU_M)|(PWM_X_GENA_ACTCMPBD_M));

    if(mode==Up_Down_counterMode)
        pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=PWM_X_GENA_ACTZERO_ZERO;
    else if(mode==Down_counterMode)
        pwm_reg(pwm_mod_base,pwm_gen_offset,pwm_GenX_offset)|=PWM_X_GENA_ACTLOAD_ZERO;
}

void __CopyConfig(const Pwm_ConfigType* ConfigPtr )
{
    Pwm_ChannelType channelId=ConfigPtr->PwmChannelId;
    configptr[channelId]->PwmChannelId=ConfigPtr->PwmChannelId;
    configptr[channelId]->period=ConfigPtr->period;
    configptr[channelId]->dutyCycle=ConfigPtr->dutyCycle;
    configptr[channelId]->polarity=ConfigPtr->polarity;
    configptr[channelId]->idleState=ConfigPtr->idleState;
    configptr[channelId]->channelClass=ConfigPtr->channelClass;
    configptr[channelId]->genMode=ConfigPtr->genMode;
    configptr[channelId]->SysClkDiv=ConfigPtr->SysClkDiv;
}
//AUTOSAR APIs

void Pwm_Init ( const Pwm_ConfigType* ConfigPtr ){

    Pwm_ChannelType channelId=ConfigPtr->PwmChannelId;
    Pwm_DutyCycle relative_dutycycle=ConfigPtr->dutyCycle;
    PWM_MOD pwm_module=(channelId/8); //determine whether module 0 or 1
    PWM_GEN pwm_gen =(channelId/2);    //determine whether gen 0,1,2,3

    // re-initialization must be preceded by de-initialization
    if(init_status[channelId]==initialized) Pwm_DeInit();

    //PWM channel is initialized
    init_status[channelId]=initialized;

    //copy the struct to local struct
    __CopyConfig(ConfigPtr);

    //clock configrations
    __PwmClkEnable(pwm_module, pwm_gen, ConfigPtr->SysClkDiv);

    //Genertor/counter disable(not counting)
    __PwmGenState(pwm_module, pwm_gen,Gen_disable);

    //pin configurations
    __PwmPinConfig(ConfigPtr->PwmChannelId, ConfigPtr->genMode,ConfigPtr->polarity,relative_dutycycle);

    //period and duty cycle
    if(relative_dutycycle!=DutyCycle_0 && relative_dutycycle!=DutyCycle_100){
        __PwmGenPeriodSet(pwm_module, pwm_gen, ConfigPtr->period);
        // width = period(N ticks) *(absolute dutycycle) - absolute dutycycle= relative dutycycle>>15; 0x8000 =100% =1
        uint16_t width=(ConfigPtr->period)*(relative_dutycycle)>>15;
        __PwmPulseWidthSet(channelId, ConfigPtr->genMode, width);
    }

    //enable output pin

}

void Pwm_DeInit ( void ){
#ifdef PwmDeInitApi
    Pwm_ChannelType channelId=0;
    for(;channelId<16;channelId++)
    {
        init_status[channelId]=uninitialized;
        Pwm_SetOutputToIdle(channelId);
    }

#endif

}



void Pwm_SetDutyCycle ( Pwm_ChannelType ChannelNumber, uint16_t DutyCycle ){
#ifdef PwmSetDutyCycle
    configptr[ChannelNumber]->dutyCycle=(Pwm_DutyCycle)DutyCycle;
    Pwm_Polarity polarity=configptr[ChannelNumber]->polarity;
    PWM_GeneratorMode mode=configptr[ChannelNumber]->genMode;
    uint16_t width;

    if(DutyCycle==DutyCycle_0){
        if(polarity==PWM_polarity_pos) __PwmOutputLow(ChannelNumber, mode);
        else __PwmOutputHigh(ChannelNumber, mode); }

    else if(DutyCycle==DutyCycle_100){
        if(polarity==PWM_polarity_pos) __PwmOutputHigh(ChannelNumber, mode);
        else __PwmOutputLow(ChannelNumber, mode); }

    else{
        width=(configptr[ChannelNumber]->period)*(DutyCycle)>>15;
        __PwmPulseWidthSet(ChannelNumber, mode, width);

    }
#endif

}
void Pwm_SetPeriodAndDuty ( Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16_t DutyCycle ){
#ifdef PwmSetPeriodAndDuty
    if(configptr[ChannelNumber]->channelClass==PWM_VARIABLE_PERIOD)
    {

    }
#endif
}

void Pwm_SetOutputToIdle ( Pwm_ChannelType ChannelNumber ){
#ifdef PwmSetOutputToIdle
    PwmIdleState idleState=configptr[ChannelNumber]->idleState;
    PWM_GeneratorMode mode=configptr[ChannelNumber]->genMode;
    if(idleState==pwm_idle_low) __PwmOutputLow(ChannelNumber,mode);
    else if(idleState==pwm_idle_high) __PwmOutputLow(ChannelNumber,mode);
    if(configptr[ChannelNumber]->channelClass==PWM_VARIABLE_PERIOD){

    }
    else{

    }
#endif
}
