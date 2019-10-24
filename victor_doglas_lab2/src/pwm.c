// Victor Hugo S. dos Reis e Doglas Querino
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
#include "system_TM4C1294.h" 

extern uint32_t up;
extern uint32_t down;
extern uint32_t clt;
uint32_t Freq_func=120000000;
uint8_t LED_D1 = 0;
//uint8_t freq = 0;
//uint8_t pulse_width = 0;
//uint8_t periodo = 0;
//uint8_t duty_cycle = 0;
//uint8_t aux = 0;
//uint16_t cont = 0;

static uint32_t ctl=0;
static uint32_t up=0;
static uint32_t down=0;
static float freq=0.;
static float pulse_width = 0.;
static float periodo = 0.;
static float duty_cycle = 0;
static int aux = 0;
static uint32_t cont = 0;

static char sfreq[20];
static char speriodo[20];
static char sduty_cycle[20];
void GPIO_Init(void);
void GPIO_Init(void);
void ON_TIMER_1(void);
void ON_TIMER_0(void);
void OFF_TIMER_1(void);
void OFF_TIMER_0(void);
void TIMER_0_A(void);
void TIMER_1_A(void);

// Configuração da UART *********

void UARTInit(void){
  // Enable the GPIO Peripheral used by the UART.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

  // Enable UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

  // Configure GPIO Pins for UART mode.
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // Initialize the UART for console I/O.
  UARTStdioConfig(0, 9600, SystemCoreClock);
} // UARTInit

void UART0_Handler(void){
  UARTStdioIntHandler();
} // UART0_Handler

// ******************************


/*
void SysTick_Handler(void){
  LED_D1 ^= GPIO_PIN_1; // Troca estado do LED D1
  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, LED_D1); // Acende ou apaga LED D1
} // SysTick_Handler
*/

//void SysTick_Handler(void){
//  LED_D1 ^= GPIO_PIN_1; // Troca estado do LED D1
//  GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, LED_D1); // Acende ou apaga LED D1
//} // SysTick_Handler

 void main(void)
 {
  
  UARTInit();
  UARTprintf("Teste\n");
  SysTickPeriodSet(Freq_func); // f = 1Hz para clock = 24MHz
  aux=1;
  GPIO_Init();
  // Configuração dos LEDs e push-buttons
  
  // ========================== port N=========================================
  //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); // Habilita GPIO N (LED D1 = PN1, LED D2 = PN0)
  //while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)); // Aguarda final da habilitação
  
  //GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1); // LEDs D1 e D2 como saída
  //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0); // LEDs D1 e D2 apagados
  //GPIOPadConfigSet(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
  
  // ========================== port F =========================================
  //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Habilita GPIO F (LED D3 = PF4, LED D4 = PF0)
  //while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)); // Aguarda final da habilitação
  
  //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4); // LEDs D3 e D4 como saída
  //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0); // LEDs D3 e D4 apagados
  //GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
  
  // ========================== port L =========================================
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL); // Habilita GPIO L 
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)); // Aguarda final da habilitação
  GPIOPinTypeGPIOInput(GPIO_PORTL_BASE, GPIO_PIN_4); // pin L4 como entrada
  GPIOPadConfigSet(GPIO_PORTL_BASE,GPIO_PIN_4, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0, push-button SW2 = PJ1)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)); // Aguarda final da habilitação
    
  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1); // push-buttons SW1 e SW2 como entrada
  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

  
  // ************************************

 // SysTickIntEnable();
//  SysTickEnable();
  
  //aux = GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4);
  //while(1){
  //  if(GPIOPinRead(GPIO_PORTL_BASE, GPIO_PIN_4) == aux) // Testa estado do pino do gerador até ser alto
  //    break;
  // }
  
  while(1){
    ON_TIMER_0();
    ON_TIMER_1();
    while(ctl==0)
    {cont++;}
    
    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0 && aux==1) // Testa estado o push-button SW2
    { 
         cont=up+down;
         Freq_func=SystemCoreClock;
         pulse_width=(float)up/Freq_func;
         
         periodo = (float)cont/(float)Freq_func;
         
         freq = ((float)Freq_func/(float)cont);
         duty_cycle = (pulse_width/periodo)*100;
         
         //sprintf(sfreq, "%f\n",freq);
         //UARTprintf("freq = %s\n",sfreq);
         //sprintf(speriodo, "%f\n",periodo);
         //UARTprintf("periodo = %s\n", speriodo);
         //sprintf(sduty_cycle, "%f\n",duty_cycle);
         //UARTprintf("duty_cycle = %s\n\n", sduty_cycle);
         aux=0;
    }
    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 1 )
      aux=1;

    //if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == GPIO_PIN_1) // Testa estado do push-button SW2
     // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0); // Apaga LED D4
    //else
    //  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0); // Acende LED D4
    
   
   
  } // while
} // main