ltl spec1 {
	[] (((state == DISARMED) &&  correct) -> <> (state == ARMED)) &&
	[] (((state == ARMED) && correct) -> <> ((state == DISARMED) && (alarm == 0)))
}

ltl spec2 {
	[] (((state == ARMED) && !correct && presence) -> <> (alarm == 1)) &&
	[] ((state == DISARMED) -> <> (alarm == 0))
}

ltl spec3 {
	[] (((index != 2) || ((index == 2) && (digit[index] != password[index]))) -> (<> (correct == 0))) &&
	[] ((!button && ((index == 2) && (digit[index] == password[index]))) -> (<> (correct == 1)))
}

#define timeout true

mtype {DISARMED, ARMED}
mtype {DIGIT}


bit button;
bit presence;

bit alarm = 0;
bit correct = 0;

byte index = 0;
byte digit[3];
byte password[3] = {1, 1, 1};

mtype state_light;
mtype state_password;
mtype state;

active proctype digits_fsm() {
	state_password = DIGIT;
	do
	:: (state_password == DIGIT) -> atomic {
										if
										:: (button && !correct) -> digit[index] = (digit[index] + 1) %  10; button = 0
										:: (!button && timeout && (digit[index] == password[index]) && (index < 2)) -> index = index + 1
										:: (!button && timeout && (digit[index] == password[index]) && (index == 2)) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index = 0; correct = 1
										:: (!button && timeout && (digit[index] != password[index])) -> digit[0] = 0; digit[1] = 0; digit[2] = 0; index = 0
										fi
									}
	od
}

active proctype alarm_fsm(){
	state = DISARMED;
	do
	:: (state == DISARMED) -> atomic {
								if
								:: correct -> correct = 0; presence = 0; state = ARMED
								fi
							}
							
	:: (state == ARMED) -> atomic {
								if
								:: correct -> correct = 0; alarm = 0; presence = 0; state = DISARMED
								:: (!correct && presence) -> alarm = 1;
								fi
							}
	od
}

active proctype entorno() {
	do
		::if
  		:: button = 1
  		:: presence = 1
		:: (!button || !presence) -> skip
  		fi 
  		printf("State: %e, Password: %d %d %d, Button: %d, index: %d, Correct: %d, Presence: %d, Alarm: %d \n", state, digit[0], digit[1], digit[2], button, index, correct, presence, alarm)
	od

}