#ifndef CONTROL_TRAFFIC_LIGHT_H_
#define CONTROL_TRAFFIC_LIGHT_H_

#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"
#include "fsm.h"
#include "define_constants.h"


///// TRAFFIC_LIGHT FSM /////
typedef enum fsm_state_traffic{
	CONTROL,
}traffic_fsm_state_t;


typedef struct traffic_fsm_{
	fsm_t fsm;	
	int green;
	int ambar;
	int red;
	int timeout;
	int presence_road;
	int presence_other;
	int timeout_road;
	int timeout_other;
	int* other_closed;	
    		
}traffic_fsm_t;

traffic_fsm_t* new_traffic_fsm(fsm_trans_t* traffic_transition_table, int presence_road, int presence_other,
							int timeout_road, int timeout_other, fsm_t** other_fsm);
int delete_traffic_fsm(traffic_fsm_t* traffic_fsm);

void set_green(fsm_t* fsm);
void set_ambar(fsm_t* fsm);
void set_red(fsm_t* fsm);
void set_timeout(fsm_t* fsm, int extend);

int get_presence(fsm_t* fsm);
int get_other_presence(fsm_t* fsm);
int get_green(fsm_t* fsm);
int get_ambar(fsm_t* fsm);
int get_red(fsm_t* fsm);
int get_timeout(fsm_t* fsm);
int get_overtime(fsm_t* fsm);
int get_overtime_other(fsm_t* fsm);
int get_other_closed(fsm_t* fsm);

#endif /* CONTROL_TRAFFIC_LIGHT_H_*/