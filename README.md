# RISC-V-Simulator
Implementation of a function simulator in C/C++ for 32bit RISCV ISA instructions.

Overall code is divided into 4 phases <br/><br/> PHASE1: This part takes the RISC-V instructions in the "input.asm" file and converts it into the machine code("machine_code.mc" file) for further processing.Read the README file in phase1 folder for more details <br/><br/> PHASE2: This part takes the "machine_code.mc" file as input and executes all the instructions one by one.Read the README file in phase2 folder for more details <br/><br/> PHASE3: This part is similar to phase2, the only difference is that this part is used for concurrent execution of instructions (PIPELINING). Checking for data dependencies among instructions,implementing the branch prediction and data forwarding. All these have been implemented for implementation of PIPELINING <br/><br/> PHASE4: Cache is implemented in this part for faster memory accesss.



