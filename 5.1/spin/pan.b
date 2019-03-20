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
		
	case 7: // STATE 20
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC entorno */

	case 8: // STATE 1
		;
		now.button = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 10: // STATE 7
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC fsm_temporizado */
;
		;
		
	case 12: // STATE 5
		;
		now.state = trpt->bup.ovals[1];
		now.button = trpt->bup.ovals[0];
		;
		ungrab_ints(trpt->bup.ovals, 2);
		goto R999;
;
		;
		
	case 14: // STATE 13
		;
		now.state = trpt->bup.ovals[1];
		now.button = trpt->bup.ovals[0];
		;
		ungrab_ints(trpt->bup.ovals, 2);
		goto R999;
;
		
	case 15: // STATE 16
		goto R999;
;
		
	case 16: // STATE 14
		goto R999;

	case 17: // STATE 21
		;
		p_restor(II);
		;
		;
		goto R999;
	}

