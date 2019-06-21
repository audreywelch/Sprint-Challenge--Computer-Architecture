#ifndef _CPU_H_
#define _CPU_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Holds all information about the CPU
// Type for a single unsigned byte is unsigned char
struct cpu {
  
  // PC: Program Counter
  unsigned char pc;

  // registers (array): Instruction Register
  unsigned char registers[8];

  // ram (array)
  unsigned char ram[256];

  // flag register - 00000LGE
  unsigned char flag;

};

// ALU operations
enum alu_op {
	ALU_MUL,
  ALU_ADD
};

// Instructions

// These use binary literals. If these aren't available with your compiler, hex
// literals should be used.

#define LDI  0b10000010
#define PRN  0b01000111
// TODO: more instructions here. These can be used in cpu_run().
#define HLT  0b00000001
#define MUL  0b10100010
#define ADD  0b10100000
#define PUSH 0b01000101
#define POP  0b01000110
#define SP   7 // Points to the value at the top of the stack / most recently pushed
#define CALL 0b01010000
#define RET  0b00010001
#define CMP  0b10100111
#define JMP  0b01010100

// Function declarations

extern void cpu_load(struct cpu *cpu, char *filename);
extern void cpu_init(struct cpu *cpu);
extern void cpu_run(struct cpu *cpu);

#endif
