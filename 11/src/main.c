#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "fsm.h"
#include "control_traffic_light.h"
#include "define_constants.h"

extern fsm_trans_t traffic_transition_table[];

char* get_color(int state);


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

// MAIN
void task_traffic(void* ignore)
{   
    portTickType xLastWakeTime;
    
    GPIO_ConfigTypeDef io_conf;

    io_conf.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;
    io_conf.GPIO_Mode = GPIO_Mode_Input;
    io_conf.GPIO_Pin = BIT(MAIN_SENSOR);
    io_conf.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_config(&io_conf);

    io_conf.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;
    io_conf.GPIO_Mode = GPIO_Mode_Input;
    io_conf.GPIO_Pin = BIT(SECONDARY_SENSOR);
    io_conf.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_config(&io_conf);

    fsm_t* fsm_main = (fsm_t*) new_traffic_fsm(traffic_transition_table, MAIN_SENSOR, SECONDARY_SENSOR, TIME_MAIN, TIME_SECONDARY, NULL);
    fsm_t* fsm_secondary = (fsm_t*) new_traffic_fsm(traffic_transition_table, SECONDARY_SENSOR, MAIN_SENSOR, TIME_SECONDARY, TIME_MAIN, &fsm_main);

    xLastWakeTime = xTaskGetTickCount ();
        int color_main_next = 0;
        int color_secondary_next = 0;
        int timer = 0;
        int timer_next = 0;
        int timer_start = 0;

    while(true) {

        fsm_fire (fsm_main);
        fsm_fire (fsm_secondary);
		vTaskDelayUntil(&xLastWakeTime, PERIOD_TICK);

        if (DEBUG){
        int color_main = 100*(((traffic_fsm_t*) fsm_main) -> green) + 10*(((traffic_fsm_t*) fsm_main) -> ambar) + (((traffic_fsm_t*) fsm_main) -> red);
        int  color_secondary = 100*(((traffic_fsm_t*) fsm_secondary) -> green) + 10*(((traffic_fsm_t*) fsm_secondary) -> ambar) + (((traffic_fsm_t*) fsm_secondary) -> red);
        int timer = xTaskGetTickCount()/100 - timer_start;

        if(timer >= timer_next || timer == 0){
            printf("\rPrincipal: %s \t Secundario: %s \t", get_color(color_main), get_color(color_secondary));
            printf("Sensor_M: %d \t Sensor_S: %d \t", !GPIO_INPUT_GET(MAIN_SENSOR), !GPIO_INPUT_GET(SECONDARY_SENSOR));    
            printf("Contador: %d \t\t", timer);
            fflush(stdout);
            timer_next = timer + 1;
        }

        if (color_main != color_main_next || color_secondary != color_secondary_next){
            timer_start = xTaskGetTickCount()/100;
            color_main_next = color_main;
            color_secondary_next = color_secondary;
        }
      }
    }

    delete_digit_fsm((traffic_fsm_t*) fsm_main);
    delete_alarm_fsm((traffic_fsm_t*) fsm_secondary);
    vTaskDelete(NULL);
}

char* get_color(int state){
    switch (state){
        case 1:
            return (char*) "rojo";
        case 10:
            return (char*) "ambar";
        case 100:
            return (char*) "verde";
        default:
            return (char*) "error";
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
    xTaskCreate(&task_traffic, "fsm", 2048, NULL, 1, NULL);
}

