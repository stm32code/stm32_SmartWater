#ifndef __RELAY_H
#define __RELAY_H

#include "stm32f10x.h"

#define BEEP_GPIO_PORT GPIOA
#define BEEP_GPIO_CLK RCC_APB2Periph_GPIOA
#define BEEP_GPIO_PIN GPIO_Pin_2

#define RELAY1_GPIO_PORT GPIOB
#define RELAY1_GPIO_CLK RCC_APB2Periph_GPIOB
#define RELAY1_GPIO_PIN GPIO_Pin_6

#define RELAY2_GPIO_PORT GPIOB
#define RELAY2_GPIO_CLK RCC_APB2Periph_GPIOB
#define RELAY2_GPIO_PIN GPIO_Pin_9

#define relay1in PBin(6)
#define relay2in PBin(5)

#define relay1_ON digitalHi(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN)
#define relay1_OFF digitalLo(RELAY1_GPIO_PORT, RELAY1_GPIO_PIN)

#define relay1out PBout(6) // LED接口
#define relay2out PBout(5) // LED接口

void RELAY_GPIO_Config(void);

#endif /* __RELAY_H */
