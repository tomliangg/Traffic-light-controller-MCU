
/* Include core modules */
#include "stm32f4xx.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_exti.h"
#include "tm_stm32f4_delay.h"

volatile uint8_t Activate_NS = 0;
volatile uint8_t Activate_NS_Assist = 0;
volatile uint8_t Activate_EW= 0;
volatile uint8_t counter_NS_pedestrian = 0;
volatile uint32_t AddTime_NS_temp = 0;
volatile uint32_t AddTime_EW_temp = 0;
volatile uint32_t AddTime_NS = 0;
volatile uint32_t AddTime_EW = 0;

GPIO_InitTypeDef  GPIO_InitStructure;


int main(void) {

	/* Initialize system */
	SystemInit();
	TM_DELAY_Init();
	/* Initialize LEDS */
	TM_DISCO_LedInit();
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Attach interrupt on pin PC1 = External line 1      PC1 is used for EW pedestrian button*/   
	if (TM_EXTI_Attach(GPIOC, GPIO_Pin_1, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
			//AddTime_NS_temp += 0x01F4;      //if the NS car button pressed, add 500ms   01F4 hex = 500 decimal
	}
	
	/* Attach interrupt on pin PB2 = External line 2      PB2 is used for EW pedestrian button*/   
	if (TM_EXTI_Attach(GPIOB, GPIO_Pin_2, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
			//AddTime_EW_temp += 0x01F4;      //if the EW car button pressed, add 500ms   01F4 hex = 500 decimal
	}
	
	/* Attach interrupt on pin PE3 = External line 3      PE3 is used for NS pedestrian button*/
	if (TM_EXTI_Attach(GPIOE, GPIO_Pin_3, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
		// This pin is used for pedestrian cross
	}
	
	/* Attach interrupt on pin PE4 = External line 4      PE4 is used for Reset button*/
	if (TM_EXTI_Attach(GPIOE, GPIO_Pin_4, TM_EXTI_Trigger_Rising) == TM_EXTI_Result_Ok) {
	}
	
	while (1) {
			/* State 1 NS Red (PD7) ON at least 6s, EW Green ON (PD15) at least 6s */
		if (Activate_EW == 1)
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_3 | GPIO_Pin_11);
			GPIO_SetBits(GPIOD, GPIO_Pin_7 | GPIO_Pin_15 | GPIO_Pin_2);
			/* Insert delay */
			Delayms(0x1770+AddTime_EW);
			AddTime_EW = AddTime_EW_temp; //assign the value to AddTime_EW
			AddTime_EW_temp = 0x00;       //reset the temp AddTime after assigning the value to AddTime_EW
			
			/* State 2 NS Red (PD7) ON 3s, EW Yellow ON (PD14) 3s */
			GPIO_ResetBits(GPIOD, GPIO_Pin_15);
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
			/* Insert delay */
			Delayms(3000);
			Activate_EW = 0; //reset it to 0
		}
			/* State 3 NS Red (PD7) ON 2s, EW Red ON (PD11) 2s */
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
			GPIO_SetBits(GPIOD, GPIO_Pin_11 | GPIO_Pin_7);
			/* Insert delay */
			Delayms(2000);	
				
			/* State 4 NS Green (PD3) ON at least 6s, EW Red ON (PD11) at least 6s */
			if (counter_NS_pedestrian >0 )   //if someone press the pedestrian cross button
			{
				GPIO_SetBits(GPIOD, GPIO_Pin_0);    //turn on Green pedestrian light
				GPIO_ResetBits(GPIOD, GPIO_Pin_2); //turn off the Red pedestrian light
				if (Activate_NS > 0)
				{
					counter_NS_pedestrian = 0;
				}
			}
			
			if (Activate_NS == 1)
			{
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
			GPIO_SetBits(GPIOD, GPIO_Pin_3);
			/* Insert delay */
			Delayms(0x1770+AddTime_NS);	
			AddTime_NS = AddTime_NS_temp; //assign the value to AddTime_NS
			AddTime_NS_temp = 0;       //reset the temp AddTime after assigning the value to AddTime_NS

			/* State 5 NS Yellow (PD6) ON 3s, EW Red ON (PD11) 3s */
			GPIO_ResetBits(GPIOD, GPIO_Pin_3 | GPIO_Pin_0);
			GPIO_SetBits(GPIOD, GPIO_Pin_6 | GPIO_Pin_2);
			/* Insert delay */
			Delayms(3000);
			Activate_NS = 0;
			}				
			
			if (counter_NS_pedestrian > 0 && Activate_NS_Assist == 0)
			{
				Delayms(6000);
				counter_NS_pedestrian=0;   //reset this value
			}	
			
			Activate_NS_Assist = 0;
			/* State 6 NS Red (PD7) ON 2s, EW Red ON (PD11) 2s */
			GPIO_ResetBits(GPIOD, GPIO_Pin_6 | GPIO_Pin_0);
			GPIO_SetBits(GPIOD, GPIO_Pin_7 | GPIO_Pin_2);
			/* Insert delay */
			Delayms(2000);	
		}
}

void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	/* Handle external line 1 interrupts */
	if (GPIO_Pin == GPIO_Pin_1) {
		if (Activate_NS == 0)
		{
				Activate_NS += 1;
			  Activate_NS_Assist += 1;
		}
		else if (Activate_NS == 1)
		{
		AddTime_NS_temp += 0xfa;      //if the NS car button pressed, add 500ms   01F4 hex = 500 decimal
		Delayms(10);
		}
	}
	
	if (GPIO_Pin == GPIO_Pin_2) {
		if (Activate_EW == 0)
		{
				Activate_EW += 1;
		}
		else if (Activate_EW == 1)
		{
		AddTime_EW_temp += 0xfa;      //if the EW car button pressed, add 500ms   01F4 hex = 500 decimal
		Delayms(10);
		}
	}
	
	/* Handle external line 3 interrupts */
	if (GPIO_Pin == GPIO_Pin_3) {
		counter_NS_pedestrian++;
	}
	
	if (GPIO_Pin == GPIO_Pin_4) {
	NVIC_SystemReset();    //system reset
	}
}

