#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

int main()
{
    //
    //  Initialise the peripheral clock.
    //
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOD;
    //
    //  Initilaise the GPIO port.
    //
    GPIOD->MODER |= GPIO_Mode_OUT;
    GPIOD->OSPEEDR |= GPIO_Speed_25MHz;
    GPIOD->OTYPER |= GPIO_OType_PP;
    GPIOD->PUPDR |= GPIO_PuPd_NOPULL;
    //
    //  Toggle Port D, pin 0 indefinitely.
    //
    while (1)
    {
        GPIOD->BSRRL = GPIO_Pin_0;
        GPIOD->BSRRH = GPIO_Pin_0;
    }
}