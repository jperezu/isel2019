#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "fsm.h"

#define DEBUG 1
#define ALARM 2
#define BUTTON 14
#define DIG1 2
#define DIG2 5
#define DIG3 1

#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS
#define TIMEOUT_MAX 1000/portTICK_RATE_MS

#define ETS_GPIO_INTR_ENABLE()  _xt_isr_unmask(1 << ETS_GPIO_INUM)  //ENABLE INTERRUPTS
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM)    //DISABLE INTERRUPTS

volatile int pressed, number, done = 0;
volatile int digit = 1;

///// DIGITS FSM /////
typedef enum fsm_state_digit {
    NONE,
    ZERO,
	ONE,
	TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
}digit_fsm_state_t;


typedef struct digit_fsm_{
	fsm_t fsm;
    int timeout_time;			
}digit_fsm_t;

void isr_gpio(void* arg);

digit_fsm_t* new_digit_fsm(fsm_trans_t* digit_transition_table);
int delete_digit_fsm(digit_fsm_t* digit_fsm);

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
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
    {NONE, pressed_digit, ONE,  clean_increment},
    {ZERO, timeout_right, NONE, right_value },
    {ZERO, timeout_wrong, NONE, wrong_value },
    {ZERO, pressed_digit, ONE,  clean_increment},
    {ONE, timeout_right, NONE, right_value },
    {ONE, timeout_wrong, NONE, wrong_value },
    {ONE, pressed_digit, TWO,  clean_increment},
	{TWO, timeout_right, NONE, right_value },
    {TWO, timeout_wrong, NONE, wrong_value },
    {TWO, pressed_digit, THREE,  clean_increment},
    {THREE, timeout_right, NONE, right_value },
    {THREE, timeout_wrong, NONE, wrong_value },
    {THREE, pressed_digit, FOUR, clean_increment},
    {FOUR, timeout_right, NONE, right_value },
    {FOUR, timeout_wrong, NONE, wrong_value },
    {FOUR, pressed_digit, FIVE,  clean_increment},
    {FIVE, timeout_right, NONE, right_value },
    {FIVE, timeout_wrong, NONE, wrong_value },
    {FIVE, pressed_digit, SIX,  clean_increment},
    {SIX, timeout_right, NONE, right_value },
    {SIX, timeout_wrong, NONE, wrong_value },
    {SIX, pressed_digit, SEVEN,  clean_increment},
    {SEVEN, timeout_right, NONE, right_value },
    {SEVEN, timeout_wrong, NONE, wrong_value },
    {SEVEN, pressed_digit, EIGHT,  clean_increment},
    {EIGHT, timeout_right, NONE, right_value },
    {EIGHT, timeout_wrong, NONE, wrong_value },
    {EIGHT, pressed_digit, NINE,  clean_increment},
    {NINE, timeout_right, NONE, right_value},
    {NINE, timeout_wrong, NONE, wrong_value },
    {NINE, pressed_digit, ZERO,  clean_increment},
	{-1, NULL, -1, NULL },
};

///// ALARM FSM /////
typedef enum fsm_state_alarm {
	DISARMED,
	ARMED,
}alarm_fsm_state_t;


typedef struct alarm_fsm_{
	fsm_t fsm;	
	int alarm;			
}alarm_fsm_t;

alarm_fsm_t* new_alarm_fsm(fsm_trans_t* alarm_transition_table, int alarm);
int delete_alarm_fsm(alarm_fsm_t* alarm_fsm);

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

  uint32 status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);          //READ STATUS OF INTERRUPT

  portTickType now = xTaskGetTickCount ();
  if (status & BIT(BUTTON)) {
    if (now > xLastISRTick0) {
      pressed = 1;
    }
    xLastISRTick0 = now + REBOUND_TICK;
  }
//should not add print in interruption, except that we want to debug something
  //if (DEBUG) printf("in io intr: 0X%08x\r\n",status);                    //WRITE ON SERIAL UART0
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);       //CLEAR THE STATUS IN THE W1 INTERRUPT REGISTER
}

/////DIGIT FSM object functions//////
digit_fsm_t* new_digit_fsm(fsm_trans_t* digit_transition_table){
	digit_fsm_t* new_digit_fsm = (digit_fsm_t*) malloc(sizeof(digit_fsm_t));

    if(new_digit_fsm != NULL){
		new_digit_fsm-> fsm.current_state = NONE;
		new_digit_fsm-> fsm.tt = digit_transition_table;//Herencia
        new_digit_fsm -> timeout_time = xTaskGetTickCount () + TIMEOUT_MAX;
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
static int pressed_digit(fsm_t* fsm){
    if (pressed){
        switch (digit)
            {
                case 1:
                    return 1;
                case 2:
                    return 1;
                case 3:
                    return 1;
                default:
                    return 0;
            }
    }
    return 0;
}

static int timeout_right(fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    if(xTaskGetTickCount () >= digit_fsm -> timeout_time){
        switch (digit)
            {
                case 1:
                    if(number == DIG1)return 1;
                    return 0;
                case 2:
                    if(number == DIG2) return 1;
                    return 0;
                case 3:
                    if(number == DIG3) return 1;
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
        switch (digit)
            {
                case 1:
                    if(number == DIG1) return 0;
                    return 1;
                case 2:
                    if(number == DIG2) return 0;
                    return 1;
                case 3:
                    if(number == DIG3) return 0;
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
    if (number < 9) number++;
    else number = 0;
    
    if (DEBUG) printf("DIGIT %d, NUMBER: %d\n", digit, number);

    digit_fsm -> timeout_time = xTaskGetTickCount () + TIMEOUT_MAX;
}

static void right_value (fsm_t* fsm){
    number = 0;
    pressed = 0;
    switch (digit)
            {
                case 1:
                    if (DEBUG) printf("DIGIT: %d -> CORRECT NUMBER\n", digit);
                    digit = 2;
                    break;
                case 2:
                    if (DEBUG) printf("DIGIT: %d -> CORRECT NUMBER\n", digit);
                    digit = 3;
                    break;
                case 3:
                    if (DEBUG) printf("DIGIT: %d -> CORRECT NUMBER\n", digit);
                    digit = 1;
                    done = 1;
                    if (DEBUG) printf("PASSWORD COMPLETED\n");
                    break;
                default:
                    digit = 0;
                    break;
            }
}

static void wrong_value (fsm_t* fsm){
    if (DEBUG) printf("DIGIT: %d -> INCORRECT NUMBER, TRY AGAIN\n", digit);
    digit = 1;
    number = 0;
    pressed = 0;
    done = 0;
}

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
    if (DEBUG) {
        alarm_fsm_t* alarm_fsm = (alarm_fsm_t*) fsm;
        if (done & fsm -> current_state == 0)  printf("ALARM ON\n");
        if (done & fsm -> current_state == 1)  printf("ALARM OFF\n");
    }
    return done;
}

static int presence(fsm_t* fsm){
    return (GPIO_INPUT_GET(12));
}


//OUTPUT FUNCTIONS
static void clear(fsm_t* fsm){
    alarm_fsm_t* alarm_fsm = (alarm_fsm_t*) fsm;

    done = 0;
    GPIO_OUTPUT_SET(alarm_fsm -> alarm, 1);
}

static void alarm_on(fsm_t* fsm){
    alarm_fsm_t* alarm_fsm = (alarm_fsm_t*) fsm;
    GPIO_OUTPUT_SET(alarm_fsm -> alarm, 0);
    
    //if (DEBUG) printf("INTRUDER!\n");
}


// MAIN
void task_alarm_digit(void* ignore)
{   
    portTickType xLastWakeTime;
    GPIO_ConfigTypeDef io_conf;

    io_conf.GPIO_IntrType = GPIO_PIN_INTR_POSEDGE;
    io_conf.GPIO_Mode = GPIO_Mode_Input;
    io_conf.GPIO_Pin = BIT(12);
    io_conf.GPIO_Pullup = GPIO_PullUp_DIS;
    gpio_config(&io_conf);

    io_conf.GPIO_IntrType = GPIO_PIN_INTR_POSEDGE;
    io_conf.GPIO_Mode = GPIO_Mode_Input;
    io_conf.GPIO_Pin = BIT(BUTTON);
    io_conf.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_config(&io_conf);

    PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);

    gpio_intr_handler_register((void*)isr_gpio, NULL);
    gpio_pin_intr_state_set(BUTTON, GPIO_PIN_INTR_POSEDGE);
    ETS_GPIO_INTR_ENABLE();

    fsm_t* fsm_digit= (fsm_t*) new_digit_fsm(digit_transition_table);
    fsm_t* fsm_alarm = (fsm_t*) new_alarm_fsm(alarm_transition_table, ALARM);

    xLastWakeTime = xTaskGetTickCount ();

    while(true) {
    	fsm_fire (fsm_digit);
        fsm_fire (fsm_alarm);
		vTaskDelayUntil(&xLastWakeTime, PERIOD_TICK);
    }

    delete_digit_fsm((digit_fsm_t*) fsm_digit);
    delete_alarm_fsm((alarm_fsm_t*)fsm_alarm);
    vTaskDelete(NULL);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    xTaskCreate(&task_alarm_digit, "fsm", 2048, NULL, 1, NULL);
}

