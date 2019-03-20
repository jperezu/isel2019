#define rand	pan_rand
#define pthread_equal(a,b)	((a)==(b))
#if defined(HAS_CODE) && defined(VERBOSE)
	#ifdef BFS_PAR
		bfs_printf("Pr: %d Tr: %d\n", II, t->forw);
	#else
		cpu_printf("Pr: %d Tr: %d\n", II, t->forw);
	#endif
#endif
	switch (t->forw) {
	default: Uerror("bad forward move");
	case 0:	/* if without executable clauses */
		continue;
	case 1: /* generic 'goto' or 'skip' */
		IfNotBlocked
		_m = 3; goto P999;
	case 2: /* generic 'else' */
		IfNotBlocked
		if (trpt->o_pm&1) continue;
		_m = 3; goto P999;

		 /* CLAIM spec */
	case 3: // STATE 1 - _spin_nvr.tmp:3 - [((!(!(((state==DISARMED)&&(button==1))))&&!((state==ARMED))))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported1 = 0;
			if (verbose && !reported1)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported1 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported1 = 0;
			if (verbose && !reported1)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported1 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][1] = 1;
		if (!(( !( !(((((int)now.state)==2)&&(((int)now.button)==1))))&& !((((int)now.state)==1)))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 4: // STATE 3 - _spin_nvr.tmp:4 - [((!(!(((state==ARMED)&&(presence==1))))&&!((alarm==1))))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported3 = 0;
			if (verbose && !reported3)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported3 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported3 = 0;
			if (verbose && !reported3)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported3 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][3] = 1;
		if (!(( !( !(((((int)now.state)==1)&&(((int)now.presence)==1))))&& !((((int)now.alarm)==1)))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 5: // STATE 5 - _spin_nvr.tmp:5 - [((!(!(((state==ARMED)&&(button==0))))&&!(((state==DISARMED)&&(alarm==0)))))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported5 = 0;
			if (verbose && !reported5)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported5 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported5 = 0;
			if (verbose && !reported5)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported5 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][5] = 1;
		if (!(( !( !(((((int)now.state)==1)&&(((int)now.button)==0))))&& !(((((int)now.state)==2)&&(((int)now.alarm)==0))))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 6: // STATE 12 - _spin_nvr.tmp:10 - [(!((state==ARMED)))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported12 = 0;
			if (verbose && !reported12)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported12 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported12 = 0;
			if (verbose && !reported12)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported12 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][12] = 1;
		if (!( !((((int)now.state)==1))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 7: // STATE 17 - _spin_nvr.tmp:14 - [(!((alarm==1)))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported17 = 0;
			if (verbose && !reported17)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported17 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported17 = 0;
			if (verbose && !reported17)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported17 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][17] = 1;
		if (!( !((((int)now.alarm)==1))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 8: // STATE 22 - _spin_nvr.tmp:18 - [(!(((state==DISARMED)&&(alarm==0))))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported22 = 0;
			if (verbose && !reported22)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported22 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported22 = 0;
			if (verbose && !reported22)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported22 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][22] = 1;
		if (!( !(((((int)now.state)==2)&&(((int)now.alarm)==0)))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 9: // STATE 27 - _spin_nvr.tmp:20 - [-end-] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported27 = 0;
			if (verbose && !reported27)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported27 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported27 = 0;
			if (verbose && !reported27)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported27 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][27] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC entorno */
	case 10: // STATE 1 - alarma.pml:30 - [button = 1] (0:0:1 - 1)
		IfNotBlocked
		reached[1][1] = 1;
		(trpt+1)->bup.oval = ((int)now.button);
		now.button = 1;
#ifdef VAR_RANGES
		logval("button", ((int)now.button));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 11: // STATE 2 - alarma.pml:31 - [button = 0] (0:0:1 - 1)
		IfNotBlocked
		reached[1][2] = 1;
		(trpt+1)->bup.oval = ((int)now.button);
		now.button = 0;
#ifdef VAR_RANGES
		logval("button", ((int)now.button));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 12: // STATE 5 - alarma.pml:34 - [presence = 1] (0:0:1 - 1)
		IfNotBlocked
		reached[1][5] = 1;
		(trpt+1)->bup.oval = ((int)now.presence);
		now.presence = 1;
#ifdef VAR_RANGES
		logval("presence", ((int)now.presence));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 13: // STATE 6 - alarma.pml:35 - [presence = 0] (0:0:1 - 1)
		IfNotBlocked
		reached[1][6] = 1;
		(trpt+1)->bup.oval = ((int)now.presence);
		now.presence = 0;
#ifdef VAR_RANGES
		logval("presence", ((int)now.presence));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 14: // STATE 12 - alarma.pml:39 - [-end-] (0:0:0 - 1)
		IfNotBlocked
		reached[1][12] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC alarm_fsm */
	case 15: // STATE 1 - alarma.pml:12 - [((state==DISARMED))] (0:0:0 - 1)
		IfNotBlocked
		reached[0][1] = 1;
		if (!((((int)now.state)==2)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 16: // STATE 2 - alarma.pml:14 - [(button)] (17:0:1 - 1)
		IfNotBlocked
		reached[0][2] = 1;
		if (!(((int)now.button)))
			continue;
		/* merge: state = ARMED(17, 3, 17) */
		reached[0][3] = 1;
		(trpt+1)->bup.oval = ((int)now.state);
		now.state = 1;
#ifdef VAR_RANGES
		logval("state", ((int)now.state));
#endif
		;
		/* merge: .(goto)(17, 5, 17) */
		reached[0][5] = 1;
		;
		/* merge: .(goto)(0, 18, 17) */
		reached[0][18] = 1;
		;
		_m = 3; goto P999; /* 3 */
	case 17: // STATE 7 - alarma.pml:17 - [((state==ARMED))] (0:0:0 - 1)
		IfNotBlocked
		reached[0][7] = 1;
		if (!((((int)now.state)==1)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 18: // STATE 8 - alarma.pml:19 - [(!(button))] (16:0:2 - 1)
		IfNotBlocked
		reached[0][8] = 1;
		if (!( !(((int)now.button))))
			continue;
		/* merge: state = DISARMED(16, 9, 16) */
		reached[0][9] = 1;
		(trpt+1)->bup.ovals = grab_ints(2);
		(trpt+1)->bup.ovals[0] = ((int)now.state);
		now.state = 2;
#ifdef VAR_RANGES
		logval("state", ((int)now.state));
#endif
		;
		/* merge: alarm = 0(16, 10, 16) */
		reached[0][10] = 1;
		(trpt+1)->bup.ovals[1] = ((int)now.alarm);
		now.alarm = 0;
#ifdef VAR_RANGES
		logval("alarm", ((int)now.alarm));
#endif
		;
		/* merge: .(goto)(16, 14, 16) */
		reached[0][14] = 1;
		;
		_m = 3; goto P999; /* 3 */
	case 19: // STATE 14 - alarma.pml:22 - [.(goto)] (0:16:0 - 2)
		IfNotBlocked
		reached[0][14] = 1;
		;
		_m = 3; goto P999; /* 0 */
	case 20: // STATE 11 - alarma.pml:20 - [(presence)] (16:0:1 - 1)
		IfNotBlocked
		reached[0][11] = 1;
		if (!(((int)now.presence)))
			continue;
		/* merge: alarm = 1(16, 12, 16) */
		reached[0][12] = 1;
		(trpt+1)->bup.oval = ((int)now.alarm);
		now.alarm = 1;
#ifdef VAR_RANGES
		logval("alarm", ((int)now.alarm));
#endif
		;
		/* merge: .(goto)(16, 14, 16) */
		reached[0][14] = 1;
		;
		_m = 3; goto P999; /* 2 */
	case 21: // STATE 16 - alarma.pml:23 - [printf('Estado: %e, Button: %d, Presence: %d, Alarm: %d \\n',state,button,presence,alarm)] (0:0:0 - 1)
		IfNotBlocked
		reached[0][16] = 1;
		Printf("Estado: %e, Button: %d, Presence: %d, Alarm: %d \n", ((int)now.state), ((int)now.button), ((int)now.presence), ((int)now.alarm));
		_m = 3; goto P999; /* 0 */
	case 22: // STATE 20 - alarma.pml:25 - [-end-] (0:0:0 - 1)
		IfNotBlocked
		reached[0][20] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */
	case  _T5:	/* np_ */
		if (!((!(trpt->o_pm&4) && !(trpt->tau&128))))
			continue;
		/* else fall through */
	case  _T2:	/* true */
		_m = 3; goto P999;
#undef rand
	}

