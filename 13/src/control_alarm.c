#include "control_alarm.h"

volatile int presence_alarm = 0;

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int password_right(fsm_t* fsm);
static int presence(fsm_t* fsm);

//OUTPUT FUNCTIONS
static void clear(fsm_t* fsm);
static void alarm_on(fsm_t* fsm);

// State Machine: transition table
// { OrigState, TriggerCondition, NextState, OutputFunction }
fsm_trans_t alarm_transition_table[] = {
    {DISARMED, password_right, ARMED, clear },
	{ARMED, password_right, DISARMED, clear },
    {ARMED, presence, ARMED, alarm_on },
	{-1, NULL, -1, NULL },
};



///// ALARM FSM FUNCTIONS/////
alarm_fsm_t* new_alarm_fsm(fsm_trans_t* alarm_transition_table, int alarm){
	alarm_fsm_t* new_alarm_fsm = (alarm_fsm_t*) malloc(sizeof(alarm_fsm_t));
    if(new_alarm_fsm != NULL){
		new_alarm_fsm-> fsm.current_state = DISARMED;
		new_alarm_fsm-> fsm.tt = alarm_transition_table;//Herencia
		new_alarm_fsm-> alarm = alarm;
	}
    
    clear((fsm_t*) new_alarm_fsm);
	return new_alarm_fsm;
}
int delete_alarm_fsm(alarm_fsm_t* alarm_fsm){
	free(alarm_fsm);
	return 1;
}

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int password_right(fsm_t* fsm){
    int last_digit = done == 1;
    if (DEBUG) {
        if (last_digit & fsm -> current_state == 0)  printf("ALARM ON\n");
        if (last_digit & fsm -> current_state == 1)  printf("ALARM OFF\n");
    }
    return last_digit;
}

static int presence(fsm_t* fsm){
    return presence_alarm;
}


//OUTPUT FUNCTIONS
static void clear(fsm_t* fsm){
    alarm_fsm_t* alarm_fsm = (alarm_fsm_t*) fsm;

    done = 0;
    presence_alarm = 0;
    GPIO_OUTPUT_SET(alarm_fsm -> alarm, 0);
}

static void alarm_on(fsm_t* fsm){
    alarm_fsm_t* alarm_fsm = (alarm_fsm_t*) fsm;
    presence_alarm = 0;
    GPIO_OUTPUT_SET(alarm_fsm -> alarm, 1);
    
    //if (DEBUG) printf("INTRUDER!\n");
}
