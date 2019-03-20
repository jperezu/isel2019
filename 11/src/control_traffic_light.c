#include "control_traffic_light.h"

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS

static int not_other_timeout(fsm_t* fsm);
static int ready_other_timeout(fsm_t* fsm);
static int ambar_timeout(fsm_t* fsm);
static int red_timeout(fsm_t* fsm);

//OUTPUT FUNCTIONS
static void increase_green(fsm_t* fsm);
static void start_ambar(fsm_t* fsm);
static void start_red(fsm_t* fsm);
static void start_green(fsm_t* fsm);

// State Machine: transition table
// { OrigState, TriggerCondition, NextState, OutputFunction }
fsm_trans_t traffic_transition_table[] = {
	{CONTROL, ready_other_timeout, CONTROL, start_ambar},
	{CONTROL, not_other_timeout, CONTROL, increase_green},
	{CONTROL, ambar_timeout, CONTROL, start_red},
	{CONTROL, red_timeout, CONTROL, start_green},

	{-1, NULL, -1, NULL },
};



///// TRAFFIC FSM FUNCTIONS/////
traffic_fsm_t* new_traffic_fsm(fsm_trans_t* traffic_transition_table, int presence_road, int presence_other,
							int timeout_road, int timeout_other, fsm_t** other_fsm){
	traffic_fsm_t* new_traffic_fsm = (traffic_fsm_t*) malloc(sizeof(traffic_fsm_t));							
    traffic_fsm_t** fsm_other = (traffic_fsm_t**) other_fsm;
	
    if(new_traffic_fsm != NULL){
		new_traffic_fsm-> fsm.current_state = CONTROL;
		new_traffic_fsm-> fsm.tt = traffic_transition_table;
		new_traffic_fsm -> green = (presence_road == MAIN_SENSOR) ? 1 : 0;
		new_traffic_fsm -> ambar = 0;
		new_traffic_fsm -> red = (presence_road == MAIN_SENSOR) ? 0 : 1;
		new_traffic_fsm -> timeout = xTaskGetTickCount() + timeout_road/1000;
		new_traffic_fsm -> presence_road = presence_road;
		new_traffic_fsm -> presence_other = presence_other;
		new_traffic_fsm -> timeout_road = timeout_road;
		new_traffic_fsm -> timeout_other = timeout_other;
		if (other_fsm == NULL){int unity = -1; new_traffic_fsm -> other_closed = &unity;}
		else new_traffic_fsm -> other_closed =  &(*fsm_other)->red;	
	}

	return new_traffic_fsm;
}

int delete_traffic_fsm(traffic_fsm_t* traffic_fsm){
	free(traffic_fsm);
	return 1;
}

void set_green(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	traffic_fsm -> green = 1;
	traffic_fsm -> ambar = 0;
	traffic_fsm -> red = 0;
}
void set_ambar(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	traffic_fsm -> green = 0;
	traffic_fsm -> ambar = 1;
	traffic_fsm -> red = 0;
}

void set_red(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	traffic_fsm -> green = 0;
	traffic_fsm -> ambar = 0;
	traffic_fsm -> red = 1;
}

void set_timeout(fsm_t* fsm, int extend){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	traffic_fsm -> timeout = xTaskGetTickCount() + (extend/portTICK_RATE_MS);
}

int get_presence(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return !GPIO_INPUT_GET(traffic_fsm -> presence_road);
}

int get_other_presence(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return !GPIO_INPUT_GET(traffic_fsm -> presence_other);
}

int get_green(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return traffic_fsm -> green;
}
int get_ambar(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return traffic_fsm -> ambar;
}

int get_red(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return traffic_fsm -> red;
}

int get_timeout(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return traffic_fsm -> timeout <= xTaskGetTickCount() ;
}

int get_overtime(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return traffic_fsm -> timeout_road;
}

int get_overtime_other(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return traffic_fsm -> timeout_other;
}

int get_other_closed(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	return *(traffic_fsm -> other_closed);
}

///////FSM TABLE FUNCTIONS//////
//CONDITION FUNCTIONS
static int not_other_timeout(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	int presence;
	if (traffic_fsm ->presence_road == SECONDARY_SENSOR) presence = get_presence(fsm) & !get_other_presence(fsm);
	else presence = !get_other_presence(fsm);
	return presence & get_timeout(fsm) & get_green(fsm);
}

static int ready_other_timeout(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	if (traffic_fsm ->presence_road == SECONDARY_SENSOR) return get_timeout(fsm) & get_green(fsm);
	return !get_presence(fsm) & get_other_presence(fsm)  & get_timeout(fsm) & get_green(fsm);
}

static int ambar_timeout(fsm_t* fsm){
	return get_ambar(fsm) & get_timeout(fsm);
}

static int red_timeout(fsm_t* fsm){
	if (get_other_closed(fsm) != -1){
		return get_red(fsm) & get_timeout(fsm) & get_other_closed(fsm);
	}
	return get_red(fsm) & get_timeout(fsm);
}

//OUTPUT FUNCTIONS
static void increase_green(fsm_t* fsm){
	set_green(fsm);
	set_timeout(fsm, OVERTIME);
}

static void start_ambar(fsm_t* fsm){
	set_ambar(fsm);
	set_timeout(fsm, OVERTIME);
}

static void start_red(fsm_t* fsm){
	traffic_fsm_t* traffic_fsm = (traffic_fsm_t*) fsm;
	int secondary_ambar_time = OVERTIME*(traffic_fsm ->presence_other == SECONDARY_SENSOR);
	set_red(fsm);
	set_timeout(fsm, get_overtime_other(fsm) + secondary_ambar_time);
}

static void start_green(fsm_t* fsm){
	set_green(fsm);
	set_timeout(fsm, get_overtime(fsm));
}
