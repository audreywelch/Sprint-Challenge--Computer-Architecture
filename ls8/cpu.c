#include "cpu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Helper function to read a value from the specified index in RAM
// Returns the read value
// index = memory address register (mar)
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index) {
  return cpu->ram[index];
}

// Helper function to write a value to the specified index in RAM
// value = memory data register (mdr) - value to write to
void cpu_ram_write(struct cpu *cpu, unsigned char value, unsigned char index) {
  cpu->ram[index] = value;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filename)
{

  FILE *fp = fopen(filename, "r");
  char line[1024];

  unsigned char ram_address = 0x00;

  // Open the file
  if (fp == NULL) {
    fprintf(stderr, "Cannot open file %s\n", filename);
    exit(2);
  }

  // fp = fopen(argv[1], "r"); // fopen("print8.ls8", "r")

  while (fgets(line, sizeof(line), fp) != NULL) {

    char *endpointer;

    // Convert binary strings to integer values to store in RAM
    unsigned char byte = strtoul(line, &endpointer, 2); // Finds everything in binary

    // Ignore lines where no numbers were found
    if (line == endpointer) {
      continue;
    }

    // Store the value in memory/RAM
    cpu_ram_write(cpu, byte, ram_address++);
  }
  fclose(fp);
}



/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      // Multiply the values in two registers together and store the result in registerA
      cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
      break;

    // TODO: implement more ALU ops
    case ALU_ADD:
      // Add the value in two registers and store the result in registerA
      cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];
      break;
  }
}

// // Helps trace what CPU is doing
void trace(struct cpu *cpu)
{
    printf("%02X | ", cpu->pc);

    printf("%02X %02X %02X |",
        cpu_ram_read(cpu, cpu->pc),
        cpu_ram_read(cpu, cpu->pc + 1),
        cpu_ram_read(cpu, cpu->pc + 2));

    for (int i = 0; i < 8; i++) {
        printf(" %02X", cpu->registers[i]);
    }

    printf("\n");
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  // Just so we don't have to type cpu-> every time
  // unsigned char *reg = cpu->reg;

  // unsigned char *pc = cpu->pc;

  while (running) {

    unsigned char operandA = 0;
    unsigned char operandB = 0;
    
    // 1. Get the value of the current instruction (in address PC).
    unsigned char ir = cpu_ram_read(cpu, cpu->pc);

    // 2. Figure out how many operands this next instruction requires
    unsigned int num_operands = (ir >> 6);

    // 3. Get the appropriate value(s) of the operands
    if (num_operands == 2) {
      operandA = cpu_ram_read(cpu, (cpu->pc + 1)  & 0xff);
      operandB = cpu_ram_read(cpu, (cpu->pc + 2)  & 0xff);
    } else if (num_operands == 1) {
      operandA = cpu_ram_read(cpu, (cpu->pc + 1)  & 0xff);
    }
    
    // True if this instruction might set the PC
    // This line is shifting the instruction by 4 bits to access the flag that indicates whether the PC might be set, and then &'ing it to see if the bit is set to 0 or 1
    int instruction_set_pc = (ir >> 4) & 1;
    
    trace(cpu);

    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
    switch (ir) {

      case CALL: // Calls a subroutine at the address stored in the register

        // 1. The address of the instruction directly after CALL is pushed onto the stack. This allows us to return to where we left off when the subroutine finishes executing

        // Decrement the stack pointer
        cpu->registers[SP]--;

        // Push the address where we currently are onto the stack
        cpu->ram[cpu->registers[SP]] = cpu->pc + 2;

        // 2. The PC is set to the address stored in the given register. We jump to that location in RAM and execute the first instruction in the subroutine.
        
        // Set the PC to the subroutine address, which is provided in the first operand
        cpu->pc = cpu->registers[operandA];

        // Account for later when we reset the PC
        num_operands = -1;

        break;

      case RET: // Return from subroutine

        // Pop the value (return address) from the top of the stack and store it in the PC
        cpu->pc = cpu->ram[cpu->registers[SP]];

        cpu->registers[SP]++;
        
        break;

      case PUSH:

        // Decrement the Stack Pointer
        cpu->registers[SP]--;

        // Copy the value in given register to the address pointed to by SP
        cpu->ram[cpu->registers[SP]] = cpu->registers[operandA];
        break;

      case POP:

        // Copy the value from the address pointed to by SP to the given register
        cpu->registers[operandA] = cpu->ram[cpu->registers[SP]];

        // Increment the Stack Pointer
        cpu->registers[SP]++;
        break;
      
      case HLT:
        // Halt the CPU & exit the emulator 
        running = 0;
        break;

      case LDI:
        // Set the value of a register to an integer
        cpu->registers[operandA] = operandB;
        break;

      case PRN:
        // Print to the console the decimal integer value stored in the given register
        printf("%d\n", cpu->registers[operandA]);
        break;

      case MUL:
        // Multiply the values in two registers together and store the result in registerA
        alu(cpu, ALU_MUL, operandA, operandB);
        break;

      case ADD:

        // Add the value in two registers and store the result in registerA
        alu(cpu, ALU_ADD, operandA, operandB);

        break;

      // Handle unknown instructions
      default:
        fprintf(stderr, "PC %02x: unknown instruction %02x\n", cpu->pc, ir);
        exit(3);
    }

    // If it's not 1 / True
    if(!instruction_set_pc) {

      // Increment PC by the number of arguments that were passed to the instruction we just executed
      // We do this by shifting 6 bits and modding by 4 to access the 1st and 2nd bits of the IR, which indicate how many operands the previous instruction expected
      // Plus 1 because that is the size of the opcode itself
      cpu->pc += num_operands + 1;
    }
  }
}

/**
 * Initialize a CPU struct
 */

// Takes a pointer to a struct cpu and initializes it as necessary
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers, clearing them to 0
  cpu->pc = 0;
  cpu->flag = 0;

  memset(cpu->registers, 0, sizeof(cpu->registers));
  memset(cpu->ram, 0, sizeof(cpu->ram));

  // Initialize SP (stack pointer)
  cpu->registers[7] = 0xF4;

  //cpu->registers[SP] = ADDR_EMPTY_STACK;
}










// """
// #define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
//   765433210 bit numbers (pos)
// 0b10101110

// x = 325;
// x = 0b101000101; // 325

// if (CHECK_BIT(x, 3)) {
//   printf("Bit 3 is set\n");
// }

// 0b0001 << 3 == 0b1000

// x = 0b101000101
//   & 0b000001000 // 1 << 3
//   -------------
//     00000000000 //  means False - bit was not set

// """
