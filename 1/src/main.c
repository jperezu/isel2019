#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>

//Define total number of rows to calculate
#define N_ROWS 7

int n_elements_calculator(int n_rows);
void fill_pyramid(int * pnumbers, int n_elements);
void print_pyramid(int * pnumbers, int n_elements);

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

void task_pascal(void* ignore)
{   
    int * pnumbers;
    int n_elements;

    n_elements = n_elements_calculator(N_ROWS);
    pnumbers = (int *) malloc(n_elements * sizeof(int));

    fill_pyramid(pnumbers, n_elements);
    print_pyramid(pnumbers, n_elements);
    free(pnumbers);

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
    xTaskCreate(&task_pascal, "pascal", 2048, NULL, 1, NULL);
}

//Calculates total number of elements for the number of rows defined
int n_elements_calculator(int n_rows){
    int i;
    int n_elements = 0;

    for (i = 0; i <= n_rows; i++){
        n_elements += i;
    }
    return n_elements;
}

//Writes the values in memory
void fill_pyramid(int * pnumbers, int n_elements){
    int i;
    int row_elem = 1;
    int current_row = 1;

    for (i = 0; i < n_elements; i++){
        if (row_elem == current_row){
            *(pnumbers + i) = 1;
            row_elem = 1;
            current_row ++;
        }
        else if (row_elem == 1) {
            *(pnumbers + i) = 1;
            row_elem ++;
        }
        else {
            *(pnumbers + i) = *(pnumbers + i - current_row) + *(pnumbers + i + 1 - current_row);
            row_elem ++;
        }

    }
}

//Print the results in a pyramid shape
void print_pyramid(int * pnumbers, int n_elements){
    int i;
    int row_elem = 1;
    int current_row = 1;

    for (i = 0; i < n_elements; i++) {
        printf("%d ", pnumbers[i]);

        if (row_elem == current_row){
            printf("\n");
            row_elem = 1;
            current_row ++;
        }
        else {
            row_elem ++;
        }
    }
}