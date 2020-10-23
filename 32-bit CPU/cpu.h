/* Represents a 32-bit processor */
#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>


enum cpuStatus
{
    cpuOK,
    cpuHalted,
    cpuIllegalInstruction,
    cpuIllegalOperand,
    cpuInvalidAddress,
    cpuInvalidStackOperation,
    cpuDivByZero,
    cpuIOError
};

struct cpu
{
    int32_t A;
    int32_t B;
    int32_t C;
    int32_t D;
#ifdef BONUS_JMP
    int32_t result;
#endif
    enum cpuStatus status;
    int stackSize;
    int instructionPointer;
    int32_t *memory;
    int32_t *stackBottom;
    int32_t *stackLimit;
};

int32_t *cpuCreateMemory(FILE *program, size_t stackCapacity, int32_t **stackBottom);

void cpuCreate(struct cpu *cpu, int32_t *memory, int32_t *stackBottom, size_t stackCapacity);

void cpuDestroy(struct cpu *cpu);

void cpuReset(struct cpu *cpu);

int cpuStatus(struct cpu *cpu);

int32_t cpuPeek(struct cpu *cpu, char reg);

int cpuStep(struct cpu *cpu);

int cpuRun(struct cpu *cpu, size_t steps);




#endif
