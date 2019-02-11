#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "fsm.h"

#define DEBUG 0
#define ALARM 2
#define BUTTON 14
#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS

#define ETS_GPIO_INTR_ENABLE()  _xt_isr_unmask(1 << ETS_GPIO_INUM)  //ENABLE INTERRUPTS
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM)    //DISABLE INTERRUPTS

volatile int pressed = 0;


typedef enum fsm_state {
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
static int btn_high(fsm_t* fsm);
static int btn_low(fsm_t* fsm);
static int presence(fsm_t* fsm);

//OUTPUT FUNCTIONS
static void clear(fsm_t* fsm);
static void alarm_on(fsm_t* fsm);

// State Machine: transition table
// { OrigState, TriggerCondition, NextState, OutputFunction }
fsm_trans_t alarm_transition_table[] = {
    {DISARMED, btn_high, ARMED, clear },
	{ARMED, btn_low, DISARMED, clear },
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

/////ALARM FSM object functions//////
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
static int btn_high(fsm_t* fsm){
    return GPIO_INPUT_GET(BUTTON);
}
static int btn_low(fsm_t* fsm){
    return !GPIO_INPUT_GET(BUTTON);
}
static int presence(fsm_t* fsm){
    return (GPIO_INPUT_GET(12));
}


//OUTPUT FUNCTIONS
static void clear(fsm_t* fsm){
    alarm_fsm_t* alarm_fsm = (alarm_fsm_t*) fsm;
    GPIO_OUTPUT_SET(alarm_fsm -> alarm, 1);
}

static void alarm_on(fsm_t* fsm){
    alarm_fsm_t* alarm_fsm = (alarm_fsm_t*) fsm;
    GPIO_OUTPUT_SET(alarm_fsm -> alarm, 0);
}

// MAIN
void task_alarm(void* ignore)
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

    fsm_t* fsm = (fsm_t*) new_alarm_fsm(alarm_transition_table, ALARM);
    xLastWakeTime = xTaskGetTickCount ();

    while(true) {
    	fsm_fire (fsm);
        if (DEBUG)
            printf("BOTON: %d PULSADO: %d PRESENCIA: %d ESTADO: %d\n",
                    GPIO_INPUT_GET(14), pressed,GPIO_INPUT_GET(12), fsm->current_state);
		vTaskDelayUntil(&xLastWakeTime, PERIOD_TICK);
    }

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
    xTaskCreate(&task_alarm, "fsm", 2048, NULL, 1, NULL);
}

