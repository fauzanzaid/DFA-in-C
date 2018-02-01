#include <stdlib.h>

#include "Dfa.h"
#include "HashTable.h"
#include "LinkedList.h"



///////////////
// Constants //
///////////////

static const int STATE_CLASS_NONFINAL = 0;
static const int STATE_CLASS_FINAL = 1;


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

	HashTable *state_class_table;
	HashTable *transition_table;

	// State handling
	
	int state_cur;
	int symbol_counter;

	int state_last_final_valid;
	int state_last_final;
	int symbol_counter_last_final;

} Dfa;


/////////////////////////////////
// Private Function Prototypes //
/////////////////////////////////

static int hash_function(void *key);
static int key_compare(void *key1, void *key2);


//////////////////////////////////
// Constructors and Destructors //
//////////////////////////////////

Dfa *Dfa_new(int *states, int len_states, char *symbols, int len_symbols, int start_state, int *final_states, int len_final_states){

	// Allocate

	Dfa *dfa_ptr = malloc( sizeof(Dfa) );


	// Copy parameters

	dfa_ptr->states = states;
	dfa_ptr->len_states = len_states;

	dfa_ptr->symbols = symbols;
	dfa_ptr->len_symbols = len_symbols;

	dfa_ptr->start_state = start_state;

	dfa_ptr->final_states = final_states;
	dfa_ptr->len_final_states = len_final_states;


	// Init state class table

	HashTable *state_class_table = HashTable_new(len_states, hash_function, key_compare);

	for (int i = 0; i < len_final_states; ++i){
		// Add all final states
		HashTable_add(state_class_table, (void *)&final_states[i], (void *)&STATE_CLASS_FINAL);
	}

	for (int i = 0; i < len_states; ++i){
		// Check if state is not final
		if(HashTable_get(state_class_table, &states[i]) != NULL){
			HashTable_add(state_class_table, (void *)&states[i], (void *)&STATE_CLASS_NONFINAL);
		}
	}

	dfa_ptr->state_class_table = state_class_table;


	// Init transition table

	dfa_ptr->transition_table = HashTable_new(len_states, hash_function, key_compare);


	// Init state

	dfa_ptr->state_cur = dfa_ptr->start_state;
	dfa_ptr->symbol_counter = 0;
	
	// If start state is a final state...
	if( HashTable_get(dfa_ptr->state_class_table, &start_state) == &STATE_CLASS_FINAL ){
		dfa_ptr->state_last_final_valid = 1;
		dfa_ptr->state_last_final = start_state;
		dfa_ptr->symbol_counter_last_final = dfa_ptr->symbol_counter;
	}
	// ... is not a final state
	dfa_ptr->state_last_final_valid = 0;
}

void Dfa_destroy(Dfa *dfa_ptr){

}


// Hashing

static int hash_function(void *key){
	return (*(int *)(key));
}

static int key_compare(void *key1, void *key2){
	return *(int *)(key1) - *(int *)(key2);
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
