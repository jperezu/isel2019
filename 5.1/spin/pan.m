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
	case 3: // STATE 1 - _spin_nvr.tmp:3 - [((!(!(((state==OFF)&&button)))&&!((state==ON))))] (0:0:0 - 1)
		
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
		if (!(( !( !(((((int)now.state)==2)&&((int)now.button))))&& !((((int)now.state)==1)))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 4: // STATE 3 - _spin_nvr.tmp:4 - [((!(!(((state==ON)&&timeout)))&&!((state==OFF))))] (0:0:0 - 1)
		
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
		if (!(( !( !(((((int)now.state)==1)&&((trpt->tau)&1))))&& !((((int)now.state)==2)))))
			continue;
		_m = 1; goto P999; /* 0 */
	case 5: // STATE 10 - _spin_nvr.tmp:9 - [(!((state==ON)))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported10 = 0;
			if (verbose && !reported10)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported10 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported10 = 0;
			if (verbose && !reported10)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported10 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][10] = 1;
		if (!( !((((int)now.state)==1))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 6: // STATE 15 - _spin_nvr.tmp:13 - [(!((state==OFF)))] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported15 = 0;
			if (verbose && !reported15)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported15 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported15 = 0;
			if (verbose && !reported15)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported15 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][15] = 1;
		if (!( !((((int)now.state)==2))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 7: // STATE 20 - _spin_nvr.tmp:15 - [-end-] (0:0:0 - 1)
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported20 = 0;
			if (verbose && !reported20)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported20 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported20 = 0;
			if (verbose && !reported20)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported20 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[2][20] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC entorno */
	case 8: // STATE 1 - temporizado.pml:27 - [button = 1] (0:0:1 - 1)
		IfNotBlocked
		reached[1][1] = 1;
		(trpt+1)->bup.oval = ((int)now.button);
		now.button = 1;
#ifdef VAR_RANGES
		logval("button", ((int)now.button));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 9: // STATE 2 - temporizado.pml:28 - [(!(button))] (0:0:0 - 1)
		IfNotBlocked
		reached[1][2] = 1;
		if (!( !(((int)now.button))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 10: // STATE 7 - temporizado.pml:30 - [-end-] (0:0:0 - 1)
		IfNotBlocked
		reached[1][7] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC fsm_temporizado */
	case 11: // STATE 1 - temporizado.pml:11 - [((state==OFF))] (0:0:0 - 1)
		IfNotBlocked
		reached[0][1] = 1;
		if (!((((int)now.state)==2)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 12: // STATE 2 - temporizado.pml:13 - [(button)] (18:0:2 - 1)
		IfNotBlocked
		reached[0][2] = 1;
		if (!(((int)now.button)))
			continue;
		/* merge: printf('State: %e, button: %d -> led on\\n',state,button)(18, 3, 18) */
		reached[0][3] = 1;
		Printf("State: %e, button: %d -> led on\n", ((int)now.state), ((int)now.button));
		/* merge: button = 0(18, 4, 18) */
		reached[0][4] = 1;
		(trpt+1)->bup.ovals = grab_ints(2);
		(trpt+1)->bup.ovals[0] = ((int)now.button);
		now.button = 0;
#ifdef VAR_RANGES
		logval("button", ((int)now.button));
#endif
		;
		/* merge: state = ON(18, 5, 18) */
		reached[0][5] = 1;
		(trpt+1)->bup.ovals[1] = ((int)now.state);
		now.state = 1;
#ifdef VAR_RANGES
		logval("state", ((int)now.state));
#endif
		;
		/* merge: .(goto)(18, 7, 18) */
		reached[0][7] = 1;
		;
		/* merge: .(goto)(0, 19, 18) */
		reached[0][19] = 1;
		;
		_m = 3; goto P999; /* 5 */
	case 13: // STATE 9 - temporizado.pml:16 - [((state==ON))] (0:0:0 - 1)
		IfNotBlocked
		reached[0][9] = 1;
		if (!((((int)now.state)==1)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 14: // STATE 10 - temporizado.pml:18 - [(1)] (18:0:2 - 1)
		IfNotBlocked
		reached[0][10] = 1;
		if (!(1))
			continue;
		/* merge: printf('State: %e, Timeout -> led off\\n',state)(18, 11, 18) */
		reached[0][11] = 1;
		Printf("State: %e, Timeout -> led off\n", ((int)now.state));
		/* merge: button = 0(18, 12, 18) */
		reached[0][12] = 1;
		(trpt+1)->bup.ovals = grab_ints(2);
		(trpt+1)->bup.ovals[0] = ((int)now.button);
		now.button = 0;
#ifdef VAR_RANGES
		logval("button", ((int)now.button));
#endif
		;
		/* merge: state = OFF(18, 13, 18) */
		reached[0][13] = 1;
		(trpt+1)->bup.ovals[1] = ((int)now.state);
		now.state = 2;
#ifdef VAR_RANGES
		logval("state", ((int)now.state));
#endif
		;
		/* merge: .(goto)(18, 16, 18) */
		reached[0][16] = 1;
		;
		/* merge: .(goto)(0, 19, 18) */
		reached[0][19] = 1;
		;
		_m = 3; goto P999; /* 5 */
	case 15: // STATE 16 - temporizado.pml:21 - [.(goto)] (0:18:0 - 2)
		IfNotBlocked
		reached[0][16] = 1;
		;
		/* merge: .(goto)(0, 19, 18) */
		reached[0][19] = 1;
		;
		_m = 3; goto P999; /* 1 */
	case 16: // STATE 14 - temporizado.pml:19 - [(1)] (18:0:0 - 1)
		IfNotBlocked
		reached[0][14] = 1;
		if (!(1))
			continue;
		/* merge: .(goto)(18, 16, 18) */
		reached[0][16] = 1;
		;
		/* merge: .(goto)(0, 19, 18) */
		reached[0][19] = 1;
		;
		_m = 3; goto P999; /* 2 */
	case 17: // STATE 21 - temporizado.pml:23 - [-end-] (0:0:0 - 1)
		IfNotBlocked
		reached[0][21] = 1;
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

