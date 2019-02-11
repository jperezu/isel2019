#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"

#define LED 2
#define PERIOD_TICK 100/portTICK_RATE_MS

//extern fsm_trans_t led_transition_table[];
#include "fsm.h"

#define REBOUND_TICK 200/portTICK_RATE_MS

#define ETS_GPIO_INTR_ENABLE()  _xt_isr_unmask(1 << ETS_GPIO_INUM)  //ENABLE INTERRUPTS
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM)    //DISABLE INTERRUPTS

volatile int pressed = 0;


typedef enum fsm_state {
	LED_OFF,
	LED_ON,
}led_fsm_state_t;


typedef struct led_fsm_{
	fsm_t fsm;	
	int led;			
}led_fsm_t;

void isr_gpio(void* arg);

led_fsm_t* new_led_fsm(fsm_trans_t* led_transition_table, int led);
int delete_led_fsm(led_fsm_t* led_fsm);

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int btn_pressed(fsm_t* fsm);

//OUTPUT FUNCTIONS
static void led_on(fsm_t* fsm);
static void led_off(fsm_t* fsm);

// State Machine: transition table
// { OrigState, TriggerCondition, NextState, OutputFunction }
fsm_trans_t led_transition_table[] = {
		{LED_OFF, btn_pressed, LED_ON, led_on },
		{LED_ON, btn_pressed, LED_OFF, led_off },
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
  if (status & BIT(0) || status & BIT(15)) {
    if (now > xLastISRTick0) {
      pressed = 1;
    }
    xLastISRTick0 = now + REBOUND_TICK;
  }

  //should not add print in interruption, except that we want to debug something
  //printf("in io intr: 0X%08x\r\n",status);                    //WRITE ON SERIAL UART0
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);       //CLEAR THE STATUS IN THE W1 INTERRUPT REGISTER
}

/////LED FSM object functions//////
led_fsm_t* new_led_fsm(fsm_trans_t* led_transition_table, int led){
	led_fsm_t* new_led_fsm = (led_fsm_t*) malloc(sizeof(led_fsm_t));

    if(new_led_fsm != NULL){
		new_led_fsm-> fsm.current_state = LED_OFF;
		new_led_fsm-> fsm.tt = led_transition_table;//Herencia
		new_led_fsm-> led = led;
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
static int btn_pressed(fsm_t* fsm){
    return pressed;
}


//OUTPUT FUNCTIONS
static void led_on(fsm_t* fsm){
    led_fsm_t* led_fsm = (led_fsm_t*) fsm;
    pressed = 0;
    GPIO_OUTPUT_SET(led_fsm -> led, 0);
}

static void led_off(fsm_t* fsm){
    led_fsm_t* led_fsm = (led_fsm_t*) fsm;
    pressed = 0;
    GPIO_OUTPUT_SET(led_fsm -> led, 1);
}


void task_led(void* ignore)
{   
    portTickType xLastWakeTime;

    PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);

    gpio_intr_handler_register((void*)isr_gpio, NULL);
    gpio_pin_intr_state_set(0, GPIO_PIN_INTR_NEGEDGE);
    gpio_pin_intr_state_set(15, GPIO_PIN_INTR_POSEDGE);
    ETS_GPIO_INTR_ENABLE();

    fsm_t* fsm = (fsm_t*) new_led_fsm(led_transition_table, LED);
    xLastWakeTime = xTaskGetTickCount ();

    while(true) {
    	fsm_fire (fsm);
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
    xTaskCreate(&task_led, "fsm", 2048, NULL, 1, NULL);
}

