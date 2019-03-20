
ltl spec {
	[] (((state == OFF) && button) -> <> (state  == ON)) &&
	[] (((state == ON) && timeout) -> <> (state  == OFF))
}

#define timeout true
bit button;
mtype {OFF, ON}
byte state = OFF;

active proctype fsm_temporizado() {
	do
	:: (state == OFF) ->  atomic {
							if
							:: button -> button = 0; state = ON
							fi
						}
	:: (state == ON) ->  atomic {
							if
							:: timeout -> button = 0; state = OFF
							:: skip
							fi
						}
	od
}

active proctype entorno() {
	do
	:: if
	:: button = 1
	:: (! button) -> skip
	:: fi
	:: printf("State: %e, button: %d -> led on\n", state, button)
	od
}