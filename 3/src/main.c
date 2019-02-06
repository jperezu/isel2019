#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


const char * morse (char c);
int str2morse (char * buf, int n, const char * str);
int morse_send (const char* msg);

#define MS_DOT 250
#define LED 2

void task_morse(void* ignore)
{   
    int n = 32;
    char* buf = (char *) malloc((n+1) * sizeof(char));
    char* msg = "hola mundo";

    str2morse(buf, n, msg);
    printf("%s  --> %s\n", msg, buf);
    morse_send(buf);
    free(buf);

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
    xTaskCreate(&task_morse, "startup", 2048, NULL, 1, NULL);
}

/* Devuelve la correspondencia en Morse del caracter c */
const char * morse (char c){
    static const char * morse_ch[] = {
        "._",     //A
        "_...",   //B
        "_._.",   //C
        "_..",    //D
        ".",      //E
        ".._.",   //F
        "__.",    //G
        "....",   //H
        "..",     //I
        ".___",   //J
        "_._",    //K
        "._..",   //L
        "__",     //M
        "_.",     //N
        "___",    //O
        ".__.",   //P
        "__._",   //Q
        "._.",    //R
        "...",    //S
        "-",      //T
        ".._",    //U
        "..._",   //V
        ".__",    //W
        "_.._",   //X
        "_.__",   //Y
        "__..",   //Z
        ".____",  //1
        "..___",  //2
        "...__",  //3
        "...._",  //4
        ".....",  //5
        "_....",  //6
        "__...",  //7
        "___..",  //8
        "____.",  //9
        "......", //.
        "._._._", //,
        "..__..", //?
        "__..__"  //!
    };

    return morse_ch[c - 'a'];
}

/* Copia en buf la version en Morse del mensaje str, con un limite de n caracteres*/
int str2morse (char* buf, int n, const char* str){
    char* parsed;
    int i;

    strcpy(parsed,morse(*str++));

    while (*str != '\0'){
        strcat(parsed, "   ");
        strcat(parsed, (*str == ' ') ? "  " : morse(*str));
        str++;
    }

    for (i = 0; i < n; i++) *buf++ = *parsed++;
    *(buf+1) = '\0';

    return 0;
}

/*Envia el mensaje msg ya codificadp en Morse, encendiendo y apagando el LED */
int morse_send (const char* msg){
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount ();

    GPIO_OUTPUT_SET(LED, 0);
    switch (*msg){
        case '.':
            break;
        case '_':
            vTaskDelayUntil(&xLastWakeTime, (2 * MS_DOT)/portTICK_RATE_MS);
            break;
        case ' ':
            break;
        case '\0':
            GPIO_OUTPUT_SET(LED, 1);
            return 0;
    }
    vTaskDelayUntil(&xLastWakeTime, MS_DOT/portTICK_RATE_MS);
    GPIO_OUTPUT_SET(LED, 1);
    vTaskDelayUntil(&xLastWakeTime, MS_DOT/portTICK_RATE_MS);
    morse_send(++msg);
    return 0;
}