#define timeout true
#define LIMIT 10

//SPECIFICACIONES PARA EL PLANTEAMIENTO CON TAMAÑO DE BUFFER
/*
ltl spec_buffer {
	([](<>timeout) ->[](<> (buffer_size == buffer_size_temp + 1)))
}

ltl spec_send {
	[] ((state_send == SEND && (buffer_size > 0) && connected)-> <> (buffer_size == buffer_size_temp - 1))
}
*/

ltl spec_buffer {
	([](timeout) ->[](<>msg_push))
}

ltl spec_send {
	([](!buffer_empty && connected)-> [](<>msg_send)) &&
	([](buffer_empty || !connected)-> [](<>!msg_send))
}

//DESCOMENTAR PARA COMPROBAR FSM_DIGIT
/*
ltl spec_code {
	[] ((index_code != 3) -> (<> (code_ok == 0))) &&
	[] ((index_code == 3) -> (<> (code_ok == 1)))
}
*/

ltl spec_lowbat {
	[] ((Vbat <= LIMIT) -> <>(lock W (state_rent == RENTED)))
}

ltl spec_rent {
	[] (((Vbat > LIMIT) && code_ok) -> <> (!lock W ((Vbat < LIMIT && !code_ok) || !code_ok)))
}


ltl spec_red_led {
	[] ((Vbat <= LIMIT) -> <>(red_led W (Vbat > LIMIT))) &&
	[] ((Vbat > LIMIT) -> <>(!red_led W (Vbat <= LIMIT))) 
}

ltl spec_green_led {
	[] ((Vbat > LIMIT) -> <>(green_led W (Vbat <= LIMIT))) &&
	[] ((Vbat <= LIMIT) -> <>(!green_led W (Vbat > LIMIT))) 
}

ltl spec_blue_led {
	[] ((!lock) -> <>(blue_led W lock)) &&
	[] ((lock) -> <>(!blue_led W !lock))
}



mtype {DIGIT, BUFFER, SEND, BAT_LOW, BAT_OK, RENTED, NOT_RENTED, UPDATE}


byte Vbat = 0;
bit bat_low = 1;
bit lock = 1;

bit red_led = 0;
bit green_led = 0;
bit blue_led = 0;

bit buffer_empty = 1;
bit msg_push = 0;
bit msg_send = 0;
bit connected = 0;

bit button_digit;
bit code_ok = 0;
byte index_code
byte digit[4]
byte password[4] = {1, 1, 1, 0}

mtype state_battery;
mtype state_send;
mtype state_buffer;
mtype state_rent;
mtype state_flags;
mtype state_password;

// Funcionamieto de la FSM de control de digitos comentado para reducir el peso en memoria
// En caso de querer verificar su comportamiento descomentar su LTL correspondiente y eliminar code_ok = 1 del entorno
/*
active proctype digits_fsm() {
	state_password = DIGIT
	do
	:: (state_password == DIGIT) -> atomic {
										if
										:: (button_digit && (digit[index_code] < 9) && (index_code != 3)) -> digit[index_code] = digit[index_code] + 1; button_digit = 0
										:: (button_digit && (digit[index_code] >= 9) && (index_code != 3)) -> digit[index_code] = 0; button_digit = 0
										:: (!button_digit && timeout && (digit[index_code] == password[index_code]) && (index_code != 3)) -> index_code = index_code + 1; 
										:: (!button_digit && timeout && (digit[index_code] != password[index_code]) && (index_code != 3)) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index_code = 0
										:: (index_code == 3) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index_code = 0; code_ok = 1; printf("CODE CORRECT\n")
										fi
									}
	od
}
*/

// Planteamiento original para buffer_size_temp = buffer_size; buffer_size = buffer_size + 1; printf("Msg pushed\n");
// Esto se ha sustituido para disminuir el numero de estados en memoria y agilizar la verificación
active proctype buffer_fsm(){
	state_buffer = BUFFER
	do
	:: (state_buffer == BUFFER) ->  atomic {
							if
							:: timeout -> msg_push = 1; buffer_empty = 0; printf("Msg pushed\n");
							fi
						}	
	od
}

// Planteamiento original para buffer_size_temp = buffer_size; buffer_size = buffer_size - 1; printf("Msg send\n");
// Esto se ha sustituido para disminuir el numero de estados en memoria y agilizar la verificación
active proctype send_fsm(){
	state_send = SEND
	do
	:: (state_send == SEND) ->  atomic {
							if
							:: (!buffer_empty && connected) -> msg_send = 1; buffer_empty = 0; printf("Msg send\n");
							:: (!buffer_empty && connected) -> msg_send = 1; buffer_empty = 1; printf("Msg send\n");
							:: else -> msg_send = 0;
							fi
						}
	od
	
}

active proctype fsm_battery() {
	state_battery = BAT_LOW
	do
	:: (state_battery == BAT_LOW) ->  atomic {
							if
							:: (Vbat > LIMIT) -> state_battery = BAT_OK; bat_low = 0; printf("BATTERY OK\n")
							fi
						}
	:: (state_battery == BAT_OK) ->  atomic {
							if
							:: (Vbat <= LIMIT)  -> state_battery = BAT_LOW; bat_low = 1; printf("BATTERY LOW\n")
							fi
						}
	od
}

active proctype fsm_rent() {
	state_rent = NOT_RENTED
	do
	:: (state_rent == NOT_RENTED) ->  atomic {
							if
							:: (code_ok && !bat_low) -> state_rent = RENTED; lock = 0; printf("code_ok: %d, Unlocked\n", code_ok); code_ok = 0
							:: else -> code_ok = 0
							fi
						}
	:: (state_rent == RENTED) ->  atomic {
							if
							:: code_ok  -> state_rent = NOT_RENTED; lock = 1; code_ok = 0; printf("Locked\n")
							fi
						}
	od
}
// Descomentar para probar UPDATE de flags, Spin deja un máximo de solo 6 procesos si no se compila con DNFAIR=3
active proctype fsm_update() {
	state_flags = UPDATE
	do
	:: (state_flags == UPDATE) ->  atomic {
							if
							:: timeout -> red_led = bat_low; green_led = !bat_low; blue_led = !lock;
							fi
						}
	od
}


active proctype entorno() {
	do
		::if
			:: code_ok = 1;
			:: connected = 1;
			:: connected = 0;
		    :: button_digit = 1;
		    :: Vbat > 0 -> Vbat = Vbat - 1;
		    :: Vbat < 255 -> Vbat = Vbat + 1;
		    :: msg_push = 0;
			:: skip
  		fi
  		//printf("Connected: %d, Empty: %d, msg_send: %d\n", connected, buffer_empty, msg_send)
  		printf("R: %d G: %d B: %d, %e/%e -> code_ok: %d Vbat:%d bat_low: %d lock: %d\n",red_led, green_led, blue_led, state_battery, state_rent, code_ok, Vbat, bat_low, lock)
	od
}
