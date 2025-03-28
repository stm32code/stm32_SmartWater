#ifndef __LEVEL_H
#define __LEVEL_H

#include "stm32f10x.h"

//  Òý½Å¶¨Òå
#define LEVEL1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LEVEL1_GPIO_PORT GPIOB
#define LEVEL1_GPIO_PIN GPIO_Pin_7

#define LEVEL2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LEVEL2_GPIO_PORT GPIOB
#define LEVEL2_GPIO_PIN GPIO_Pin_8

#define LEVEL3_GPIO_CLK RCC_APB2Periph_GPIOB
#define LEVEL3_GPIO_PORT GPIOB
#define LEVEL3_GPIO_PIN GPIO_Pin_9

#define LEVEL1 PBin(9)
#define LEVEL2 PBin(7)
#define LEVEL3 PBin(8)


void Level_GPIO_Config(void);

#endif /* __LEVEL_H */
