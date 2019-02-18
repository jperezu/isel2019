#ifndef DEFINE_CONSTANTS_H_
#define DEFIINE_CONSTANTS_H_

#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"

#define DEBUG 1

#define ALARM 13
#define LED 2
#define IR_SENSOR 12
#define BUTTON 14
#define SWITCH_1 0
#define SWITCH_2 15

#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS
#define TIMEOUT_MAX 1000/portTICK_RATE_MS
#define TIMEOUT_LIGHT_MAX 1000/portTICK_RATE_MS

#define ETS_GPIO_INTR_ENABLE()  _xt_isr_unmask(1 << ETS_GPIO_INUM)  //ENABLE INTERRUPTS
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM)    //DISABLE INTERRUPTS

volatile int pressed;

#endif /* DEFINE_CONSTANTS_H */