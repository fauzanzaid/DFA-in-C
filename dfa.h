#ifndef INCLUDE_GUARD_17F57653EFD148E09EA7E032CB25872E
#define INCLUDE_GUARD_17F57653EFD148E09EA7E032CB25872E

typedef struct Dfa Dfa;

Dfa *Dfa_new(int *states, int len_states, char *symbols, int len_symbols, int start_state, int *final_states, int len_final_states);
void Dfa_destroy(Dfa *dfa);


void Dfa_add_transition_single(Dfa *dfa, int from_state, int to_state, char symbol);
void Dfa_add_transition_single_invert(Dfa *dfa, int from_state, int to_state, char symbol);
void Dfa_add_transition_many(Dfa *dfa, int from_state, int to_state, char symbols, int len_symbols);
void Dfa_add_transition_many_invert(Dfa *dfa, int from_state, int to_state, char symbols, int len_symbols);
void Dfa_add_transition_custom(Dfa *dfa, int from_state, int to_state, int (*check_function)(char))
void Dfa_add_transition_regex(Dfa *dfa, int from_state, int to_state, char *pattern);

int Dfa_run(Dfa *dfa, char* input, int len_input);
int Dfa_reset(Dfa *dfa);


int Dfa_get_current_state(Dfa *dfa, int *state_type, int *counter);
int Dfa_get_previous_state(Dfa *dfa, int *state_type, int *counter);
int Dfa_get_last_final_state(Dfa *dfa, int *state_type, int *counter);


#endif