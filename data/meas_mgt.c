/*Demo implementations of State Machines
 *
 * This demo leverages a table driven approach and function pointers
 *
 * Example state machine to be implemented
 *
 *          +-----+      Event1        +-----+      Event2        +-----+
 *    O---->|  A  +------------------->|  B  +------------------->|  C  |
 *          +-----+                    +-----+                    +-----+
 *             ^                                                     |
 *             |                       Event3                        |
 *             +-----------------------------------------------------+
 *
 * States: A, B, C
 * Events: NoEvent (not shown, holding current state), Event1, Event2, Event3
 *
 * Partly leveraged the example here: http://web.archive.org/web/20160808120758/http://www.gedan.net/2009/03/18/finite-state-machine-matrix-style-c-implementation-function-pointers-addon/
 *
 * This sample code can be compiled and run using GCC.
 * >> gcc -o demo_state_machine demo_state_machine.c
 * >> ./demo_state_machine
 */

#include <stdio.h>
#include <assert.h>

// Definitions of state id's, event id's, and function pointer
#define N_STATES  3
#define N_EVENTS  4

typedef enum {
  STATE_A,
  STATE_B,
  STATE_C,
} StateId;

typedef enum {
  NOEVENT,
  EVENT1,
  EVENT2,
  EVENT3,
} Event;
typedef void (*StateRoutine)();

// Assert on number of states and events defined
static_assert(STATE_C==N_STATES-1,
  "Number of states does not match defined number of states");
static_assert(EVENT3==N_EVENTS-1,
  "Number of events does not match defined number of events");

// Defining State, holds both state id and state routine
typedef struct {
    StateId id;
    StateRoutine routine;
}  State;

// General functions
void evaluate_state(Event e);

// State routines to be executed at each state
void state_routine_a(void);
void state_routine_b(void);
void state_routine_c(void);

// Defining each state with associated state routine
const State state_a = {STATE_A, state_routine_a};
const State state_b = {STATE_B, state_routine_b};
const State state_c = {STATE_C, state_routine_c};

// Defning state transition matrix as visualized in the header (events not
// defined, result in mainting the same state)
State state_transition_mat[N_STATES][N_EVENTS] = {
   { state_a, state_b, state_a, state_a},
   { state_b, state_b, state_c, state_b},
   { state_c, state_c, state_c, state_a}};

// Define current state and initialize
State current_state = state_a;

int main()
{
    while(1) {
    // Event to receive from user
    int ev;

    printf("----------------\n");
    printf("Current state: %c\n", current_state.id + 65);
    printf("Event to occur: ");
    // Receive event from user
    scanf("%u", &ev);
    evaluate_state((Event) ev); // typecast to event enumeration type
    printf("-----------------\n");
    };
    return (0);
}

/*
 * Determine state based on event and perform state routine
 */
void evaluate_state(Event ev)
{
    //Determine state based on event
  current_state = state_transition_mat[current_state.id][ev];
  printf("Transitioned to state: %c\n", current_state.id + 65);
    // Run state routine
    (*current_state.routine)();
}

/*
 * State routines
 */
void state_routine_a() {
  printf("State A routine ran. \n");

}
void state_routine_b() {
  printf("State B routine ran. \n");
}
void state_routine_c() {
  printf("State C routine ran. \n");
}
