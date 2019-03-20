	switch (t->back) {
	default: Uerror("bad return move");
	case  0: goto R999; /* nothing to undo */

		 /* CLAIM spec */
;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		;
		
	case 9: // STATE 27
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC entorno */

	case 10: // STATE 1
		;
		now.button = trpt->bup.oval;
		;
		goto R999;

	case 11: // STATE 2
		;
		now.button = trpt->bup.oval;
		;
		goto R999;

	case 12: // STATE 5
		;
		now.presence = trpt->bup.oval;
		;
		goto R999;

	case 13: // STATE 6
		;
		now.presence = trpt->bup.oval;
		;
		goto R999;

	case 14: // STATE 12
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC alarm_fsm */
;
		;
		
	case 16: // STATE 3
		;
		now.state = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 18: // STATE 10
		;
		now.alarm = trpt->bup.ovals[1];
		now.state = trpt->bup.ovals[0];
		;
		ungrab_ints(trpt->bup.ovals, 2);
		goto R999;
;
		
	case 19: // STATE 14
		goto R999;

	case 20: // STATE 12
		;
		now.alarm = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 22: // STATE 20
		;
		p_restor(II);
		;
		;
		goto R999;
	}

