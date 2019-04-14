#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "reactor.h"
#include <string.h>
#include <stdio.h>
#include "fsm.h"
#include "control_alarm.h"
#include "control_digits.h"
#include "control_led.h"
#include "define_constants.h"

extern fsm_trans_t digit_transition_table[];
extern fsm_trans_t alarm_transition_table[];
extern fsm_trans_t led_transition_table[];
extern volatile int pressed;
extern volatile int pressed_switch;
extern volatile int presence_flag;

void isr_gpio(void* arg);
fsm_t* fsm_digit;
fsm_t* fsm_led;
fsm_t* fsm_alarm;

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void isr_gpio(void* arg) {
  static portTickType xLastISRTick0 = 0;
  static portTickType xLastISRTick1 = 0;
  static portTickType xLastISRTick2 = 0;
  static portTickType xLastISRTick3 = 0;

  uint32 status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);          //READ STATUS OF INTERRUPT

  portTickType now = xTaskGetTickCount ();

  if (status & BIT(BUTTON)) {
    if (now > xLastISRTick0) {
      pressed = 1;
    }
    xLastISRTick0 = now + REBOUND_TICK;
  }
  if (status & BIT(SWITCH_1)) {
    if (now > xLastISRTick1) {
      pressed_switch = 1;
    }
    xLastISRTick1 = now + REBOUND_TICK;
  }
  if (status & BIT(SWITCH_2)) {
    if (now > xLastISRTick2) {
      pressed_switch = 1;
    }
     xLastISRTick2 = now + REBOUND_TICK;
  }
  if (status & BIT(IR_SENSOR)) {
    if (now > xLastISRTick3) {
      presence_flag = 1;
    }
    xLastISRTick3 = now + REBOUND_TICK;
  }
//should not add print in interruption, except that we want to debug something
  //if (DEBUG) printf("in io intr: 0X%08x\r\n",status);                    //WRITE ON SERIAL UART0
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);       //CLEAR THE STATUS IN THE W1 INTERRUPT REGISTER
}

void gpio_init(){
  GPIO_ConfigTypeDef io_conf;

    io_conf.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;
    io_conf.GPIO_Mode = GPIO_Mode_Input;
    io_conf.GPIO_Pin = BIT(IR_SENSOR);
    io_conf.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_config(&io_conf);

    io_conf.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;
    io_conf.GPIO_Mode = GPIO_Mode_Input;
    io_conf.GPIO_Pin = BIT(BUTTON);
    io_conf.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_config(&io_conf);

    io_conf.GPIO_IntrType = GPIO_PIN_INTR_POSEDGE;
    io_conf.GPIO_Mode = GPIO_Mode_Output;
    io_conf.GPIO_Pin = BIT(ALARM);
    io_conf.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_config(&io_conf);

    PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);

    gpio_intr_handler_register((void*)isr_gpio, NULL);
    gpio_pin_intr_state_set(BUTTON, GPIO_PIN_INTR_POSEDGE);
    gpio_pin_intr_state_set(SWITCH_1, GPIO_PIN_INTR_NEGEDGE);
    gpio_pin_intr_state_set(SWITCH_2, GPIO_PIN_INTR_POSEDGE);
    gpio_pin_intr_state_set(IR_SENSOR, GPIO_PIN_INTR_NEGEDGE);
    ETS_GPIO_INTR_ENABLE();
}

// MAIN
static void digit_function(struct event_handler_t* this){   
    static portTickType period = 100 /portTICK_RATE_MS;

    fsm_fire (fsm_digit);
    this->next_activation += period;
}

static void led_function(struct event_handler_t* this){
  static portTickType period = 200 /portTICK_RATE_MS;

  fsm_fire (fsm_led);
  this->next_activation += period;
}   

static void alarm_function(struct event_handler_t* this){  
  static portTickType period = 500 /portTICK_RATE_MS;

  fsm_fire (fsm_alarm);
  this->next_activation += period;
} 


static void reactor_run (void* ignore){
  EventHandler eh1;
  EventHandler eh2;
  EventHandler eh3;

  reactor_init ();
  gpio_init();
  fsm_digit = (fsm_t*) new_digit_fsm(digit_transition_table);
  fsm_led = (fsm_t*) new_led_fsm(led_transition_table, LED);  
  fsm_alarm = (fsm_t*) new_alarm_fsm(alarm_transition_table, ALARM);

  event_handler_init (&eh1, 1, digit_function);
  event_handler_init (&eh2, 2, led_function);
  event_handler_init (&eh3, 3, alarm_function);
  
  reactor_add_handler (&eh1);
  reactor_add_handler (&eh2);
  reactor_add_handler (&eh3);

  while (1) {
      reactor_handle_events ();
  }
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
  xTaskHandle task;
  xTaskCreate (reactor_run, "reactor", 2048, NULL, 1, &task);
}

