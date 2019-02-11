/*
 * Copyright (c) 2017, Universidad Politecnica de Madrid - B105 Electronic Systems Lab
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the B105 Electronic Systems Lab.
 * 4. Neither the name of the B105 Electronic Systems Lab nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY AND CONTRIBUTORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * fsm.h
 *
 *  Created on: 24 de abr. de 2017
 *      Author: Roberto Rodriguez-Zurrunero  <r.rodriguezz@b105.upm.es>
 *
 */
/**
 * @file fsm.h
 */
#ifndef FSM_H_
#define FSM_H_

typedef struct fsm_ fsm_t;

typedef int (*fsm_input_func_t) (fsm_t*);
typedef void (*fsm_output_func_t) (fsm_t*);


typedef struct fsm_trans_ {
	int orig_state;
	fsm_input_func_t in;
	int dest_state;
	fsm_output_func_t out;
} fsm_trans_t;

struct fsm_ {
	int current_state;
	fsm_trans_t* tt;
};

fsm_t* fsm_new (fsm_trans_t* tt);
int fsm_delete(fsm_t* fsm);

void fsm_fire (fsm_t* fsm);


#endif /* FSM_H_ */
