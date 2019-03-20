ltl spec_alarm {
	[] (((state == DISARMED) &&  correct) -> <> (state == ARMED)) &&
	[] (((state == ARMED) && correct) -> <> (state == DISARMED)) &&
	[] (((state == ARMED) && !correct && presence_alarm) -> <> (alarm == 1)) &&
	[] ((state == DISARMED) -> <> (alarm == 0))
}

ltl spec_code {
	[] (((index != 2) || ((index == 2) && (digit[index] != password[index]))) -> (<> (correct == 0))) &&
	[] (((index == 2) && (digit[index] == password[index])) -> (<> (correct == 1)))
}

ltl spec_light {
	[] (((state_light == OFF) && (button || presence)) -> <> (state_light  == ON)) &&
	[] (((state_light == ON) && timeout) -> <> (state_light  == OFF))
}

#define timeout true

mtype {OFF, ON, DIGIT, DISARMED, ARMED}

byte deadline
byte T = 10

bit button
bit presence
bit presence_alarm

bit alarm
bit correct

byte index
byte digit[3]
byte password[3] = {1, 1, 1}

mtype state_light
mtype state_password
mtype state

active proctype fsm_temporizado() {
	state_light = OFF
	do
	:: (state_light == OFF) ->  atomic {
							if
							:: (button || presence) -> button = 0; presence_alarm = presence_alarm | presence; presence = 0; state_light = ON
							fi
						}
	:: (state_light == ON) ->  atomic {
							if
							:: presence -> presence_alarm = 1; presence = 0; deadline = deadline + T
							:: (!presence && timeout) -> state_light = OFF
							fi
						}
	od
}

active proctype digits_fsm() {
	state_password = DIGIT
	do
	:: (state_password == DIGIT) -> atomic {
										if
										:: (button && (digit[index] < 9)) -> digit[index] = digit[index] + 1; button = 0
										:: (button && (digit[index] >= 9)) -> digit[index] = 0; button = 0
										:: (timeout && (digit[index] == password[index]) && (index != 2)) -> index = index + 1; 
										:: (timeout && (digit[index] == password[index]) && (index == 2)) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index = 0; correct = 1; printf("CODE CORRECT\n")
										:: (timeout && (digit[index] != password[index])) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index = 0
										:: else -> printf("ERROR\n")
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
								fi
							}
							
	:: (state == ARMED) -> atomic {
								if
								:: correct -> correct = 0; alarm = 0; presence_alarm = 0; state = DISARMED
								:: !correct -> alarm = presence_alarm; printf("Presence: %d, Alarm: %d -> ALARM ON\n", presence_alarm, alarm)
								:: else -> printf("ERROR\n")
								fi
							}
	od
}

active proctype entorno() {
	do
		::if
  		:: button = 1
  		:: presence = 1
		:: skip
  		fi 
  		printf("State: %e, Password: %d %d %d, Correct: %d, Presence: %d Presence_alarm: %d, Alarm: %d \n", state, digit[0], digit[1], digit[2], correct, presence, presence_alarm, alarm)
	od

}