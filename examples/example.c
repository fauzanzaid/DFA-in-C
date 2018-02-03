#include <stdio.h>

#include "Dfa.h"


int check_function_2_to_3(char c){
	if(c=='b')
		return 1;
	return 0;
}


int main(int argc, char const *argv[])
{
	int states[] = {1,2,3,4,5};
	int len_states = 5;
	int start_state = 1;
	int final_states[] = {3,5};
	int len_final_states = 2;
	char symbols[] = {'a', 'b'};
	int len_symbols = 2;

	Dfa *dfa_ptr = Dfa_new(states, len_states, symbols, len_symbols, start_state, final_states, len_final_states);
	
	Dfa_add_transition_single(dfa_ptr, 1, 2, 'a');
	Dfa_add_transition_custom(dfa_ptr, 2, 3, check_function_2_to_3);
	Dfa_add_transition_single(dfa_ptr, 2, 4, 'a');
	Dfa_add_transition_single(dfa_ptr, 3, 4, 'a');
	Dfa_add_transition_single(dfa_ptr, 4, 5, 'b');
	Dfa_add_transition_single_invert(dfa_ptr, 5, 4, 'b');

	char input[] = {'a', 'b', 'a', 'b', 'a', 'a', 'a', 'b'};
	int len_input = 8;

	int state_cur;
	int symbol_counter;
	int state_class;

	Dfa_get_current_configuration(dfa_ptr, &state_cur, &state_class, &symbol_counter);
	printf("%d, %d, %d\n", state_cur, symbol_counter, state_class);

	int status;

	for (int i = 0; i < len_input; ++i){
		status = Dfa_step(dfa_ptr, input[i]);
		printf("--in:%c status:%d\n", input[i], status);
		
		Dfa_get_current_configuration(dfa_ptr, &state_cur, &state_class, &symbol_counter);
		printf("%d, %d, %d\n", state_cur, symbol_counter, state_class);

		// if(status == 1)	break;
	}

	Dfa_reset(dfa_ptr);
	printf("----Reset\n");

	Dfa_get_current_configuration(dfa_ptr, &state_cur, &state_class, &symbol_counter);
	printf("%d, %d, %d\n", state_cur, symbol_counter, state_class);

	status = Dfa_run(dfa_ptr, input, len_input, 1);
	printf("--in:%s\tstatus:%d\n", input, status);
	Dfa_get_current_configuration(dfa_ptr, &state_cur, &state_class, &symbol_counter);
	printf("%d, %d, %d\n", state_cur, symbol_counter, state_class);

	status = Dfa_retract(dfa_ptr);
	printf("retract status%d\n", status);
	Dfa_get_current_configuration(dfa_ptr, &state_cur, &state_class, &symbol_counter);
	printf("%d, %d, %d\n", state_cur, symbol_counter, state_class);

	status = Dfa_retract(dfa_ptr);
	printf("retract status%d\n", status);
	Dfa_get_current_configuration(dfa_ptr, &state_cur, &state_class, &symbol_counter);
	printf("%d, %d, %d\n", state_cur, symbol_counter, state_class);

	Dfa_destroy(dfa_ptr);

	return 0;
}
