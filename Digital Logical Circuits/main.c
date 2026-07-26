#include <stdio.h>
#include <stdint.h>

// ============================================================================
// ENGINE FUNCTION DECLARATIONS (No engine.h header required)
// ============================================================================
void set_circuit_mode(int mode);
void simulate_step(int in_a, int in_b, int unused_clk);
int get_current_step(void);
int get_history_value(int step, int signal_index);

// ============================================================================
// HELPER FUNCTION: PRINTS TIMING / HISTORY TABLE
// ============================================================================
void print_simulation_history(const char* mode_name, int num_outputs) {
    printf("====================================================\n");
    printf(" MODE: %s\n", mode_name);
    printf("====================================================\n");
    
    printf(" Step | IN_A | IN_B | CLK | OUT1 | OUT2");
    if (num_outputs >= 3) printf(" | OUT3");
    if (num_outputs >= 4) printf(" | OUT4");
    printf("\n------+------+------+-----+------+------");
    if (num_outputs >= 3) printf("+------");
    if (num_outputs >= 4) printf("+------");
    printf("\n");

    int steps = get_current_step();
    for (int i = 0; i < steps; i++) {
        printf("  %2d  |  %d   |  %d   |  %d  |  %d   |  %d   ",
               i,
               get_history_value(i, 0),  // IN_A
               get_history_value(i, 1),  // IN_B
               get_history_value(i, 2),  // CLK
               get_history_value(i, 3),  // OUT1
               get_history_value(i, 4)); // OUT2

        if (num_outputs >= 3) printf("|  %d   ", get_history_value(i, 5)); // OUT3
        if (num_outputs >= 4) printf("|  %d   ", get_history_value(i, 6)); // OUT4
        printf("\n");
    }
    printf("\n");
}

// ============================================================================
// MAIN FUNCTION: RUNS ALL TEST SUITES
// ============================================================================
int main(void) {
    printf("\n****************************************************\n");
    printf("       DIGITAL CIRCUIT ENGINE SIMULATION TEST       \n");
    printf("****************************************************\n\n");

    // ------------------------------------------------------------------------
    // TEST 1: Mode 0 - Basic Logic Gates
    // ------------------------------------------------------------------------
    set_circuit_mode(0);
    simulate_step(0, 0, 0); // AND=0, OR=0, XOR=0, NAND=1
    simulate_step(0, 1, 0); // AND=0, OR=1, XOR=1, NAND=1
    simulate_step(1, 0, 0); // AND=0, OR=1, XOR=1, NAND=1
    simulate_step(1, 1, 0); // AND=1, OR=1, XOR=0, NAND=0
    print_simulation_history("Basic Logic Gates (OUT1=AND, OUT2=OR, OUT3=XOR, OUT4=NAND)", 4);

    // ------------------------------------------------------------------------
    // TEST 2: Mode 1 - D Flip-Flop
    // ------------------------------------------------------------------------
    set_circuit_mode(1);
    simulate_step(1, 0, 0); // Step 0 (CLK=1, Rising Edge): Captures D=1 -> Q=1
    simulate_step(1, 0, 0); // Step 1 (CLK=0, Falling Edge): Retains Q=1
    simulate_step(0, 0, 0); // Step 2 (CLK=1, Rising Edge): Captures D=0 -> Q=0
    simulate_step(0, 0, 0); // Step 3 (CLK=0, Falling Edge): Retains Q=0
    print_simulation_history("D Flip-Flop (OUT1=Q, OUT2=Q_bar)", 2);

    // ------------------------------------------------------------------------
    // TEST 3: Mode 2 - JK Flip-Flop
    // ------------------------------------------------------------------------
    set_circuit_mode(2);
    simulate_step(1, 0, 0); // Rising Edge: J=1, K=0 -> SET (Q=1)
    simulate_step(0, 0, 0); // Falling Edge: Hold
    simulate_step(0, 1, 0); // Rising Edge: J=0, K=1 -> RESET (Q=0)
    simulate_step(1, 1, 0); // Falling Edge: Hold
    simulate_step(1, 1, 0); // Rising Edge: J=1, K=1 -> TOGGLE (Q=1)
    print_simulation_history("JK Flip-Flop (OUT1=Q, OUT2=Q_bar)", 2);

    // ------------------------------------------------------------------------
    // TEST 4: Mode 5 - Half Adder
    // ------------------------------------------------------------------------
    set_circuit_mode(5);
    simulate_step(0, 0, 0); // 0 + 0 = SUM: 0, CARRY: 0
    simulate_step(0, 1, 0); // 0 + 1 = SUM: 1, CARRY: 0
    simulate_step(1, 0, 0); // 1 + 0 = SUM: 1, CARRY: 0
    simulate_step(1, 1, 0); // 1 + 1 = SUM: 0, CARRY: 1
    print_simulation_history("Half Adder (OUT1=SUM, OUT2=CARRY)", 2);

    // ------------------------------------------------------------------------
    // TEST 5: Mode 6 - 2:1 Multiplexer
    // ------------------------------------------------------------------------
    set_circuit_mode(6);
    simulate_step(1, 0, 0); // CLK=1 -> Selects IN_B (0)
    simulate_step(1, 0, 0); // CLK=0 -> Selects IN_A (1)
    print_simulation_history("2:1 Multiplexer (Select = CLK)", 1);

    printf("****************************************************\n");
    printf("             SIMULATION COMPLETED                   \n");
    printf("****************************************************\n\n");

    return 0;
}