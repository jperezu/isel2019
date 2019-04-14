#include "control_led.h"

volatile int pressed_switch = 0;
volatile int presence = 0;

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int btn_pressed_presence(fsm_t* fsm);
static int timeout(fsm_t* fsm);

//OUTPUT FUNCTIONS
static void led_on(fsm_t* fsm);
static void led_off(fsm_t* fsm);

// State Machine: transition table
// { OrigState, TriggerCondition, NextState, OutputFunction }
fsm_trans_t led_transition_table[] = {
		{LED_OFF, btn_pressed_presence, LED_ON, led_on },
        {LED_ON, btn_pressed_presence, LED_ON, led_on },
		{LED_ON, timeout, LED_OFF, led_off },
		{-1, NULL, -1, NULL },
};

/////LED FSM object functions//////
led_fsm_t* new_led_fsm(fsm_trans_t* led_transition_table, int led){
	led_fsm_t* new_led_fsm = (led_fsm_t*) malloc(sizeof(led_fsm_t));

    if(new_led_fsm != NULL){
		new_led_fsm-> fsm.current_state = LED_OFF;
		new_led_fsm-> fsm.tt = led_transition_table;//Herencia
		new_led_fsm-> led = led;
        new_led_fsm -> timeout_time = xTaskGetTickCount () + TIMEOUT_MAX;
	}
    
    led_off((fsm_t*) new_led_fsm);
	return new_led_fsm;
}

int delete_led_fsm(led_fsm_t* led_fsm){
	free(led_fsm);
	return 1;
}

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int btn_pressed_presence(fsm_t* fsm){
    return pressed_switch || presence_flag;
}
static int timeout(fsm_t* fsm){
    led_fsm_t* led_fsm = (led_fsm_t*) fsm;
    if (xTaskGetTickCount () >= led_fsm -> timeout_time) {
        return 1;
    }
    return 0;
}

//OUTPUT FUNCTIONS
static void led_on(fsm_t* fsm){
    led_fsm_t* led_fsm = (led_fsm_t*) fsm;
    led_fsm -> timeout_time = xTaskGetTickCount () + TIMEOUT_MAX;   
    pressed_switch = 0;
    presence_flag = 0;
    presence_alarm = 1;
    GPIO_OUTPUT_SET(led_fsm -> led, 0);
    if(DEBUG) printf("LED ON\n");
}

static void led_off(fsm_t* fsm){
    led_fsm_t* led_fsm = (led_fsm_t*) fsm;
    pressed_switch = 0;
    GPIO_OUTPUT_SET(led_fsm -> led, 1);
    if(DEBUG) printf("LED OFF\n");
}