#include "control_digits.h"

volatile int pressed = 0;
volatile int done = 0;

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int first_not_pressed(fsm_t* fsm);
static int pressed_digit(fsm_t* fsm);
static int timeout_right(fsm_t* fsm);
static int timeout_wrong(fsm_t* fsm);

//OUTPUT FUNCTIONS
static void clean_increment(fsm_t* fsm);
static void right_value(fsm_t* fsm);
static void wrong_value(fsm_t* fsm);

// State Machine: transition table
// { OrigState, TriggerCondition, NextState, OutputFunction }
fsm_trans_t digit_transition_table[] = {
    {IDLE, pressed_digit, IDLE,  clean_increment},
    {IDLE, first_not_pressed, IDLE,  NULL},
    {IDLE, timeout_right, IDLE, right_value },
    {IDLE, timeout_wrong, IDLE, wrong_value },
	{-1, NULL, -1, NULL },
};


/////DIGIT FSM object functions//////
digit_fsm_t* new_digit_fsm(fsm_trans_t* digit_transition_table){
	digit_fsm_t* new_digit_fsm = (digit_fsm_t*) malloc(sizeof(digit_fsm_t));

    if(new_digit_fsm != NULL){
		new_digit_fsm-> fsm.current_state = IDLE;
		new_digit_fsm-> fsm.tt = digit_transition_table;//Herencia
        new_digit_fsm -> timeout_time = xTaskGetTickCount () + TIMEOUT_MAX;
        new_digit_fsm -> digit = 1;	
        new_digit_fsm -> number = -1;
	}
    //wrong_value((fsm_t*) new_digit_fsm);
	return new_digit_fsm;
}

int delete_digit_fsm(digit_fsm_t* digit_fsm){
	free(digit_fsm);
	return 1;
}

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int first_not_pressed(fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    return digit_fsm->number == -1;
}

static int pressed_digit(fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    return pressed & !done;
}

static int timeout_right(fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    if(xTaskGetTickCount () >= digit_fsm -> timeout_time){
        switch (digit_fsm -> digit)
            {
                case 1:
                    if(digit_fsm->number == DIG1)return 1;
                    return 0;
                case 2:
                    if(digit_fsm->number == DIG2) return 1;
                    return 0;
                case 3:
                    if(digit_fsm->number == DIG3) return 1;
                    return 0;
                default:
                    return 0;
            }
    }
    return 0;
}
static int timeout_wrong(fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    if(xTaskGetTickCount () >= digit_fsm -> timeout_time){
        switch (digit_fsm -> digit)
            {
                case 1:
                    if(digit_fsm->number == DIG1) return 0;
                    return 1;
                case 2:
                    if(digit_fsm->number == DIG2) return 0;
                    return 1;
                case 3:
                    if(digit_fsm->number == DIG3) return 0;
                    return 1;
                default:
                    return 1;
            }
    }
    return 0;
}

//OUTPUT FUNCTIONS
static void clean_increment(fsm_t* fsm){  
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    pressed = 0;
    
    if(digit_fsm->number <= 0) digit_fsm->number = 1;
    else if (digit_fsm->number < 9) digit_fsm->number++;
    else digit_fsm->number = 0;
    
    if (DEBUG) printf("DIGIT %d, NUMBER: %d\n", digit_fsm -> digit, digit_fsm->number);

    digit_fsm -> timeout_time = xTaskGetTickCount () + TIMEOUT_MAX;
}

static void right_value (fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    digit_fsm->number = -1;
    pressed = 0;
    switch (digit_fsm -> digit)
            {
                case 1:
                    if (DEBUG) printf("DIGIT: %d -> CORRECT NUMBER\n", digit_fsm -> digit);
                    digit_fsm -> digit = 2;
                    break;
                case 2:
                    if (DEBUG) printf("DIGIT: %d -> CORRECT NUMBER\n", digit_fsm -> digit);
                    digit_fsm -> digit = 3;
                    break;
                case 3:
                    if (DEBUG) printf("DIGIT: %d -> CORRECT NUMBER\n", digit_fsm -> digit);
                    digit_fsm -> digit = 1;
                    done = 1;
                    if (DEBUG) printf("PASSWORD COMPLETED\n");
                    break;
                default:
                    digit_fsm -> digit = 0;
                    break;
            }
}

static void wrong_value (fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    if (DEBUG) printf("DIGIT: %d -> INCORRECT NUMBER, TRY AGAIN\n", digit_fsm -> digit);
    digit_fsm -> digit = 1;
    digit_fsm->number = -1;
    pressed = 0; 
}