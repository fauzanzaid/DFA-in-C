/**
 *	This file expose the user facing API of the DFA library
 */

#ifndef INCLUDE_GUARD_17F57653EFD148E09EA7E032CB25872E
#define INCLUDE_GUARD_17F57653EFD148E09EA7E032CB25872E

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
void Dfa_add_transition_many(Dfa *dfa_ptr, int from_state, int to_state, char symbols, int len_symbols);

/**
 * Add a transition if the input symbol does not match any of the @p symbols
 * @param dfa_ptr     Pointer to Dfa struct
 * @param from_state  Current state identifier
 * @param to_state    Next state identifier
 * @param symbols     Array of symbols not to match
 * @param len_symbols Length of array
 */
void Dfa_add_transition_many_invert(Dfa *dfa_ptr, int from_state, int to_state, char symbols, int len_symbols);

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
 * Runs the Dfa on symbols in @p input. Running stops if the input is exhausted
 * or no transition is possible.
 * @param  dfa_ptr   Pointer to Dfa struct
 * @param  input     Array of input symbols
 * @param  len_input Length of array
 * @param  count     Count of the first symbol in the input array
 * @return           Status
 * @retval 1  Input exhausted
 * @retval 2  No transition is possible from current state with the symbol at
 *         index value of counter
 * @retval -1 The symbol at index value of counter cannot be found in the @p
 *         input
 */
int Dfa_run(Dfa *dfa_ptr, char* input, int len_input, int count);

/**
 * Retract Dfa to last reached final state. Sets the state and counter
 * accordingly
 * @param  dfa_ptr Pointer to Dfa struct
 * @return         Status
 * @retval 1  Retraction successful
 * @retval -1 No state available to retract to
 */
int Dfa_retract(Dfa *dfa_ptr);

/**
 * Sets the state of the Dfa to start state
 * @param  dfa_ptr Pointer to Dfa struct
 */
void Dfa_reset_state(Dfa *dfa_ptr);

/**
 * Sets the symbol counter to zero
 * @param dfa_ptr Pointer to Dfa struct
 */
void Dfa_reset_counter(Dfa *dfa_ptr);

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

#endif