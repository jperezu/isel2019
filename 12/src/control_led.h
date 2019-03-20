#ifndef CONTROL_LED_H_
#define CONTROL_LED_H_


#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "fsm.h"
#include "define_constants.h"

typedef enum fsm_state {
	LED_OFF,
	LED_ON,
}led_fsm_state_t;


typedef struct led_fsm_{
	fsm_t fsm;	
	int led;
    int timeout_time;			
}led_fsm_t;

led_fsm_t* new_led_fsm(fsm_trans_t* led_transition_table, int led);
int delete_led_fsm(led_fsm_t* led_fsm);

#endif /*CONTROL_LED_H_*/