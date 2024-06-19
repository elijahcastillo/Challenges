#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t* createMemory(size_t sizeInBytes){
	uint8_t* mem = malloc(sizeof(uint8_t) * sizeInBytes);
	if(!mem) return NULL;
	return mem;
}

typedef enum {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
	IP,
	ACC,
    NUM_REGISTERS  // Trick to give us total
} RegisterName;

typedef struct {
	uint8_t* memory;
	uint16_t* registers;
	int registerCount;
} CPU;

uint16_t getUint16(uint8_t* mem, uint16_t addr){
	uint8_t firstByte = mem[addr]; 
	uint8_t secondByte = mem[addr + 1]; 

	uint16_t value = ((secondByte << 8) | firstByte);
	return value;
}

int cpuInitialize(CPU* cpu, size_t memorySize){
	cpu->memory = createMemory(memorySize);
	if(!cpu->memory) return -1;

	cpu->registerCount = NUM_REGISTERS;
	cpu->registers = malloc(sizeof(uint16_t)*NUM_REGISTERS);
	if(!cpu->registers){
		free(cpu->memory);
		cpu->memory = NULL;
		return -1;
	}

    // Initialize registers to 0
    for (int i = 0; i < NUM_REGISTERS; ++i) {
        cpu->registers[i] = 0;
    }

	return 0;
}

void cpuFree(CPU* cpu) {
    if (cpu->memory) free(cpu->memory);
    if (cpu->registers) free(cpu->registers);
}

uint16_t cpuGetRegister(CPU* cpu, int registerIndex){
	if(registerIndex >= cpu->registerCount){
		printf("Unable to get register: Invalid index %d\n", registerIndex);
		return 0;
	}

	return cpu->registers[registerIndex];
}

void cpuSetRegister(CPU* cpu, int registerIndex, uint16_t value){
	if(registerIndex >= cpu->registerCount){
		printf("Unable to set register: Invalid index %d\n", registerIndex);
	}

	cpu->registers[registerIndex] = value;
}

uint8_t cpuFetch(CPU* cpu){
	uint16_t instructionAddress = cpuGetRegister(cpu, IP);
	uint8_t instruction = cpu->memory[instructionAddress];
	cpuSetRegister(cpu, IP, instructionAddress + 1);
	return instruction;
}

void cpuExecute(CPU* cpu, uint8_t instruction){
	switch(instruction){
		case 0x01: 
			break;
	}
	
}

int main(){
    CPU cpu;
    if (cpuInitialize(&cpu, 1024) != 0) { 
        printf("Failed to initialize CPU\n");
        return -1;
    }

    // Set some values in memory
    cpu.memory[0] = 0x12;
    cpu.memory[1] = 0x34;

    // Retrieve the 16-bit value from memory address 0
    uint16_t value = getUint16(cpu.memory, 0);
    printf("16-bit value at address 0: 0x%04X\n", value);

    // Expected output: 0x3412
    // Explanation: 0x34 is the high byte and 0x12 is the low byte, resulting in 0x3412

    cpuFree(&cpu);  // Free the allocated memory
    return 0;
	
}
