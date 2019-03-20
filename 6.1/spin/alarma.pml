ltl spec {
	[] (((state == DISARMED) && (button == 1)) -> <> (state == ARMED)) &&
	[] (((state == ARMED) && (presence == 1)) -> <> (alarm == 1)) &&
	[] (((state == ARMED) && (button == 0)) -> <> ((state == DISARMED) && (alarm == 0)))
}


mtype {DISARMED, ARMED}
bit button;
bit presence;
bit alarm;
byte state = DISARMED;

active proctype alarm_fsm() {
	do
	:: (state == DISARMED) -> atomic {
								if
								:: button -> state = ARMED
								fi
							}
	:: (state == ARMED) -> atomic {
								if
								:: !button -> state = DISARMED; alarm = 0
								:: presence -> alarm = 1
								fi
							}
	printf("Estado: %e, Button: %d, Presence: %d, Alarm: %d \n", state, button, presence, alarm)
	od
}

active proctype entorno() {
	do
		::if
  		:: button = 1
  		:: button = 0
  		fi
  		::if
  		:: presence = 1
		:: presence = 0
  		fi 
	od

}