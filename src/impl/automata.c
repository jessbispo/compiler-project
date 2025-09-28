#include <stdio.h>
#include <stdlib.h>

#include "../header/automata.h"
#include "../header/lexical-analysis.h"

State* create_state(int id, int is_final) {
    State *state = (State *)malloc(sizeof(State));
    state->id = id;
    state->is_final = is_final;
    state->transitions = NULL;
    state->transition_count = 0;
    return state;
}

void add_transition(State *from, char symbol, State *to) {
    from->transitions = (Transition **)realloc(from->transitions, sizeof(Transition *) * (from->transition_count + 1));
    Transition *transition = (Transition *)malloc(sizeof(Transition));
    transition->symbol = symbol;
    transition->to = to;
    from->transitions[from->transition_count++] = transition;
}

Automaton* create_automaton() {
    Automaton *automaton = (Automaton *)malloc(sizeof(Automaton));
    automaton->initial_state = NULL;
    automaton->states = NULL;
    automaton->state_count = 0;
    return automaton;
}

void add_state(Automaton *automaton, State *state) {
    automaton->states = (State **)realloc(automaton->states, sizeof(State *) * (automaton->state_count + 1));
    automaton->states[automaton ->state_count++] = state;
}

void set_start_state(Automaton *automaton, State *state) {
    automaton->initial_state = state;
}

void add_final_state(Automaton *automaton, State *state) {
    state->is_final = 1;
}

int is_in_final_state(Automaton *automaton, State *state) {
    return state->is_final;
}

int process_input(Automaton *automaton, const char *input) {
    State *current_state = automaton->initial_state;
    for (int i = 0; input[i] != '\0'; i++) {
        char symbol = input[i];
        int found_transition = 0;
        for (int j = 0; j < current_state->transition_count; j++) {
            if (current_state->transitions[j]->symbol == symbol) {
                current_state = current_state->transitions[j]->to;
                found_transition = 1;
                break;
            }
        }
        if (!found_transition) {
            return 0;
        }
    }
    return current_state->is_final;
}

int free_automaton(Automaton *automaton) {
    if (!automaton) return -1;

    for (int i = 0; i < automaton->state_count; i++) {
        State *state = automaton->states[i];
        for (int j = 0; j < state->transition_count; j++) {
            free(state->transitions[j]);
        }
        free(state->transitions);
        free(state);
    }
    free(automaton->states);
    free(automaton);
    return 0;
}


