#ifndef _CPU_H_
#define _CPU_H_
#define SP 5 // Points to the value at the top of the stack / most recently pushed

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
  ALU_ADD,
  // SPRINT ----------------
  ALU_CMP
};

// Memory locations
#define ADDR_PROGRAM_ENTRY 0x00 // Where programs start getting loaded
#define ADDR_EMPTY_STACK 0xF4 // Where SP is on an empty stack

// Flags
#define FLAG_EQ 0b00000001
#define FLAG_GT 0b00000010
#define FLAG_LT 0b00000100

// These use binary literals. If these aren't available with your compiler, hex
// literals should be used.

#define LDI  0b10000010
#define PRN  0b01000111
#define HLT  0b00000001
#define MUL  0b10100010
#define ADD  0b10100000
#define PUSH 0b01000101
#define POP  0b01000110
#define CALL 0b01010000
#define RET  0b00010001
// SPRINT ----------------
#define CMP  0b10100111
#define JMP  0b01010100
#define JEQ  0b01010101
#define JNE  0b01010110

// Function declarations

extern void cpu_load(struct cpu *cpu, char *filename);
extern void cpu_init(struct cpu *cpu);
extern void cpu_run(struct cpu *cpu);
extern unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index);
extern void cpu_ram_write(struct cpu *cpu, unsigned char value, unsigned char index);

#endif
