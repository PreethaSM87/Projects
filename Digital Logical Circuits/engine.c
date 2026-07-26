#include <stdint.h>

#define MAX_CYCLES 20
#define NUM_SIGNALS 8

typedef struct {
    int mode;
    int current_step;
    int clk_state;
    int ff_q;
    int history[MAX_CYCLES][NUM_SIGNALS];
} CircuitEngine;

static CircuitEngine engine = {0, 0, 0, 0, {{0}}};

void set_circuit_mode(int mode) {
    engine.mode = mode;
    engine.current_step = 0;
    engine.clk_state = 0;
    engine.ff_q = 0;
    
    for (int i = 0; i < MAX_CYCLES; i++) {
        for (int j = 0; j < NUM_SIGNALS; j++) {
            engine.history[i][j] = 0;
        }
    }
}

void simulate_step(int in_a, int in_b, int unused_clk) {
    if (engine.current_step >= MAX_CYCLES) {
        for (int i = 0; i < MAX_CYCLES - 1; i++) {
            for (int j = 0; j < NUM_SIGNALS; j++) {
                engine.history[i][j] = engine.history[i + 1][j];
            }
        }
        engine.current_step = MAX_CYCLES - 1;
    }

    int s_a = in_a ? 1 : 0;
    int s_b = in_b ? 1 : 0;
    
    // Toggle Clock on every step for Square Wave
    int prev_clk = engine.clk_state;
    engine.clk_state = !engine.clk_state;
    int s_clk = engine.clk_state;
    int is_rising_edge = (s_clk == 1 && prev_clk == 0);

    int out1 = 0, out2 = 0, out3 = 0, out4 = 0, out5 = 0;

    switch (engine.mode) {
        case 0: // Basic Logic Gates
            out1 = s_a && s_b;       // AND
            out2 = s_a || s_b;       // OR
            out3 = s_a ^ s_b;        // XOR
            out4 = !(s_a && s_b);    // NAND
            break;

        case 1: // D Flip-Flop (IN_A = D)
            if (is_rising_edge) {
                engine.ff_q = s_a;
            }
            out1 = engine.ff_q;  // Q
            out2 = !engine.ff_q; // Q_bar
            break;

        case 2: // JK Flip-Flop (IN_A = J, IN_B = K)
            if (is_rising_edge) {
                if (s_a == 0 && s_b == 0)      { /* No change */ }
                else if (s_a == 0 && s_b == 1) { engine.ff_q = 0; } // Reset
                else if (s_a == 1 && s_b == 0) { engine.ff_q = 1; } // Set
                else if (s_a == 1 && s_b == 1) { engine.ff_q = !engine.ff_q; } // Toggle
            }
            out1 = engine.ff_q;  // Q
            out2 = !engine.ff_q; // Q_bar
            break;

        case 3: // T Flip-Flop (IN_A = T)
            if (is_rising_edge) {
                if (s_a == 1) { engine.ff_q = !engine.ff_q; } // Toggle when T=1
            }
            out1 = engine.ff_q;  // Q
            out2 = !engine.ff_q; // Q_bar
            break;

        case 4: // SR Flip-Flop (IN_A = S, IN_B = R)
            if (is_rising_edge) {
                if (s_a == 1 && s_b == 0)      { engine.ff_q = 1; } // Set
                else if (s_a == 0 && s_b == 1) { engine.ff_q = 0; } // Reset
            }
            out1 = engine.ff_q;  // Q
            out2 = !engine.ff_q; // Q_bar
            break;

        case 5: // Half Adder
            out1 = s_a ^ s_b;        // SUM
            out2 = s_a && s_b;       // CARRY
            break;

        case 6: // 2:1 Multiplexer
            out1 = s_clk ? s_b : s_a;
            break;
    }

    int step = engine.current_step;
    engine.history[step][0] = s_a;
    engine.history[step][1] = s_b;
    engine.history[step][2] = s_clk; // Square Wave
    engine.history[step][3] = out1;
    engine.history[step][4] = out2;
    engine.history[step][5] = out3;
    engine.history[step][6] = out4;
    engine.history[step][7] = out5;

    engine.current_step++;
}

int get_current_step() {
    return engine.current_step;
}

int get_history_value(int step, int signal_index) {
    if (step < 0 || step >= MAX_CYCLES || signal_index < 0 || signal_index >= NUM_SIGNALS) {
        return 0;
    }
    return engine.history[step][signal_index];
}