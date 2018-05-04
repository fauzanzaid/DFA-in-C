# DFA-in-C
A library for simulating Deterministic Finite Automata written in C

### Building
First, download the dependencies:
```bash
./download_dependencies.sh
```

Then, to build the static library, run the following commands from the terminal:
```bash
mkdir build && cd build && cmake .. && make ; cd ..
```
This will build ```libDfa.a``` in ```./lib``` directory.

### Usage
See ```include/Dfa.h``` for information about functionality provided by this module, or generate doxygen documentation.
