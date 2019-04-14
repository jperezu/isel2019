#ifndef CONTROL_DIGITS_H_
#define CONTROL_DIGITS_H_

#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "fsm.h"
#include "define_constants.h"


///// DIGITS FSM /////
typedef enum fsm_state_digit {
    IDLE,
}digit_fsm_state_t;


typedef struct digit_fsm_{
	fsm_t fsm;
    int timeout_time;
    int digit;	
    int number;	
}digit_fsm_t;

digit_fsm_t* new_digit_fsm(fsm_trans_t* digit_transition_table);
int delete_digit_fsm(digit_fsm_t* digit_fsm);

#endif /*CONTROL_DIGITS_H_*/