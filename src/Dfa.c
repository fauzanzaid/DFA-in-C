#include <Dfa.h>

/////////////////////
// Data Structures //
/////////////////////

typedef struct DfaTransition
{
	
} DfaTransition;

typedef struct Dfa{
	
	// Parameters

	int *states;
	int len_states;

	char *symbols;
	int len_symbols;

	int start_state;

	int *final_states;
	int len_final_states;


	// State handling
	
	int state_cur;
	int symbol_counter;

	int state_last_final_valid;
	int state_last_final;
	int symbol_counter_last_final;

} Dfa;

//////////////////////////////////
// Constructors and Destructors //
//////////////////////////////////

Dfa *Dfa_new(int *states, int len_states, char *symbols, int len_symbols, int start_state, int *final_states, int len_final_states){

}

void Dfa_destroy(Dfa *dfa_ptr){

}

/////////////////////
// Add transitions //
/////////////////////

void Dfa_add_transition_single(Dfa *dfa_ptr, int from_state, int to_state, char symbol){

}

void Dfa_add_transition_single_invert(Dfa *dfa_ptr, int from_state, int to_state, char symbol){

}

void Dfa_add_transition_many(Dfa *dfa_ptr, int from_state, int to_state, char symbols, int len_symbols){

}

void Dfa_add_transition_many_invert(Dfa *dfa_ptr, int from_state, int to_state, char symbols, int len_symbols){

}

void Dfa_add_transition_custom(Dfa *dfa_ptr, int from_state, int to_state, int (*check_function)(char)){

}

void Dfa_add_transition_regex(Dfa *dfa_ptr, int from_state, int to_state, char *pattern){

}

/////////////
// Run DFA //
/////////////

int Dfa_step(Dfa *dfa_ptr, char c){

}

int Dfa_run(Dfa *dfa_ptr, char* input, int len_input, int count){

}

int Dfa_retract(Dfa *dfa_ptr){

}

void Dfa_reset_state(Dfa *dfa_ptr){

}

void Dfa_reset_counter(Dfa *dfa_ptr){

}

void Dfa_get_current_configuration(Dfa *dfa_ptr, int *state_ptr, int *state_type_ptr, int *counter_ptr){

}
