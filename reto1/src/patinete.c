#include "esp_common.h"
#include "espconn.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "fsm.h"
#include "c_types.h"


#define SSID      "TelecoEmprende"
#define PASSWORD "T3L3C03MPR3ND3"     

const uint8 udp_server_ip[4] = { 192, 168, 2, 232 }; //Direccion IP servidor 
#define UDP_SERVER_LOCAL_PORT (1024) //Puerto de entrada

#define UDP_Client_GREETING "PRUEBA ENVIO UDP\n"


// CONSTANTS //
#define PERIOD_TICK 100/portTICK_RATE_MS
#define REBOUND_TICK 200/portTICK_RATE_MS

#define HYPERPERIOD 1000
#define SECONDARY 100

#define TIMEOUT_BUFFER 1000/portTICK_RATE_MS
#define TIMEOUT_SEND 1000/portTICK_RATE_MS
#define TIMEOUT_MAX 1000/portTICK_RATE_MS
#define TIMEOUT_UPDATE_FLAGS 100/portTICK_RATE_MS

#define LIMIT 500 //LIMITE BATERIA BAJA
#define MAX_LENGTH 255

#define BAT_FLAG 12 //LED BATERIA CARGADA GPIO12
#define RENTED_FLAG 13 //LED ALQUILADO GPIO13
#define BUTTON 14 //BOTON PARA CONTRASEÑA GPIO14
#define LOW_FLAG 15 //LED BATERIA BAJA GPIO15

//CONTRASEÑA CORRECTA
#define DIG1 1
#define DIG2 1
#define DIG3 1

#define DEBUG 0

#define ETS_GPIO_INTR_ENABLE()  _xt_isr_unmask(1 << ETS_GPIO_INUM)  //ENABLE INTERRUPTS
#define ETS_GPIO_INTR_DISABLE() _xt_isr_mask(1 << ETS_GPIO_INUM)    //DISABLE INTERRUPTS

//////////////////////////////////////////////

// HEADER //

volatile int vbat = 0;
volatile int bat_low = 1;
volatile int code_ok = 0;
volatile int lock = 1;
volatile int pressed = 0;
volatile int connected = 0;

static struct espconn udp_client;
static const char* BATTERY[] = {"OK", "LOW",};
static const char* RENT[] = {"NOT_RENTED", "RENTED",};

typedef struct buffer{
    char* info;
    struct buffer *next;
} buffer;

buffer *buff;

typedef enum buffer_fsm_state {
	BUFFER,
}buffer_fsm_state_t;

typedef enum send_fsm_state {
	SEND,
}send_fsm_state_t;

typedef enum battery_fsm_state {	
  BAT_OK,
  BAT_LOW,
}battery_fsm_state_t;

typedef enum rent_fsm_state {
	NOT_RENTED,
  RENTED,
}rent_fsm_state_t;

typedef enum fsm_state_digit {
    IDLE,
}digit_fsm_state_t;

typedef enum fsm_state_update {
    UPDATE,
}update_fsm_state_t;

typedef struct buffer_fsm_{
	fsm_t fsm;	
	int next;			
}buffer_fsm_t;

typedef struct send_fsm_{
	fsm_t fsm;	
	int next;			
}send_fsm_t;

typedef struct battery_fsm_{
	fsm_t fsm;			
}battery_fsm_t;

typedef struct rent_fsm_{
	fsm_t fsm;	
}rent_fsm_t;

typedef struct digit_fsm_{
	fsm_t fsm;
    int timeout_time;
    int digit;	
    int number;		
}digit_fsm_t;

typedef struct update_fsm_{
	fsm_t fsm;	
  int timeout_update;	
}update_fsm_t;

//init
buffer_fsm_t* new_buffer_fsm(fsm_trans_t* buffer_transition_table);
int delete_buffer_fsm(buffer_fsm_t* buffer_fsm);

send_fsm_t* new_send_fsm(fsm_trans_t* send_transition_table);
int delete_send_fsm(send_fsm_t* send_fsm);

battery_fsm_t* new_battery_fsm(fsm_trans_t* battery_transition_table);
int delete_battery_fsm(battery_fsm_t* battery_fsm);

rent_fsm_t* new_rent_fsm(fsm_trans_t* rent_transition_table);
int delete_rent_fsm(rent_fsm_t* rent_fsm);

digit_fsm_t* new_digit_fsm(fsm_trans_t* digit_transition_table);
int delete_digit_fsm(digit_fsm_t* digit_fsm);

update_fsm_t* new_update_fsm(fsm_trans_t* update_transition_table);
int delete_update_fsm(update_fsm_t* update_fsm);

//condition
static int time_timeout(fsm_t* fsm);

static int timeout_empty(fsm_t* fsm);

static int battery_charged(fsm_t* fsm);
static int battery_discharged(fsm_t* fsm);

static int available(fsm_t* fsm);
static int user_left(fsm_t* fsm);

static int first_not_pressed(fsm_t* fsm);
static int pressed_digit(fsm_t* fsm);
static int timeout_right(fsm_t* fsm);
static int timeout_wrong(fsm_t* fsm);

static int update_timeout(fsm_t* fsm);


//Output
static void push_buf(fsm_t* fsm);

static void send_msg(fsm_t* fsm);

static void set_not_low(fsm_t* fsm);
static void set_low(fsm_t* fsm);

static void set_unlock(fsm_t* fsm);
static void set_lock(fsm_t* fsm);

static void clean_increment(fsm_t* fsm);
static void right_value(fsm_t* fsm);
static void wrong_value(fsm_t* fsm);

static void update_flags(fsm_t* fsm);

int push(buffer *buff, char* msg);
char* pop(buffer *buff);
int not_empty( buffer *buff);

void isr_gpio(void* arg);

/////////////////////////////////////////////

//TRANSITION TABLES
fsm_trans_t buffer_transition_table[] = {
		{BUFFER, time_timeout, BUFFER, push_buf},
		{-1, NULL, -1, NULL },
};

fsm_trans_t send_transition_table[] = {
		{SEND, timeout_empty, SEND, send_msg},
		{-1, NULL, -1, NULL },
};

fsm_trans_t battery_transition_table[] = {
		{BAT_LOW, battery_charged, BAT_OK, set_not_low },
    {BAT_OK, battery_discharged, BAT_LOW, set_low},
		{-1, NULL, -1, NULL },
};

fsm_trans_t rent_transition_table[] = {
		{NOT_RENTED, available, RENTED, set_unlock },
    {RENTED, user_left, NOT_RENTED, set_lock},
		{-1, NULL, -1, NULL },
};

fsm_trans_t digit_transition_table[] = {
    {IDLE, pressed_digit, IDLE,  clean_increment}, 
    {IDLE, first_not_pressed, IDLE,  NULL},
    {IDLE, timeout_right, IDLE, right_value },
    {IDLE, timeout_wrong, IDLE, wrong_value },
	{-1, NULL, -1, NULL },
};

fsm_trans_t update_transition_table[] = {
    {IDLE, update_timeout, IDLE,  update_flags},
	{-1, NULL, -1, NULL },
};

/*
 * Como se puede observar el lenguaje del sistema en C esta contenido en el modelo de verificación.
 * Esto se consiguió gracias al modelado de las máquinas de estados mediante diagramas de bolas 
 * que permitieron su facil traducción a ambos modelos sin esfuerzo. De esta forma tanto máquinas de estados,
 * variables, funciones de condición, de salida y estados estan comparidos entre promela y C como puede comprobarse.
 *  Aunque eliminando la dependencia temporal en el modelo Promela para simplificar la verificación.
 */


//INIT FUNCTIONS
buffer_fsm_t* new_buffer_fsm(fsm_trans_t* buffer_transition_table){
	buffer_fsm_t* new_buffer_fsm = (buffer_fsm_t*) malloc(sizeof(buffer_fsm_t));

    if(new_buffer_fsm != NULL){
		new_buffer_fsm-> fsm.current_state = BUFFER;
		new_buffer_fsm-> fsm.tt = buffer_transition_table;//Herencia
    new_buffer_fsm -> next = xTaskGetTickCount () + TIMEOUT_BUFFER;
	}
	return new_buffer_fsm;
}
int delete_buffer_fsm(buffer_fsm_t* buffer_fsm){
	free(buffer_fsm);
	return 1;
}

send_fsm_t* new_send_fsm(fsm_trans_t* send_transition_table){
	send_fsm_t* new_send_fsm = (send_fsm_t*) malloc(sizeof(send_fsm_t));

    if(new_send_fsm != NULL){
		new_send_fsm-> fsm.current_state = SEND;
		new_send_fsm-> fsm.tt = send_transition_table;//Herencia
    new_send_fsm -> next = xTaskGetTickCount () + TIMEOUT_SEND;
	}
	return new_send_fsm;
}
int delete_send_fsm(send_fsm_t* send_fsm){
	free(send_fsm);
	return 1;
}

battery_fsm_t* new_battery_fsm(fsm_trans_t* battery_transition_table){
	battery_fsm_t* new_battery_fsm = (battery_fsm_t*) malloc(sizeof(battery_fsm_t));

    if(new_battery_fsm != NULL){
		new_battery_fsm-> fsm.current_state = BAT_LOW;
		new_battery_fsm-> fsm.tt = battery_transition_table;//Herencia
	}
	return new_battery_fsm;
}
int delete_battery_fsm(battery_fsm_t* battery_fsm){
	free(battery_fsm);
	return 1;
}

rent_fsm_t* new_rent_fsm(fsm_trans_t* rent_transition_table){
	rent_fsm_t* new_rent_fsm = (rent_fsm_t*) malloc(sizeof(rent_fsm_t));

    if(new_rent_fsm != NULL){
		new_rent_fsm-> fsm.current_state = NOT_RENTED;
		new_rent_fsm-> fsm.tt = rent_transition_table;//Herencia
	}
	return new_rent_fsm;
}
int delete_rent_fsm(rent_fsm_t* rent_fsm){
	free(rent_fsm);
	return 1;
}

digit_fsm_t* new_digit_fsm(fsm_trans_t* digit_transition_table){
	digit_fsm_t* new_digit_fsm = (digit_fsm_t*) malloc(sizeof(digit_fsm_t));

    if(new_digit_fsm != NULL){
		new_digit_fsm-> fsm.current_state = IDLE;
		new_digit_fsm-> fsm.tt = digit_transition_table;//Herencia
        new_digit_fsm -> timeout_time = xTaskGetTickCount () + TIMEOUT_MAX;
        new_digit_fsm -> digit = 1;	
        new_digit_fsm -> number = -1;
	}
    wrong_value((fsm_t*) new_digit_fsm);
	return new_digit_fsm;
}

int delete_digit_fsm(digit_fsm_t* digit_fsm){
	free(digit_fsm);
	return 1;
}

update_fsm_t* new_update_fsm(fsm_trans_t* update_transition_table){
	update_fsm_t* new_update_fsm = (update_fsm_t*) malloc(sizeof(update_fsm_t));

    if(new_update_fsm != NULL){
		new_update_fsm-> fsm.current_state = UPDATE;
		new_update_fsm-> fsm.tt = update_transition_table;//Herencia
    new_update_fsm -> timeout_update = xTaskGetTickCount () + TIMEOUT_UPDATE_FLAGS;
	}
	return new_update_fsm;
}
int delete_update_fsm(update_fsm_t* update_fsm){
	free(update_fsm);
	return 1;
}


//CONDITION FUNCTIONS
static int time_timeout(fsm_t* fsm){
  buffer_fsm_t* buffer_fsm = (buffer_fsm_t*) fsm;
  return xTaskGetTickCount () > buffer_fsm -> next;
}

static int timeout_empty(fsm_t* fsm){
  send_fsm_t* send_fsm = (send_fsm_t*) fsm;
  return xTaskGetTickCount () > send_fsm -> next && not_empty(buff) && connected;
}

static int battery_charged(fsm_t* fsm){
  return vbat > LIMIT;
}
static int battery_discharged(fsm_t* fsm){
  return vbat <= LIMIT;
}

static int available(fsm_t* fsm){
  return code_ok && !bat_low;
}
static int user_left(fsm_t* fsm){
  return code_ok;
}

static int first_not_pressed(fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    return digit_fsm->number == -1;
}

static int pressed_digit(fsm_t* fsm){
    digit_fsm_t* digit_fsm = (digit_fsm_t*) fsm;
    return pressed & !code_ok;
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

static int update_timeout(fsm_t* fsm){
  update_fsm_t* update_fsm = (update_fsm_t*) fsm;

  return xTaskGetTickCount () >= update_fsm->timeout_update;
}


//OUTPUT FUNCTIONS
static void push_buf(fsm_t* fsm){
  buffer_fsm_t* buffer_fsm = (buffer_fsm_t*) fsm;
  char msg [MAX_LENGTH];

  int time_stamp = xTaskGetTickCount()/portTICK_RATE_MS;
  buffer_fsm -> next = xTaskGetTickCount () + TIMEOUT_BUFFER;

  if (DEBUG) printf(" PUSH --> BATTERY: %d - %s, RENTED: %d \r\n", vbat, BATTERY[bat_low], !lock);
  //ID:Dato_sensor_1:Timestamp;
  sprintf(msg, "%d : %d mV (%s) : %s : %d", system_get_chip_id(), vbat, BATTERY[bat_low], RENT[!lock], time_stamp);
  
  push(buff, msg);
  
  if (DEBUG) printf("CONNECTED: %d\n", connected);
}

void send_msg(fsm_t* fsm){   
  char* msg = pop(buff);
  //ID:Dato_sensor_1:Timestamp;
  if (DEBUG) printf("POP --> %s\n", msg);
  espconn_send(&udp_client, msg, strlen(msg));

}

static void set_not_low(fsm_t* fsm){
  battery_fsm_t* battery_fsm = (battery_fsm_t*) fsm;
  if (DEBUG) printf("BATTERY OK\n");
  bat_low = 0;
}

static void set_low(fsm_t* fsm){
  battery_fsm_t* battery_fsm = (battery_fsm_t*) fsm;
  if (DEBUG) printf("BATTERY LOW\n");
  bat_low = 1;
}

static void set_unlock(fsm_t* fsm){
  rent_fsm_t* rent_fsm = (rent_fsm_t*) fsm;
  lock = 0;
  code_ok = 0;
}

static void set_lock(fsm_t* fsm){
  rent_fsm_t* rent_fsm = (rent_fsm_t*) fsm;
  lock = 1;
  code_ok = 0;
}

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
                    code_ok = !bat_low || !lock;
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

static void update_flags(fsm_t* fsm){
  update_fsm_t* update_fsm = (update_fsm_t*) fsm;

  vbat = system_adc_read();
  GPIO_OUTPUT_SET(BAT_FLAG, !bat_low);
  GPIO_OUTPUT_SET(LOW_FLAG, bat_low);
  GPIO_OUTPUT_SET(RENTED_FLAG, !lock);

  update_fsm->timeout_update = xTaskGetTickCount() + TIMEOUT_UPDATE_FLAGS;
}


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

buffer* buffer_init(void){
	buffer* new_buffer = (buffer*) malloc(sizeof(buffer));
	new_buffer->info = NULL;
	new_buffer->next = NULL;
	return new_buffer;
}

int push(buffer *buff, char* msg){
    buffer *p = malloc( sizeof(buffer) );
    int success = p != NULL;

    if (success)
    {
        p->info = msg;
        p->next = buff->next;
        buff->next = p;
    }

    return success;
}

char* pop(buffer *buff){
  buffer* aux = buff->next;
	buffer* prev = buff;

	if(aux != NULL){
		while (aux->next != NULL){
			prev = aux;
			aux = aux->next;
		}
		prev->next = NULL;
		return aux->info;
	}

	return NULL;
}

int not_empty( buffer *buff){
    int success = buff->next != NULL;  

    return success;
}

void isr_gpio(void* arg) {
  static portTickType xLastISRTick = REBOUND_TICK;

  uint32 status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);          //READ STATUS OF INTERRUPT

  portTickType now = xTaskGetTickCount ();
  if (status & BIT(BUTTON)) {
    if (now > xLastISRTick) {
      pressed = 1;
    }
    xLastISRTick = now + REBOUND_TICK;
  }

  //if (DEBUG) printf("in io intr: 0X%08x\r\n",status);                    //WRITE ON SERIAL UART0
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);       //CLEAR THE STATUS IN THE W1 INTERRUPT REGISTER
}

void gpio_init(){
  GPIO_ConfigTypeDef io_conf;
  io_conf.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;
  io_conf.GPIO_Mode = GPIO_Mode_Input;
  io_conf.GPIO_Pin = BIT(BUTTON);
  io_conf.GPIO_Pullup = GPIO_PullUp_EN;
  gpio_config(&io_conf);

  io_conf.GPIO_Mode = GPIO_Mode_Output;
  io_conf.GPIO_Pin = BIT(LOW_FLAG);
  io_conf.GPIO_Pullup = GPIO_PullUp_EN;
  gpio_config(&io_conf); 

  io_conf.GPIO_Mode = GPIO_Mode_Output;
  io_conf.GPIO_Pin = BIT(BAT_FLAG);
  io_conf.GPIO_Pullup = GPIO_PullUp_EN;
  gpio_config(&io_conf);

  io_conf.GPIO_Mode = GPIO_Mode_Output;
  io_conf.GPIO_Pin = BIT(RENTED_FLAG);
  io_conf.GPIO_Pullup = GPIO_PullUp_EN;
  gpio_config(&io_conf);
  
  PIN_FUNC_SELECT(GPIO_PIN_REG_12, FUNC_GPIO12);
  PIN_FUNC_SELECT(GPIO_PIN_REG_13, FUNC_GPIO13);
  PIN_FUNC_SELECT(GPIO_PIN_REG_14, FUNC_GPIO14);
  PIN_FUNC_SELECT(GPIO_PIN_REG_15, FUNC_GPIO15);

  gpio_intr_handler_register((void*)isr_gpio, NULL);
  gpio_pin_intr_state_set(BUTTON, GPIO_PIN_INTR_NEGEDGE);
  ETS_GPIO_INTR_ENABLE();
}

static inline int32_t asm_ccount(void) {
  int32_t r;
  asm volatile ("rsr %0, ccount" : "=r"(r));
  return r;
}

////////////////////////////////////////////////////////////////////////////////

void udpClient()
{

    static esp_udp udp;
    udp_client.type = ESPCONN_UDP;
    udp_client.proto.udp = &udp;
    udp.remote_port = UDP_SERVER_LOCAL_PORT;

    memcpy(udp.remote_ip, udp_server_ip, sizeof(udp_server_ip));
    uint8 i = 0;
    os_printf("serve ip:\n");
    for (i = 0; i <= 3; i++) {
        os_printf("%u.", udp_server_ip[i]);
    }
    os_printf("\n remote ip\n");
    for (i = 0; i <= 3; i++) {
        os_printf("%u.", udp.remote_ip[i]);
    }
    os_printf("\n");
    int8 res = 0;
    res = espconn_create(&udp_client);

}

void WifiConfig(void* arg)
{
    StaConectApConfig(SSID, PASSWORD);
    vTaskDelete(NULL);
}

////////////////////////////////////////////////////////////////////////////

void wifi_handle_event_cb(System_Event_t *evt)
{
    //printf("event %x\n", evt->event_id);
    switch (evt->event_id) {
        case EVENT_STAMODE_CONNECTED:
            printf("connect to ssid %s, channel %d\n", evt->event_info.connected.ssid,
                    evt->event_info.connected.channel);
            connected = 1;
            break;
        case EVENT_STAMODE_DISCONNECTED:
            printf("disconnect from ssid %s, reason %d\n", evt->event_info.disconnected.ssid,
                    evt->event_info.disconnected.reason);
            connected = 0;
            break;
        case EVENT_STAMODE_AUTHMODE_CHANGE:
            printf("mode: %d -> %d\n", evt->event_info.auth_change.old_mode, evt->event_info.auth_change.new_mode);
            break;
        case EVENT_STAMODE_GOT_IP:
            printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR, IP2STR(&evt->event_info.got_ip.ip),
                    IP2STR(&evt->event_info.got_ip.mask), IP2STR(&evt->event_info.got_ip.gw));
            printf("\n");
            break;
        case EVENT_SOFTAPMODE_STACONNECTED:
            printf("station: " MACSTR "join, AID = %d\n", MAC2STR(evt->event_info.sta_connected.mac),
                    evt->event_info.sta_connected.aid);
            break;
        case EVENT_SOFTAPMODE_STADISCONNECTED:
            printf("station: " MACSTR "leave, AID = %d\n", MAC2STR(evt->event_info.sta_disconnected.mac),
                    evt->event_info.sta_disconnected.aid);
            break;
        default:
            break;
    }
}

void conn_ap_init(void){
    wifi_set_opmode(STATIONAP_MODE);
    struct station_config config;
    memset(&config, 0, sizeof(config));  //set value of config from address of &config to width of size to be value '0'
    sprintf(config.ssid, SSID);
    sprintf(config.password, PASSWORD);
    wifi_station_set_config(&config);
    wifi_set_event_handler_cb(wifi_handle_event_cb);
    wifi_station_connect();
}


// MAIN
void task_patinete_digit(void* ignore){
  fsm_t* fsm_buffer;
  fsm_t* fsm_send;
  fsm_t* fsm_battery;
  fsm_t* fsm_rent;
  fsm_t* fsm_digit;
  fsm_t* fsm_update;
  int frame;
  portTickType xLastWakeTime; 

  conn_ap_init();
  udpClient();

  gpio_init();
  buff = buffer_init();

  fsm_buffer = (fsm_t*) new_buffer_fsm(buffer_transition_table);
  fsm_send = (fsm_t*) new_send_fsm(send_transition_table);
  fsm_battery = (fsm_t*) new_battery_fsm(battery_transition_table);
  fsm_rent = (fsm_t*) new_rent_fsm(rent_transition_table);
  fsm_digit = (fsm_t*) new_digit_fsm(digit_transition_table);
  fsm_update = (fsm_t*) new_update_fsm(update_transition_table);
  
  /* METODO MEDIDA TIEMPOS DE EJECUCIÓN (COSTE) DE LAS TAREAS
  int btime, etime;
  btime = asm_ccount();
    etime = asm_ccount();
    etime = (etime - btime);
    if (etime < 0) {
      etime += INT_MAX;
    }
    printf("%d\n", etime);
  */
 
  frame = 0;
  xLastWakeTime = xTaskGetTickCount();

  while(true) {
    switch (frame){
      case 0:
        fsm_fire(fsm_digit);
        fsm_fire(fsm_battery);
        fsm_fire(fsm_rent);
        fsm_fire(fsm_update);
        fsm_fire(fsm_buffer);
        fsm_fire(fsm_send);
        break;
      case 100:
      case 300:
      case 700:
      case 900:
        fsm_fire(fsm_digit);
        break;
      case 200:
      case 400:
      case 600:
      case 800:
        fsm_fire(fsm_digit);
        fsm_fire(fsm_battery);
        fsm_fire(fsm_rent);
        break;
		  case 500:
        fsm_fire(fsm_digit);
        fsm_fire(fsm_update);	
        break;
		  default:
			  break;
    }

    if (DEBUG) printf("Frame: %d\n", frame);

    frame = (SECONDARY + frame) % (HYPERPERIOD);

    vTaskDelayUntil(&xLastWakeTime, SECONDARY/portTICK_RATE_MS);    
  }

/* Hemos decidido emplear ejecutivo ciclico para llevar a cabo la planificación debido a que evita la exclusión
 * mutua por lo que los distintos procedimientos pueden compartir recursos y datos. Además, los periodos empleados 
 * son armónicos luego la planificación empleando ejecutivo ciclico será optima y correcta por construcción. 
 * Por otro lado, al no existir concurrencia en la ejecución nos ha sido más fácil depurar el sistema en busca 
 * de fallos. Finalmente, este método de planificación optimiza la memoria y el consumo, básico en un sistema 
 * empotrado como el planteado en el reto, donde debe estar largas horas sin acceso a carga para dar servicio.
 */

  delete_buffer_fsm((buffer_fsm_t*) fsm_buffer);
  delete_send_fsm((send_fsm_t*) fsm_send);
  delete_alarm_fsm((battery_fsm_t*) fsm_battery);
  delete_led_fsm ((rent_fsm_t*) fsm_rent);
  delete_digit_fsm((digit_fsm_t*) fsm_digit);
  delete_update_fsm((update_fsm_t*) fsm_update);
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
  xTaskHandle task;
  xTaskCreate(&task_patinete_digit, "fsm", 2048, NULL, 1, NULL);
}
