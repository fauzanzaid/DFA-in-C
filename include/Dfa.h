/**
 *	This file expose the user facing API of the DFA library
 */

#ifndef INCLUDE_GUARD_17F57653EFD148E09EA7E032CB25872E
#define INCLUDE_GUARD_17F57653EFD148E09EA7E032CB25872E

///////////////
// Constants //
///////////////

static const int DFA_STATE_CLASS_NONFINAL = 0;
static const int DFA_STATE_CLASS_FINAL = 1;


///////////
// Types //
///////////

typedef enum{
	DFA_STEP_RESULT_SUCCESS,
	DFA_STEP_RESULT_UNKNOWN,
	DFA_STEP_RESULT_FAIL = -1
}DFA_StepResult_type;

typedef enum{
	DFA_RUN_RESULT_MORE_INPUT,
	DFA_RUN_RESULT_TRAP,
	DFA_RUN_RESULT_UNKNOWN,
	DFA_RUN_RESULT_WRONG_INDEX = -1
}DFA_RunResult_type;

typedef enum{
	DFA_RETRACT_RESULT_SUCCESS,
	DFA_RETRACT_RESULT_UNKNOWN,
	DFA_RETRACT_RESULT_FAIL = -1
}DFA_RetractResult_type;


/////////////////////
// Data Structures //
/////////////////////

/**
 * Opaque struct to hold the DFA
 */
typedef struct Dfa Dfa;

//////////////////////////////////
// Constructors and Destructors //
//////////////////////////////////

/**
 * Allocates space for and initializes a Dfa struct and returns a pointer to it
 * @param  states           Array of state identifiers
 * @param  len_states       Length of array
 * @param  symbols          Array of input symbols
 * @param  len_symbols      Length of array
 * @param  start_state      The start state identifier
 * @param  final_states     Array of final states' identifiers
 * @param  len_final_states Length of array
 * @return                  Pointer to allocated Dfa struct
 */
Dfa *Dfa_new(int *states, int len_states, char *symbols, int len_symbols, int start_state, int *final_states, int len_final_states);

/**
 * Deallocates all memory blocks associated with the Dfa struct
 * @param dfa_ptr pointer to the Dfa struct
 */
void Dfa_destroy(Dfa *dfa_ptr);

/////////////////////
// Add transitions //
/////////////////////

/**
 * Add a transition if the input symbol matches @p symbol
 * @param dfa_ptr    Pointer to Dfa struct
 * @param from_state Current state identifier
 * @param to_state   Next state identifier
 * @param symbol     Symbol to match
 */
void Dfa_add_transition_single(Dfa *dfa_ptr, int from_state, int to_state, char symbol);

/**
 * Add a transition if the input symbol does not match @p symbol
 * @param dfa_ptr    Pointer to Dfa struct
 * @param from_state Current state identifier
 * @param to_state   Next state identifier
 * @param symbol     Symbol to not match
 */
void Dfa_add_transition_single_invert(Dfa *dfa_ptr, int from_state, int to_state, char symbol);

/**
 * Add a transition if the input symbol matches any of the @p symbols
 * @param dfa_ptr     Pointer to Dfa struct
 * @param from_state  Current state identifier
 * @param to_state    Next state identifier
 * @param symbols     Array of symbols to match
 * @param len_symbols Length of array
 */
void Dfa_add_transition_many(Dfa *dfa_ptr, int from_state, int to_state, char *symbols, int len_symbols);

/**
 * Add a transition if the input symbol does not match any of the @p symbols
 * @param dfa_ptr     Pointer to Dfa struct
 * @param from_state  Current state identifier
 * @param to_state    Next state identifier
 * @param symbols     Array of symbols not to match
 * @param len_symbols Length of array
 */
void Dfa_add_transition_many_invert(Dfa *dfa_ptr, int from_state, int to_state, char *symbols, int len_symbols);

/**
 * Add a transition if the integer value of input symbol is between @p symbol_min and @p symbol_min
 * @param dfa_ptr     Pointer to Dfa struct
 * @param from_state  Current state identifier
 * @param to_state    Next state identifier
 * @param symbol_min  Minimum value of input symbol ,inclusive
 * @param symbol_max  Maximum value of input symbol ,inclusive
 */
void Dfa_add_transition_range(Dfa *dfa_ptr, int from_state, int to_state, char symbol_min, char symbol_max);

/**
 * Add a transition if @p check_function returns true on calling it with an
 * input symbol
 * @param dfa_ptr        Pointer to Dfa struct
 * @param from_state     Current state identifier
 * @param to_state       Next state identifier
 * @param check_function Function which returns true or false on being called
 * with a symbol
 */
void Dfa_add_transition_custom(Dfa *dfa_ptr, int from_state, int to_state, int (*check_function)(char));

/**
 * Add a transition if the input symbol matched the @p pattern
 * @param dfa_ptr    Pointer to Dfa struct
 * @param from_state Current state identifier
 * @param to_state   Next state identifier
 * @param pattern    A regex pattern in the extended POSIX syntax
 */
void Dfa_add_transition_regex(Dfa *dfa_ptr, int from_state, int to_state, char *pattern);

/////////////
// Run DFA //
/////////////

/**
 * Attempts one transition if possible.
 * @param  dfa_ptr       Pointer to Dfa struct
 * @param  input_symbol  Input symbol
 * @return               Status
 * @retval DFA_STEP_RESULT_SUCCESS  Transition successful
 * @retval DFA_STEP_RESULT_FAIL     No transition is possible from current state
 * with the input symbol
 */
DFA_StepResult_type Dfa_step(Dfa *dfa_ptr, char input_symbol);

/**
 * Runs the Dfa on symbols in @p input. Running stops if the input is exhausted
 * or no transition is possible.
 * @param  dfa_ptr   Pointer to Dfa struct
 * @param  input     Array of input symbols
 * @param  len_input Length of array
 * @param  global_index     Global index of the first symbol in the input array.
 * Global index is one based (starts from 1)
 * @return           Status
 * @retval DFA_RUN_RESULT_TRAP        No transition is possible from current
 * reached state with the symbol at index value of counter
 * @retval DFA_RUN_RESULT_MORE_INPUT  Input exhausted
 * @retval DFA_RUN_RESULT_WRONG_INDEX The symbol at index value of counter
 * cannot be found in the @p input
 */
DFA_RunResult_type Dfa_run(Dfa *dfa_ptr, char* input, int len_input, int global_index);

/**
 * Skip a character. This function can be called in case step fails, or run
 * traps. This function increases the count of internal counter by one, which
 * discards the index of the character which caused the failure.
 * @param dfa_ptr Pointer to Dfa struct
 */
void Dfa_skip(Dfa *dfa_ptr);

/**
 * Retract Dfa to last reached final state. Sets the state and counter
 * accordingly
 * @param  dfa_ptr Pointer to Dfa struct
 * @return         Status
 * @retval DFA_RETRACT_RESULT_SUCESS  Retraction successful
 * @retval DFA_RETRACT_RESULT_FAIL    No state available to retract to
 */
DFA_RetractResult_type Dfa_retract(Dfa *dfa_ptr);

/**
 * Sets the state of the Dfa to start state
 * Invalidates last reached final state
 * @param  dfa_ptr Pointer to Dfa struct
 */
void Dfa_reset_state(Dfa *dfa_ptr);

/**
 * Sets the state of the Dfa to start state
 * Invalidates last reached final state
 * Sets the symbol counter to zero
 * @param dfa_ptr Pointer to Dfa struct
 */
void Dfa_reset(Dfa *dfa_ptr);

/**
 * Get infomation about current Dfa configuration
 * @param  dfa_ptr        Pointer to Dfa struct
 * @param  state_ptr      Pointer to location which will be assigned the state
 *                        identifier. Set to NULL to skip.
 * @param  state_type_ptr Pointer to location which will be assigned the state
 *                        type. 0 for start. 1 for non start, non final. 2 for
 *                        final. Set to NULL to skip.
 * @param  counter_ptr    Pointer to location which will be assigned the value
 *                        of counter. Set to NULL to skip.
 */
void Dfa_get_current_configuration(Dfa *dfa_ptr, int *state_ptr, int *state_type_ptr, int *counter_ptr);

///////////
// Other //
///////////

/**
 * Get information about the states of Dfa
 * @param dfa_ptr          Pointer to Dfa struct
 * @param states           Pointer to an int pointer
 * @param len_states       Pointer to an int
 * @param start_state      Pointer to an int
 * @param final_states     Pointer to an int pointer
 * @param len_final_states Pointer to an int
 */
void Dfa_get_state_lists(Dfa *dfa_ptr, int **states, int *len_states, int *start_state, int **final_states, int *len_final_states);

/**
 * Get information about the accpted symbols of the Dfa
 * @param dfa_ptr     Pointer to Dfa struct
 * @param symbols     Pointer to a char pointer
 * @param len_symbols Pointer to an int
 */
void Dfa_get_symbol_list(Dfa *dfa_ptr, char **symbols, int *len_symbols);

#endif
