Markdown
# Digital Logic Circuit Simulator Engine (C & WebAssembly)

A multi-mode digital circuit simulation engine written in C99 and compiled to WebAssembly (WASM). This simulator models combinational logic, edge-triggered sequential flip-flops, half adders, and multiplexers across multiple clock cycles, featuring signal state history tracking in a sliding buffer window.

---

## 📁 Repository File Structure

| File | Language / Type | Description |
| :--- | :--- | :--- |
| **`engine.c`** | C Source | Core logic engine containing the circuit algorithms, clock wave generation, flip-flop state machine, and sliding history memory buffer. |
| **`main.c`** | C Source | Native C test harness that executes simulation modes and prints formatted timing/history tables directly to the console. |
| **`index.html`** | HTML5 | Web application user interface for interacting with the circuit simulator directly inside any modern web browser. |
| **`engine.js`** | JavaScript | Emscripten glue code binding the WebAssembly module to the web front-end (`index.html`). |
| **`engine.wasm`** | WebAssembly | High-performance binary compiled from `engine.c` for fast browser execution. |
| **`README.md`** | Markdown | Complete project documentation and usage guide. |

---

## ⚙️ How the Simulator Engine Works

### 1. Internal Architecture & Data Structures
The simulation engine tracks its entire system state in a dedicated struct (`CircuitEngine`):
* **Mode (`mode`):** Selects which digital component is currently active.
* **Clock Wave (`clk_state`):** Toggles between `0` and `1` on every call to `simulate_step()`, generating an automatic square wave.
* **Edge Detection (`is_rising_edge`):** Tracks clock transitions ($0 \rightarrow 1$). Sequential flip-flop logic triggers exclusively on rising clock edges.
* **Internal Memory (`ff_q`):** Holds the state ($Q$) for sequential logic elements.
* **History Matrix (`history[20][8]`):** A $20 \times 8$ array recording up to 20 simulation steps for inputs ($A, B$), clock ($CLK$), and output signals ($OUT_1$ through $OUT_5$).
* **Sliding Buffer Window:** When simulation steps exceed 20 cycles (`MAX_CYCLES`), the engine automatically discards the oldest cycle, shifts history memory left, and logs new data into the newest slot.

---

## 🧩 Supported Circuit Modes

| Mode Index | Selected Component | Type | Outputs Recorded |
| :---: | :--- | :--- | :--- |
| **`0`** | **Basic Logic Gates** | Combinational | $OUT_1 = \text{AND}$, $OUT_2 = \text{OR}$, $OUT_3 = \text{XOR}$, $OUT_4 = \text{NAND}$ |
| **`1`** | **D Flip-Flop** | Sequential | $OUT_1 = Q$, $OUT_2 = \overline{Q}$ (Latches input $D = \text{IN\_A}$ on rising $CLK$) |
| **`2`** | **JK Flip-Flop** | Sequential | $OUT_1 = Q$, $OUT_2 = \overline{Q}$ (Supports Hold, Reset, Set, Toggle based on $J, K$) |
| **`3`** | **T Flip-Flop** | Sequential | $OUT_1 = Q$, $OUT_2 = \overline{Q}$ (Toggles state when $T = \text{IN\_A} = 1$ on rising $CLK$) |
| **`4`** | **SR Flip-Flop** | Sequential | $OUT_1 = Q$, $OUT_2 = \overline{Q}$ (Set/Reset latch on rising $CLK$) |
| **`5`** | **Half Adder** | Combinational | $OUT_1 = \text{SUM} (A \oplus B)$, $OUT_2 = \text{CARRY} (A \cdot B)$ |
| **`6`** | **2:1 Multiplexer** | Combinational | $OUT_1 = (\text{CLK} == 1 \;?\text{IN\_B} : \text{IN\_A})$ |

---

## 💻 Code Overview & Highlights

### 1. Header Inclusion
```c
#include <stdint.h>
Provides exact-width standard integer types, ensuring cross-platform behavior across standard terminal and WebAssembly environments.

2. Edge Trigger Detection Logic
C
int prev_clk = engine.clk_state;
engine.clk_state = !engine.clk_state; // Toggle clock state
int s_clk = engine.clk_state;
int is_rising_edge = (s_clk == 1 && prev_clk == 0); // Detect 0 -> 1 transition
🚀 How to Build and Run
Option A: Run Native C Command Line Application
Compile main.c and engine.c together using GCC, Clang, or MSVC:

Bash
# 1. Compile the project
gcc main.c engine.c -o simulator

# 2. Execute on Linux / macOS
./simulator

# Execute on Windows
simulator.exe
Sample Console Output:
Plaintext
****************************************************
       DIGITAL CIRCUIT ENGINE SIMULATION TEST       
****************************************************

====================================================
 MODE: Basic Logic Gates (OUT1=AND, OUT2=OR, OUT3=XOR, OUT4=NAND)
====================================================
 Step | IN_A | IN_B | CLK | OUT1 | OUT2 | OUT3 | OUT4
------+------+------+-----+------+------+------+------
   0  |  0   |  0   |  1  |  0   |  0   |  0   |  1   
   1  |  0   |  1   |  0  |  0   |  1   |  1   |  1   
   2  |  1   |  0   |  1  |  0   |  1   |  1   |  1   
   3  |  1   |  1   |  0  |  1   |  1   |  0   |  0   

====================================================
 MODE: D Flip-Flop (OUT1=Q, OUT2=Q_bar)
====================================================
 Step | IN_A | IN_B | CLK | OUT1 | OUT2
------+------+------+-----+------+------
   0  |  1   |  0   |  1  |  1   |  0   
   1  |  1   |  0   |  0  |  1   |  0   
   2  |  0   |  0   |  1  |  0   |  1   
   3  |  0   |  0   |  0  |  0   |  1   
Option B: Run WebAssembly Web Interface
Open a terminal in the root project folder.

Launch a local web server (required for web browsers to load .wasm binaries):

Bash
# Using Python 3
python -m http.server 8000
Open your web browser and go to:

Plaintext
http://localhost:8000
<img width="1588" height="818" alt="image" src="https://github.com/user-attachments/assets/a24bb1fc-f70f-4074-b380-bdecbc3136f9" />
