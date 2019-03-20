#ifndef DEFINE_CONSTANTS_H_
#define DEFIINE_CONSTANTS_H_

#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"

#define DEBUG 1


#define MAIN_SENSOR 14
#define SECONDARY_SENSOR 0

#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS

#define OVERTIME 1000 //In millisenconds
#define TIME_MAIN 5000 //In millisenconds
#define TIME_SECONDARY 3000 //In millisenconds

#endif /* DEFINE_CONSTANTS_H */