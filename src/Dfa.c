#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "Dfa.h"
#include "HashTable.h"
#include "LinkedList.h"



///////////////
// Constants //
///////////////

static const int STATE_CLASS_NONFINAL = 0;
static const int STATE_CLASS_FINAL = 1;

typedef enum {
	TRANSITION_CLASS_SINGLE,
	TRANSITION_CLASS_SINGLE_INVERT,
	TRANSITION_CLASS_MANY,
	TRANSITION_CLASS_MANY_INVERT,
	TRANSITION_CLASS_RANGE,
	TRANSITION_CLASS_CUSTOM,
	TRANSITION_CLASS_REGEX
} TransitionClass_type;


/////////////////////
// Data Structures //
/////////////////////

typedef struct DfaTransition DfaTransition;
typedef struct DfaTransition {
	DfaTransition *next;
	int from_state, to_state;
	TransitionClass_type class;
	union{
		// For class single and single invert
		char symbol;

		// For class many and many invert
		struct{
			char *symbols;
			int len_symbols;
		};

		// For class range
		struct{
			char symbol_min;
			char symbol_max;
		};

		// For class custom
		int (*check_function)(char);

		// For class regex
		regex_t regex;
	};
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

static DfaTransition *DfaTransition_new(int from_state, int to_state);

static void DfaTransition_destroy(DfaTransition *tr_ptr);

static void add_transition_to_table(Dfa *dfa_ptr, DfaTransition *tr_ptr);

// Returns 1 if tests succeeds, else 0
static int test_transition(DfaTransition *tr_ptr, char input_symbol);


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
	// Free all transitions
	for (int i = 0; i < dfa_ptr->len_states; ++i){
		// Cycle through all from states
		DfaTransition *tr_ptr = HashTable_get(dfa_ptr->transition_table, &dfa_ptr->states[i]);
		DfaTransition *tr_ptr_next;

		while(tr_ptr == NULL){
			// Cycle through all to states for current from state
			tr_ptr_next = tr_ptr->next;
			DfaTransition_destroy(tr_ptr);
			tr_ptr = tr_ptr_next;
		}
	}

	// Free hashtables
	HashTable_destroy(dfa_ptr->transition_table);
	HashTable_destroy(dfa_ptr->state_class_table);

	// Free Dfa
	free(dfa_ptr);
}

static DfaTransition *DfaTransition_new(int from_state, int to_state){
	DfaTransition *tr_ptr = malloc( sizeof(DfaTransition) );
	tr_ptr->from_state = from_state;
	tr_ptr->to_state = to_state;

	return tr_ptr;
}

static void DfaTransition_destroy(DfaTransition *tr_ptr){
	if(tr_ptr->class == TRANSITION_CLASS_MANY ||
		tr_ptr->class == TRANSITION_CLASS_MANY_INVERT){
		free(tr_ptr->symbols);
	}

	else if(tr_ptr->class == TRANSITION_CLASS_REGEX){
		regfree(&(tr_ptr->regex));
	}

	free(tr_ptr);
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
	DfaTransition *tr_ptr = DfaTransition_new(from_state, to_state);

	tr_ptr->class = TRANSITION_CLASS_SINGLE;
	tr_ptr->symbol = symbol;

	add_transition_to_table(dfa_ptr, tr_ptr);
}

void Dfa_add_transition_single_invert(Dfa *dfa_ptr, int from_state, int to_state, char symbol){
	DfaTransition *tr_ptr = DfaTransition_new(from_state, to_state);

	tr_ptr->class = TRANSITION_CLASS_SINGLE_INVERT;
	tr_ptr->symbol = symbol;

	add_transition_to_table(dfa_ptr, tr_ptr);
}

void Dfa_add_transition_many(Dfa *dfa_ptr, int from_state, int to_state, char *symbols, int len_symbols){
	DfaTransition *tr_ptr = DfaTransition_new(from_state, to_state);

	tr_ptr->class = TRANSITION_CLASS_MANY;
	tr_ptr->symbols = malloc( sizeof(char)*len_symbols );
	memcpy(tr_ptr->symbols, symbols, len_symbols);
	tr_ptr->len_symbols = len_symbols;

	add_transition_to_table(dfa_ptr, tr_ptr);
}

void Dfa_add_transition_many_invert(Dfa *dfa_ptr, int from_state, int to_state, char *symbols, int len_symbols){
	DfaTransition *tr_ptr = DfaTransition_new(from_state, to_state);

	tr_ptr->class = TRANSITION_CLASS_MANY_INVERT;
	tr_ptr->symbols = malloc( sizeof(char)*len_symbols );
	memcpy(tr_ptr->symbols, symbols, len_symbols);
	tr_ptr->len_symbols = len_symbols;

	add_transition_to_table(dfa_ptr, tr_ptr);
}

void Dfa_add_transition_range(Dfa *dfa_ptr, int from_state, int to_state, char symbol_min, char symbol_max){
	DfaTransition *tr_ptr = DfaTransition_new(from_state, to_state);

	tr_ptr->class = TRANSITION_CLASS_RANGE;
	tr_ptr->symbol_min = symbol_min;
	tr_ptr->symbol_max = symbol_max;

	add_transition_to_table(dfa_ptr, tr_ptr);
}


void Dfa_add_transition_custom(Dfa *dfa_ptr, int from_state, int to_state, int (*check_function)(char)){
	DfaTransition *tr_ptr = DfaTransition_new(from_state, to_state);

	tr_ptr->class = TRANSITION_CLASS_CUSTOM;
	tr_ptr->check_function = check_function;

	add_transition_to_table(dfa_ptr, tr_ptr);
}

void Dfa_add_transition_regex(Dfa *dfa_ptr, int from_state, int to_state, char *pattern){
	DfaTransition *tr_ptr = DfaTransition_new(from_state, to_state);

	tr_ptr->class = TRANSITION_CLASS_REGEX;

	int err = regcomp(&(tr_ptr->regex), pattern, 0);
	if(err){
		fprintf(stderr, "Could not compile regex \"%s\"\n", pattern);
		exit(1);
	}

	add_transition_to_table(dfa_ptr, tr_ptr);
}

static void add_transition_to_table(Dfa *dfa_ptr, DfaTransition *tr_ptr){
	DfaTransition *table_tr_ptr = HashTable_get(dfa_ptr->transition_table, (void *)&tr_ptr->from_state);

	// No transition from this state exist
	if(table_tr_ptr == NULL){
		tr_ptr->next = NULL;
		HashTable_add(dfa_ptr->transition_table, (void *)&tr_ptr->from_state, (void *)tr_ptr);
	}
	// Transition exists
	else{
		// Add transition to the top of the linked list
		tr_ptr->next = HashTable_get(dfa_ptr->transition_table, (void *)&tr_ptr->from_state);
		HashTable_set(dfa_ptr->transition_table, (void *)&tr_ptr->from_state, (void *)tr_ptr);
	}
}


/////////////
// Run DFA //
/////////////

static int test_transition(DfaTransition *tr_ptr, char input_symbol){
	if(tr_ptr->class == TRANSITION_CLASS_SINGLE){
		if( tr_ptr->symbol == input_symbol ) return 1;
		else return 0;
	}

	else if(tr_ptr->class == TRANSITION_CLASS_SINGLE_INVERT){
		if( tr_ptr->symbol != input_symbol ) return 1;
		else return 0;
	}

	else if(tr_ptr->class == TRANSITION_CLASS_MANY){
		for (int i = 0; i < tr_ptr->len_symbols; ++i){
			if( tr_ptr->symbols[i] == input_symbol ){
				return 1;
			}
			return 0;
		}
	}

	else if(tr_ptr->class == TRANSITION_CLASS_MANY_INVERT){
		for (int i = 0; i < tr_ptr->len_symbols; ++i){
			if( tr_ptr->symbols[i] == input_symbol ){
				return 0;
			}
			return 1;
		}
	}

	else if(tr_ptr->class == TRANSITION_CLASS_RANGE){
		if(tr_ptr->symbol_min <= input_symbol && input_symbol <= tr_ptr->symbol_max){
			return 1;
		}
		return 0;
	}

	else if(tr_ptr->class == TRANSITION_CLASS_CUSTOM){
		if( tr_ptr->check_function(input_symbol) == 0 ){
			return 0;
		}
		return 1;
	}

	else if(tr_ptr->class == TRANSITION_CLASS_REGEX){
		char input_string[2] = {input_symbol, '\0'};
		int reti = regexec(&(tr_ptr->regex), input_string, 0, NULL, 0);
		if(!reti){
			return 1;
		}
		else if(reti == REG_NOMATCH){
			return 0;
		}
		else{
			char msgbuf[100];
			regerror(reti, &(tr_ptr->regex), msgbuf, sizeof(msgbuf));
			fprintf(stderr, "Regex match failed: %s\n", msgbuf);
			exit(1);
		}
	}

	return 0;
}

int Dfa_step(Dfa *dfa_ptr, char c){
	DfaTransition *tr_ptr = HashTable_get(dfa_ptr->transition_table, &c);
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
