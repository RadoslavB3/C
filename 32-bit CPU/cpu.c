// There is a bug on the line below. Figure it out, or kontr compilation fails.

#include "cpu.h"

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define OneBlock 1024

enum Operation
{
    adding,
    multiplication,
    dividing
};

int32_t *cpuCreateMemory(FILE *program, size_t stackCapacity, int32_t **stackBottom)
{
    assert(program != NULL);
    assert(stackBottom != NULL);
    int32_t ch[1] = {0};
    uint8_t control = 0;
    size_t alloc_size = 0;
    size_t current_index = 0;
    int32_t *memory = NULL;
    while ((control = (fread(ch, 1, 4, program))) == 4) {
        if (current_index + stackCapacity + 1 > alloc_size) {
            alloc_size += OneBlock;
            int32_t *buffer = memory;
            memory = realloc(memory, alloc_size * sizeof(int32_t));
            if (!memory) {
                free(buffer);
                return NULL;
            }
            memset(memory + alloc_size - OneBlock, 0, OneBlock * sizeof(int32_t));
        }
        *(memory + current_index) = ch[0];
        current_index++;
    }
    if (control != 0) {
        free(memory);
        return NULL;
    }
    if ((memory == NULL && stackCapacity != 0) || current_index + stackCapacity > alloc_size) {
        while (stackCapacity > alloc_size) {
            alloc_size += OneBlock;
            int32_t *buffer = memory;
            memory = realloc(memory, alloc_size * sizeof(int32_t));
            if (!memory) {
                free(buffer);
                return NULL;
            }
            memset(memory + alloc_size - OneBlock, 0, OneBlock * sizeof(int32_t));
        }
    } else if (memory == NULL) {
        return NULL;
    }
    *stackBottom = memory + alloc_size - 1;
    return memory;
}

void cpuCreate(struct cpu *cpu, int32_t *memory, int32_t *stackBottom, size_t stackCapacity)
{
    assert(memory != NULL);
    assert(cpu != NULL);
    assert(stackBottom != NULL);
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
#ifdef BONUS_JMP
    cpu->result = 0;
#endif
    cpu->status = 0;
    cpu->stackSize = 0;
    cpu->instructionPointer = 0;
    cpu->memory = memory;
    cpu->stackBottom = stackBottom;
    cpu->stackLimit = stackBottom - stackCapacity;
}

void cpuDestroy(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
#ifdef BONUS_JMP
    cpu->result = 0;
#endif
    cpu->status = 0;
    cpu->stackSize = 0;
    cpu->instructionPointer = 0;
    free(cpu->memory);
    cpu->memory = NULL;
    cpu->stackBottom = NULL;
    cpu->stackLimit = NULL;
}

void cpuReset(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
#ifdef BONUS_JMP
    cpu->result = 0;
#endif
    cpu->status = 0;
    cpu->stackSize = 0;
    cpu->instructionPointer = 0;
    int32_t *bottom = cpu->stackBottom;
    while (cpu->stackLimit != bottom) {
        *bottom = 0;
         bottom--;
    }
}

int cpuStatus(struct cpu *cpu)
{
    assert(cpu != NULL);
    return cpu->status;
}

int32_t cpuPeek(struct cpu *cpu, char reg)
{
    assert(cpu != NULL);
    switch (reg) {
        case 'A':
            return cpu->A;
        case 'B':
            return cpu->B;
        case 'C':
            return cpu->C;
        case 'D':
            return cpu->D;
#ifdef BONUS_JMP
            case 'R': return cpu->result;
#endif
        case 'S':
            return cpu->stackSize;
        case 'I':
            return cpu->instructionPointer;
        default:
            return 0;
    }
}

static int checkMemory(struct cpu *cpu, int instructions)
{
    if (cpu->memory + cpu->instructionPointer + instructions > cpu->stackLimit) {
        cpu->status = cpuInvalidAddress;
        return 0;
    }
    if (cpu->memory + cpu->instructionPointer + instructions > cpu->stackBottom) {
        cpu->status = cpuInvalidAddress;
        return 0;
    }
    if (cpu->memory + cpu->instructionPointer + instructions < cpu->memory) {
        cpu->status = cpuInvalidAddress;
        return 0;
    }
    return 1;
}

static int32_t *get_reg(struct cpu *cpu, int reg, bool use_result)
{
    switch (reg) {
        case 0: {
            return &cpu->A;
        }
        case 1: {
            return &cpu->B;
        }
        case 2: {
            return &cpu->C;
        }
        case 3: {
            return &cpu->D;
        }
        case 4: {
            if (use_result) {
#ifdef BONUS_JMP
                return &cpu->result;
#endif
            }
            cpu->status = cpuIllegalOperand;
            return NULL;
        }
        default: {
            cpu->status = cpuIllegalOperand;
            return NULL;
        }
    }
}

static int numeric_operation(struct cpu *cpu, int reg, int diff, enum Operation op)
{
    int32_t *r = get_reg(cpu , reg, false);
    if (r == NULL) {
        return 0;
    }
    switch (op) {
        case adding: {
            cpu->A += *r * diff;
            break;
        }
        case multiplication: {
            cpu->A *= *r;
            break;
        }
        case dividing: {
            if (*r == 0) {
                cpu->status = cpuDivByZero;
                return 0;
            }
            cpu->A /= *r;
            break;
        }
        default: {
            break;
        }
    }
#ifdef BONUS_JMP
    cpu->result = cpu->A;
#endif
    return 1;
}

static int inc(struct cpu *cpu, int reg, int diff)
{
    int32_t *r = get_reg(cpu, reg, false);
    if (r == NULL) {
        return 0;
    }
    *r += diff * 1;
#ifdef BONUS_JMP
    cpu->result = *r;
#endif
    return 1;
}

static int loop(struct cpu *cpu, int index)
{
    if (cpu->C != 0) {
        cpu->instructionPointer = index;
        return 1;
    }
    return 0;
}

static int mover(struct cpu *cpu, int reg, int num)
{
    int32_t *r = get_reg(cpu, reg, false);
    if (r == NULL) {
        return 0;
    }
    *r = num;
    return 1;
}

static int load(struct cpu *cpu, int reg, int num)
{
    int index = -cpu->stackSize + cpu->D + num + 1;
    if (cpu->stackBottom + index <= cpu->stackBottom - cpu->stackSize) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    if (cpu->stackBottom + index > cpu->stackBottom) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    int32_t *r = get_reg(cpu, reg, false);
    if (r == NULL) {
        return 0;
    }
    *r = *(cpu->stackBottom + index);
    return 1;
}

static int store(struct cpu *cpu, int reg, int num)
{
    int index = -cpu->stackSize + cpu->D + num + 1;
    if (cpu->stackBottom + index <= cpu->stackBottom - cpu->stackSize) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    if (cpu->stackBottom + index > cpu->stackBottom) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    int32_t *r = get_reg(cpu, reg, true);
    if (r == NULL) {
        return 0;
    }
    *(cpu->stackBottom + index) = *r;
    return 1;
}

static int in(struct cpu *cpu, int reg)
{
    int32_t num = 0;
    int control = scanf(" %d", &num);
    if (control == 0) {
        cpu->status = cpuIOError;
        return 0;
    }
    if (control == -1) {
        cpu->C = 0;
    }
    int32_t *r = get_reg(cpu, reg, false);
    if (r == NULL) {
        return 0;
    }
    if (control == -1) {
        *r = -1;
    }
    else {
        *r = num;
    }
    return 1;
}

static int get(struct cpu *cpu, int reg)
{
    int ch = 0;
    ch = getchar();
    if (ch == EOF) {
        cpu->C = 0;
    }
    int32_t *r = get_reg(cpu, reg, false);
    if (r == NULL) {
        return 0;
    }
    if (ch == EOF) {
        *r = -1;
    }
    else {
        *r = ch;
    }
    return 1;
}

static int out(struct cpu *cpu, int reg)
{
    int32_t *r = get_reg(cpu, reg, true);
    if (r == NULL) {
        return 0;
    }
    printf("%d", *r);
    return 1;
}

static int put(struct cpu *cpu, int reg)
{
    int32_t *r = get_reg(cpu, reg, true);
    if (r == NULL) {
        return 0;
    }
    if ((*r >= 0) && (*r < 256)) {
        printf("%c", *r);
        return 1;
    }
    cpu->status = cpuIllegalOperand;
    return 0;
}

static int swap(struct cpu *cpu, int reg1, int reg2)
{
    int32_t *r1 = get_reg(cpu, reg1, false);
    if (r1 == NULL) {
        return 0;
    }
    int32_t *r2 = get_reg(cpu, reg2, false);
    if (r2 == NULL) {
        return 0;
    }
    int32_t aux = *r1;
    *r1 = *r2;
    *r2 = aux;
    return 1;
}

static int push(struct cpu *cpu, int reg)
{
    int32_t *r = get_reg(cpu, reg, true);
    if (r == NULL) {
        return 0;
    }
    int32_t num = *r;
    if (cpu->stackSize == 0 && cpu->stackBottom > cpu->stackLimit) {
        *(cpu->stackBottom) = num;
        return 1;
    }
    if (cpu->stackBottom - cpu->stackSize <= cpu->stackLimit) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    *(cpu->stackBottom - cpu->stackSize) = num;
    return 1;
}

static int pop(struct cpu *cpu, int reg) {
    if (cpu->stackSize <= 0) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    int32_t num = *(cpu->stackBottom - cpu->stackSize + 1);
    int32_t *r = get_reg(cpu, reg, false);
    if (r == NULL) {
        return 0;
    }
    *r = num;
    return 1;
}

#ifdef BONUS_JMP
static int cmp(struct cpu *cpu, int reg1, int reg2)
{
    int32_t *r1 = get_reg(cpu, reg1, true);
    if (r1 == NULL) {
        return 0;
    }
    int32_t *r2 = get_reg(cpu, reg2, true);
    if (r2 == NULL) {
        return 0;
    }
    cpu->result = *r1 - *r2;
    return 1;
}
#endif


int cpuStep(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->status != cpuOK) {
        return 0;
    }
    if (checkMemory(cpu, 0) != 1) {
        return 0;
    }
    switch (*(cpu->memory + cpu->instructionPointer)) {
        case 0: {
            cpu->instructionPointer++;
            return 1;
        }
        case 1: {
            cpu->status = cpuHalted;
            cpu->instructionPointer++;
            return 0;
        }
        case 2: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (numeric_operation(cpu, *(cpu->memory + cpu->instructionPointer + 1), 1, adding) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 3: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (numeric_operation(cpu, *(cpu->memory + cpu->instructionPointer + 1), -1, adding) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 4: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (numeric_operation(cpu, *(cpu->memory + cpu->instructionPointer + 1), 1, multiplication) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 5: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (numeric_operation(cpu, *(cpu->memory + cpu->instructionPointer + 1), 1, dividing) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 6: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (inc(cpu, *(cpu->memory + cpu->instructionPointer + 1), 1) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 7: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (inc(cpu, *(cpu->memory + cpu->instructionPointer + 1), -1) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 8: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (loop(cpu, *(cpu->memory + cpu->instructionPointer + 1)) != 1) {
                cpu->instructionPointer += 2;
            }
            return 1;
        }
        case 9: {
            if (checkMemory(cpu, 2) != 1) {
                return 0;
            }
            if (mover(cpu, *(cpu->memory + cpu->instructionPointer + 1),
                      *(cpu->memory + cpu->instructionPointer + 2)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 3;
            return 1;
        }
        case 10: {
            if (checkMemory(cpu, 2) != 1) {
                return 0;
            }
            if (load(cpu, *(cpu->memory + cpu->instructionPointer + 1), *(cpu->memory + cpu->instructionPointer + 2)) !=
                1) {
                return 0;
            }
            cpu->instructionPointer += 3;
            return 1;
        }
        case 11: {
            if (checkMemory(cpu, 2) != 1) {
                return 0;
            }
            if (store(cpu, *(cpu->memory + cpu->instructionPointer + 1),
                      *(cpu->memory + cpu->instructionPointer + 2)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 3;
            return 1;
        }
        case 12: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (in(cpu, *(cpu->memory + cpu->instructionPointer + 1)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 13: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (get(cpu, *(cpu->memory + cpu->instructionPointer + 1)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 14: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (out(cpu, *(cpu->memory + cpu->instructionPointer + 1)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 15: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (put(cpu, *(cpu->memory + cpu->instructionPointer + 1)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            return 1;
        }
        case 16: {
            if (checkMemory(cpu, 2) != 1) {
                return 0;
            }
            if (swap(cpu, *(cpu->memory + cpu->instructionPointer + 1), *(cpu->memory + cpu->instructionPointer + 2)) !=
                1) {
                return 0;
            }
            cpu->instructionPointer += 3;
            return 1;
        }
        case 17: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (push(cpu, *(cpu->memory + cpu->instructionPointer + 1)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            cpu->stackSize++;
            return 1;
        }
        case 18: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (pop(cpu, *(cpu->memory + cpu->instructionPointer + 1)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 2;
            cpu->stackSize--;
            return 1;
        }
#ifdef BONUS_JMP
        case 19: {
            if (checkMemory(cpu, 2) != 1) {
                return 0;
            }
            if (cmp(cpu, *(cpu->memory + cpu->instructionPointer + 1), *(cpu->memory + cpu->instructionPointer + 2)) != 1) {
                return 0;
            }
            cpu->instructionPointer += 3;
            return 1;
        }
        case 20: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            cpu->instructionPointer = *(cpu->memory + cpu->instructionPointer + 1);
            return 1;
        }
        case 21: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (cpu->result == 0) {
                cpu->instructionPointer = *(cpu->memory + cpu->instructionPointer + 1);
            }
            else {
                cpu->instructionPointer += 2;
            }
            return 1;
        }
        case 22: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (cpu->result != 0) {
                cpu->instructionPointer = *(cpu->memory + cpu->instructionPointer + 1);
            }
            else {
                cpu->instructionPointer += 2;
            }
            return 1;
        }
        case 23: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (cpu->result > 0) {
                cpu->instructionPointer = *(cpu->memory + cpu->instructionPointer +1);
            }
            else {
                cpu->instructionPointer += 2;
            }
            return 1;
        }
#endif
#ifdef BONUS_CALL
        case 24: {
            if (checkMemory(cpu, 1) != 1) {
                return 0;
            }
            if (cpu->stackBottom - cpu->stackSize <= cpu->stackLimit) {
                cpu->status = cpuInvalidStackOperation;
                return 0;
            }
            *(cpu->stackBottom - cpu->stackSize) = cpu->instructionPointer + 2;
            cpu->instructionPointer = *(cpu->memory + cpu->instructionPointer +1);
            cpu->stackSize++;
            return 1;
        }
        case 25: {
            if (cpu->stackSize == 0) {
                cpu->status = cpuInvalidStackOperation;
                return 0;
            }
            cpu->instructionPointer = *(cpu->stackBottom - cpu->stackSize + 1);
            cpu->stackSize--;
            return 1;
        }
#endif
        default: {
            cpu->status = cpuIllegalInstruction;
            return 0;
        }
    }
}

int cpuRun(struct cpu *cpu, size_t steps)
{
    assert(cpu != NULL);
    if (cpu->status != cpuOK) {
        return 0;
    }
    int count = 0;
    for (size_t i = 1; i <= steps; i++) {
        count++;
        if (cpuStep(cpu) == 0) {
            if (cpu->status != cpuHalted) {
                return -count;
            }
            return count;
        }
    }
    return count;
}

