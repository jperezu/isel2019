ltl spec_state {
	[] (((state == DISARMED) &&  correct) -> <> (state == ARMED)) &&
	[] (((state == ARMED) && correct) -> <> (state == DISARMED))
}

ltl spec_alarm_on {
	[] (((state == ARMED) && ((!correct) W (alarm == 1)) && presence_alarm) -> <> (alarm == 1))
}

ltl spec_alarm_off {
	[] (((state == ARMED) && correct) -> (<> (alarm == 0)))
}

ltl spec_code {
	[] ((index != 3) -> (<> (correct == 0))) &&
	[] ((index == 3) -> (<> (correct == 1)))
}

ltl spec_light {
	[] (((state_light == OFF) && (button || presence)) -> <> (state_light  == ON)) &&
	[] (((state_light == ON)) -> (<> (state_light  == OFF)))
}

#define timeout true

mtype {OFF, ON, DIGIT, DISARMED, ARMED}

bit button
bit button_digit
bit presence
bit presence_alarm

bit alarm
bit correct

byte index
byte digit[4]
byte password[4] = {1, 1, 1, 0}

mtype state_light
mtype state_password
mtype state

active proctype fsm_temporizado() {
	state_light = OFF
	do
	:: (state_light == OFF) ->  atomic {
							if
							:: (button || presence) -> button = 0; presence = 0; state_light = ON; printf("LIGHT ON\n")
							fi
						}
	:: (state_light == ON) ->  atomic {
							if
							:: state_light = OFF; printf("LIGHT OFF\n")
							fi
						}
	od
}

active proctype digits_fsm() {
	state_password = DIGIT
	do
	:: (state_password == DIGIT) -> atomic {
										if
										:: (button_digit && (digit[index] < 9) && (index != 3)) -> digit[index] = digit[index] + 1; button_digit = 0
										:: (button_digit && (digit[index] >= 9) && (index != 3)) -> digit[index] = 0; button_digit = 0
										:: (!button_digit && timeout && (digit[index] == password[index]) && (index != 3)) -> index = index + 1; 
										:: (!button_digit && timeout && (digit[index] != password[index]) && (index != 3)) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index = 0
										:: (index == 3) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index = 0; correct = 1; printf("CODE CORRECT\n")
										fi
									}
	od
}

active proctype alarm_fsm(){
	state = DISARMED
	do
	:: (state == DISARMED) -> atomic {
								if
								:: correct -> correct = 0; presence_alarm = 0; state = ARMED
								:: !correct -> skip
								fi
							}
							
	:: (state == ARMED) -> atomic {
								if
								:: correct -> correct = 0; alarm = 0; presence_alarm = 0; state = DISARMED
								:: !correct -> alarm = presence_alarm; printf("ALARM ON\n", presence_alarm, alarm)
								fi
							}
	od
}

active proctype entorno() {
	do
		::if
  		:: button = 1
  		:: button_digit = 1
  		:: presence = 1; presence_alarm = 1
		:: skip
  		fi 
  		printf("Light: %e, State: %e, Password: %d %d %d, Correct: %d, Button: %d, Presence: %d Presence_alarm: %d, Alarm: %d \n",state_light, state, digit[0], digit[1], digit[2], correct, button, presence, presence_alarm, alarm)
	od

}