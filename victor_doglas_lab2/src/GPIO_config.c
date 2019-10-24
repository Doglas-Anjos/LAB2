#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "system_TM4C1294.h" 
#include "tm4c1294ncpdt_pt2.h"
uint32_t up=0;
uint32_t down=0;
uint32_t ctl=0;

void EnableInterrupts(void);
void DisableInterrupts(void);

#define ONE_SECOND						(((volatile uint32_t )	0x04C4B400))
#define GPIO_PORTD 	    	                                ((volatile uint32_t) 0x00000008)

void GPIO_Init(void)
{
        int32_t GPIO_ALL=0x00000000;		//ZERA A VARIAVEL PARA INICIALIZAÇÃO
        
        GPIO_ALL=GPIO_PORTD;

        //1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R |= GPIO_ALL;
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
        while((SYSCTL_PRGPIO_R & (GPIO_ALL) ) != (GPIO_ALL) ){};
        
        // 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTD_AHB_AMSEL_R = 0x00;
        
        // 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTD_AHB_PCTL_R = 0x303;           //selecionando o timer no gpio t0ccp0 t1ccp0 pd0 e pd2
        
        // 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTD_AHB_DIR_R = 0x00;
        
        // 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTD_AHB_AFSEL_R = 0x05; //Seleciona PD0 e PD2
        
        // 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTD_AHB_DEN_R = 0x05;   //Bit0 e bit1
        
        //receita de bolo
        RCGCTIMER=0x000000FF;
	while (0x000000FF!=PRTIMER)
	{}
        
        //1. desabilitando os timers antes das configurações
        GPTMCTL_TIMER_0=0x00;
	GPTMCTL_TIMER_1=0x00;
	GPTMCTL_TIMER_2=0x00;
        //2. confiugrando o tamanho do timer
        GPTMCFG_TIMER_0=0x4;		// SETANDO O CONTADOR PARA 16BITS
	GPTMCFG_TIMER_1=0x4;
	GPTMCFG_TIMER_2=0x0;		// SETANDO O CONTADOR PARA 32BITS
        
        //3.configurando para edgemode
        GPTMTAMR_TIMER_0=0x4+0x3+0x10;  //	TnCMR =0x1; TnMR=0x3; TnCDIR=0X1 cont up	
	GPTMTAMR_TIMER_1=0x4+0x3+0x10;
	GPTMTAMR_TIMER_2=0x1;		// CONFIGURANDO PARA ONE SHOOT
        
        //4.configurando CTL taevent
        
        GPTMCTL_TIMER_0=0x00;           //positive edge
	GPTMCTL_TIMER_1=0x04;           //negative edge
        
        //5. no prescaler
        
        //6.
        GPTMTAILR_TIMER_0=0x0000;
	GPTMTAILR_TIMER_1=0x0000;
        
        //7. interrupt
        GPTMICR_TIMER_0=0x4;            //setando CaEIM
	GPTMICR_TIMER_1=0x4;            //setando CaEIM
	GPTMICR_TIMER_2=0x1;
        
        GPTMCTL_TIMER_0|=0x01;           
	GPTMCTL_TIMER_1|=0x01;
        GPTMICR_TIMER_0=0x1;
        GPTMICR_TIMER_1=0x1;
        GPTMTAILR_TIMER_2=20*ONE_SECOND;
        GPTMICR_TIMER_2=0x1;
        GPTMIMR_TIMER_2=0x1;
        
        NVIC_PRI4=0x20000000;
        NVIC_PRI5=0x20002000;
        NVIC_PRI8=0x40000000; // TIMER 0 
        NVIC_PRI9=0x40000040; // TIMER 1 E 2
        
        NVIC_EN0=0x00A80000;
        NVIC_EN1=0x000000A8; //timer 0,1 e 2
        
        EnableInterrupts();
}

void TIMER0A_Handler(void)
{GPTMICR_TIMER_0=0x1;
down=GPTMTAILR_TIMER_0;
ctl=1;}

void TIMER1A_Handler(void)
{GPTMICR_TIMER_1=0x1;
up=GPTMTAILR_TIMER_1;
}
void TIMER2A_Handler(void)
{GPTMICR_TIMER_2=0x1;}

void ON_TIMER_0(void)
{GPTMCTL_TIMER_0|=0x1;}

void ON_TIMER_1(void)
{GPTMCTL_TIMER_1|=0x1;}

void OFF_TIMER_0(void)
{GPTMCTL_TIMER_0&=0xFFFE;}

void OFF_TIMER_1(void)
{GPTMCTL_TIMER_1&=0xFFFE;}
