#ifndef CONTROL_ALARM_H_
#define CONTROL_ALARM_H_

#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "fsm.h"
#include "define_constants.h"
#include "control_digits.h"


///// ALARM FSM /////
typedef enum fsm_state_alarm {
	DISARMED,
	ARMED,
}alarm_fsm_state_t;


typedef struct alarm_fsm_{
	fsm_t fsm;	
	int alarm;
    int* done;	
    		
}alarm_fsm_t;

alarm_fsm_t* new_alarm_fsm(fsm_trans_t* alarm_transition_table, int alarm, fsm_t** digit_fsm);
int delete_alarm_fsm(alarm_fsm_t* alarm_fsm);

#endif /* CONTROL_ALARM_H_*/