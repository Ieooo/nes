#include "instruction.h"
#include "cpu.h"
#include "memory.h"


// load Accumulator with Data Stored in Memory
// affect N Z
int LDA(exector_arg_t arg) {
    register_t * reg = get_reg();
    uint8_t value = 0;
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte((uint16_t)arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    case INDIRECT_X:
        uint16_t addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        value = get_byte(addr);
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        value = get_byte(addr);
        break;
    default:
        return -1;
    }
    reg->A = value;
    p |= value & FlagNegative;
    if (value == 0) p |= FlagZero;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// load X register with Data Stored in Memory
// affect N Z
// add 1 clock cycle when page boundary is crossed
int LDX(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte((uint16_t)arg.other[0]);
        break;
    case ZERO_PAGE_Y:
        value = get_byte((uint16_t)((arg.other[0] + reg->Y) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    default:
        return -1;
    } 
    reg->X = value;
    p |= value & FlagNegative;
    if (value == 0) p |= FlagZero; 
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// load Y register with Data Stored in Memory
// affect N Z
int LDY(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:  // LDA #$nn
        value = arg.other[0];
        break;
    case ZERO_PAGE:  // LDA $nn
        value = get_byte((uint16_t)arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    default:
        return -1;
    }
    reg->Y = value;
    p |= value & FlagNegative;
    if (value == 0) p |= FlagZero; 
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// store A register with Data Stored in Memory(A->M)
int STA(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint16_t addr = 0;
    switch (arg.instr.mode)
    {
    case ZERO_PAGE:
        addr = (uint16_t)arg.other[0];
        break;
    case ZERO_PAGE_X:
        addr = (uint16_t)((arg.other[0] + reg->X) & 0xFF);
        break;
    case ABSOLUTE:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case ABSOLUTE_X:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X;
        break;
    case ABSOLUTE_Y:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y;
        break;
    case INDIRECT_X:
        addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        break;
    default:
        return -1;
    }
    set_byte(addr, reg->A);
    reg->PC += arg.instr.length;
    return 0;
}

// store X register with Data Stored in Memory(X->M)
int STX(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint16_t addr = 0;
    switch (arg.instr.mode)
    {
    case ZERO_PAGE:
        addr = (uint16_t)arg.other[0];
        break;
    case ZERO_PAGE_Y:
        addr = (uint16_t)((arg.other[0] + reg->Y) & 0xFF);
        break;
    case ABSOLUTE:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    default:
        return -1;
    }
    set_byte(addr, reg->X);
    reg->PC += arg.instr.length;
    return 0;
}

// store Y register with Data Stored in Memory
int STY(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint16_t addr = 0;
    switch (arg.instr.mode)
    {
    case ZERO_PAGE:
        addr = (uint16_t)arg.other[0];
        break;
    case ZERO_PAGE_X:
        addr = (uint16_t)((arg.other[0] + reg->X) & 0xFF);
        break;
    case ABSOLUTE:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    default:
        return -1;
    }
    set_byte(addr, reg->Y);
    reg->PC += arg.instr.length;
    return 0;
}

/*
    uint8_t c = (reg->P & FlagCarry) == FlagCarry ? 1 : 0;
    if ((int16_t)(int8_t)reg->A + (int16_t)(int8_t)value + c > 127 || (int16_t)(int8_t)reg->A + (int16_t)(int8_t)value + c < -128) 
        p |= FlagOverflow;
    // BCD Mode
    if (reg->P & FlagDecimalMode == 0) {
        if (reg->A + value + c > 0xFF) p |= FlagCarry;
        reg->A += value + c;
        p |= reg->A & FlagNegative;
    } else { // BCD mode
        uint8_t low = (reg->A & 0x0F) + (value & 0x0F) + c;
        if (low >= 0x0A) {
            low += 0x06;
            c = 1;
        }
        uint8_t high = (reg->A & 0xF0) + (value & 0xF0) + c;
        if (high >= 0xA0) {
            high += 0x06;
            p |= FlagCarry;
        }
        reg->A = high + low;
    }
    if (reg->A == 0) p |= FlagZero;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
*/
// Add memory to Accumulator with carry
// affect N Z C
int ADC(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint8_t p = reg->P & ~FlagCarry & ~FlagNegative & ~FlagZero & ~FlagOverflow;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte(arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    case INDIRECT_X:
        uint16_t addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        value = get_byte(addr);
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        value = get_byte(addr);
        break;
    default:
        return -1;
    }
    uint8_t c = (reg->P & FlagCarry) == FlagCarry ? 1 : 0;
    if ((int16_t)(int8_t)reg->A + (int16_t)(int8_t)value + c > 127 || (int16_t)(int8_t)reg->A + (int16_t)(int8_t)value + c < -128) 
        p |= FlagOverflow;
    // BCD Mode
    if ((reg->P & FlagDecimalMode) == 0) {
        if (reg->A + value + c > 0xFF) p |= FlagCarry;
        reg->A += value + c;
        p |= reg->A & FlagNegative;
    } else { // BCD mode
        uint8_t low = (reg->A & 0x0F) + (value & 0x0F) + c;
        if (low >= 0x0A) {
            low = (low + 0x06) & 0x0F;
            c = 1;
        } else {
            c = 0;
        }
        uint16_t high = ((reg->A >> 4) & 0x0F) + ((value >> 4) & 0x0F) + c;
        if (high >= 0x0A) {
            high = (high + 0x06) & 0x0F;
            p |= FlagCarry;
        }
        reg->A = (high << 4) + low;
    }
    if (reg->A == 0) p |= FlagZero;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

/*
        uint8_t c = (reg->P & FlagCarry) == 0 ? 1 : 0;
    if ((int16_t)(int8_t)reg->A - (int16_t)(int8_t)value - c > 127 || (int16_t)(int8_t)reg->A - (int16_t)(int8_t)value - c < -128) 
        p |= FlagOverflow;
    // BCD Mode
    if (reg->P & FlagDecimalMode == 0) {
        if (reg->A - value - c >= 0) p |= FlagCarry;
        reg->A = reg->A - value - c;
        p |= reg->A & FlagNegative;
    } else {
        uint8_t low = 0; 
        uint8_t high = 0;
        if ((reg->A & 0x0F) < (value & 0x0F) + c) {
            low = (reg->A & 0x0F) + 0x0A - (value & 0x0F) - c;
            c = 1;
        }
        if ((reg->A & 0xF0) < (value & 0xF0) + c) {
            high = (uint8_t)((uint16_t)(reg->A & 0xF0) + 0xA0 - (value & 0xF0) - c);
        } else {
            p |= FlagCarry;
        }
        reg->A = high + low;
    }
    if (reg->A == 0) p |= FlagZero;
    reg->P = p;
    reg->PC += arg.instr.length;
    return res_code;
*/
// Subtract memory from accumulator with borrow(A-M-~C->A),
// affect N Z C V
// add 1 clock cycle when page boundary is crossed
int SBC(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagCarry & ~FlagNegative & ~FlagZero & ~FlagOverflow;
    uint8_t value = 0;
    int res_code = 0;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte((uint16_t)arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    case INDIRECT_X:
        uint16_t addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        value = get_byte(addr);
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        value = get_byte(addr);
        break;
    default:
        return -1;
    }
    uint8_t c = (reg->P & FlagCarry) == 0 ? 1 : 0;
    if ((int16_t)(int8_t)reg->A - (int16_t)(int8_t)value - c > 127 || (int16_t)(int8_t)reg->A - (int16_t)(int8_t)value - c < -128) 
        p |= FlagOverflow;
    // BCD Mode
    if ((reg->P & FlagDecimalMode) == 0) {
        if (reg->A - value - c >= 0) p |= FlagCarry;
        reg->A = reg->A - value - c;
        p |= reg->A & FlagNegative;
    } else {
        uint8_t low = 0; 
        uint8_t high = 0;
        // 99 - 00 = 99
        if ((reg->A & 0x0F) < (value & 0x0F) + c) {
            low = (reg->A & 0x0F) + 0x0A - (value & 0x0F) - c;
            c = 1;
        } else {
            low = (reg->A & 0x0F) - (value & 0x0F) - c;
            c = 0;
        }
        if (((reg->A >> 4) & 0x0F) < ((value >> 4) & 0x0F) + c) {
            high = ((reg->A >> 4) & 0x0F) + 0x0A - ((value >> 4) & 0x0F) - c;
        } else {
            high = ((reg->A >> 4) & 0x0F) - ((value >> 4) & 0x0F) - c;
            p |= FlagCarry;
        }
        reg->A = (high << 4) + low;
    }
    if (reg->A == 0) p |= FlagZero;
    reg->P = p;
    reg->PC += arg.instr.length;
    return res_code;
}

// Increment memory by one,
// affect N Z
int INC(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    uint16_t addr = 0;
    switch (arg.instr.mode)
    {
    case ZERO_PAGE:
        addr = arg.other[0];
        break;
    case ZERO_PAGE_X:
        addr = (uint16_t)((arg.other[0] + reg->X) & 0xFF);
        break;
    case ABSOLUTE:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case ABSOLUTE_X:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X;
        break;
    default:
        return -1;
    }
    uint8_t value = get_byte(addr);
    value += 1;
    if (value == 0) p |= FlagZero;
    p |= value & FlagNegative;
    set_byte(addr, value);
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Decrement memory by one,
// affect N Z
int DEC(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    uint16_t addr = 0;
    switch (arg.instr.mode)
    {
    case ZERO_PAGE:
        addr = arg.other[0];
        break;
    case ZERO_PAGE_X:
        addr = (uint16_t)((arg.other[0] + reg->X) & 0xFF);
        break;
    case ABSOLUTE:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case ABSOLUTE_X:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X;
        break;
    default:
        return -1;
    }
    uint8_t value = get_byte(addr);
    value -= 1;
    if (value == 0) p |= FlagZero;
    p |= value & FlagNegative;
    set_byte(addr, value);
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Increment Index X by one,
// affect N Z
int INX(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    reg->X += 1;
    if (reg->X == 0) p |= FlagZero;
    p |= FlagNegative & reg->X;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Decrement index X by one(X=X-1),
// affect N Z
int DEX(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    reg->X -= 1;
    if (reg->X == 0) p |= FlagZero;
    p |= FlagNegative & reg->X;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Increment Index Y by one,
// affect N Z
int INY(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    reg->Y += 1;
    if (reg->Y == 0) p |= FlagZero;
    p |= FlagNegative & reg->Y;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Decrement index Y by one,
// affect N Z
int DEY(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagZero & ~FlagNegative;
    reg->Y -= 1;
    if (reg->Y == 0) p |= FlagZero;
    p |= FlagNegative & reg->Y;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Compare memory and accumulator(A-M)
// affect N Z C
// add 1 if page boundary is crossed
int CMP(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagCarry & ~FlagNegative & ~FlagZero;
    uint8_t value = 0;
    int res_code = 0;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte((uint16_t)arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    case INDIRECT_X:
        uint16_t addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        value = get_byte(addr);
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        value = get_byte(addr);
        break;
    default:
        return -1;
    }
    if (reg->A >= value) p |= FlagCarry; // no borrow
    if (reg->A == value) p |= FlagZero;
    p |= (reg->A - value) & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return res_code;
}

// Compare Memory and Index X(X-M),
// affecct N Z C
int CPX(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagCarry & ~FlagNegative & ~FlagZero;
    uint8_t value = 0;
    int res_code = 0;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte((uint16_t)arg.other[0]);
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    default:
        return -1;
    }
    if (reg->X >= value) p |= FlagCarry; // no borrow
    if (reg->X == value) p |= FlagZero;
    p |= (reg->X -value) & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return res_code;
}

// Compare Memory and Index Y(Y-M),
// affecct N Z C
int CPY(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagCarry & ~FlagNegative & ~FlagZero;
    uint8_t value = 0;
    int res_code = 0;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte((uint16_t)arg.other[0]);
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    default:
        return -1;
    }
    if (reg->Y >= value) p |= FlagCarry; // no borrow
    if (reg->Y == value) p |= FlagZero;
    p |= (reg->Y - value) & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return res_code;
}

// AND memory with Accumulator,
// affect N Z
int AND(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte(arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    case INDIRECT_X:
        uint16_t addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        value = get_byte(addr);
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        value = get_byte(addr);
        break;
    default: 
        return -1;
    }
    reg->A &= value;
    if (reg->A == 0) p |= FlagZero;
    p |= reg->A & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0; 
}

// Or memory with accumulator(A|M->M)
// affect N Z
int ORA(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte(arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    case INDIRECT_X:
        uint16_t addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        value = get_byte(addr);
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        value = get_byte(addr);
        break;
    default: 
        return -1;
    }
    reg->A |= value;
    if (reg->A == 0) p |= FlagZero;
    p |= reg->A & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0; 
}

// Exclusive-Or memory with accumulator(A^M->A)
// affect N Z
int EOR(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    switch (arg.instr.mode)
    {
    case IMMEDIATE:
        value = arg.other[0];
        break;
    case ZERO_PAGE:
        value = get_byte(arg.other[0]);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    case ABSOLUTE_X:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    case ABSOLUTE_Y:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->Y);
        break;
    case INDIRECT_X:
        uint16_t addr = ((uint16_t)get_byte((uint16_t)((arg.other[0] + reg->X + 1) & 0xFF)) << 8) + get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        value = get_byte(addr);
        break;
    case INDIRECT_Y:
        addr = (get_byte((uint16_t)((arg.other[0] + 1) & 0xFF)) << 8) + get_byte((uint16_t)(arg.other[0])) + reg->Y;
        value = get_byte(addr);
        break;
    default: 
        return -1;
    }
    reg->A ^= value;
    if (reg->A == 0) p |= FlagZero;
    p |= reg->A & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0; 
}

// Shift Left One Bit(Memory or Accumulator),
// affect N Z C
int ASL(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint16_t addr = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero & ~FlagCarry;
    switch (arg.instr.mode)
    {
    case REGISTER:
        value = reg->A;
        break;
    case ZERO_PAGE:
        addr = arg.other[0];
        value = get_byte(addr);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        addr = (uint16_t)(arg.other[0] + reg->X);
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case ABSOLUTE_X:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X;
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    default:
        return -1;
    }
    if ((value & 0x80) == 0x80) p |= FlagCarry;
    value <<= 1;
    if (value == 0) p |= FlagZero;
    p |= value & FlagNegative;
    reg->P = p;
    if (arg.instr.mode == REGISTER) reg->A = value; else set_byte(addr, value);
    reg->PC += arg.instr.length;
    return 0; 
}

// Shift right one bit(memory or accumulator)
// affect Z C
int LSR(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint16_t addr = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero & ~FlagCarry;
    switch (arg.instr.mode)
    {
    case REGISTER:
        value = reg->A;
        break;
    case ZERO_PAGE:
        addr = arg.other[0];
        value = get_byte(addr);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)((arg.other[0] + reg->X) & 0xFF));
        addr = (uint16_t)(arg.other[0] + reg->X);
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case ABSOLUTE_X:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X;
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    default:
        return -1;
    }
    if ((value & 0x01) == 0x01) p |= FlagCarry;
    value >>= 1;
    if (value == 0) p |= FlagZero;
    if (arg.instr.mode == REGISTER) reg->A = value; else set_byte(addr, value);
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0; 
}

// Rotate one bit left(memory or accumulator)
// affect N Z V
int ROL(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint16_t addr = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero & ~FlagCarry;
    switch (arg.instr.mode)
    {
    case REGISTER:
        value = reg->A;
        break;
    case ZERO_PAGE:
        addr = arg.other[0];
        value = get_byte(addr);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)(arg.other[0] + reg->X) & 0xFF);
        addr = (uint16_t)(arg.other[0] + reg->X);
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case ABSOLUTE_X:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X;
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    default:
        return -1;
    }
    if ((value & 0x80) == 0x80) p |= FlagCarry;
    value = (value << 1) + (reg->P & FlagCarry == FlagCarry ? 1 : 0);
    if (value == 0) p |= FlagZero;
    p |= value & FlagNegative;
    if (arg.instr.mode == REGISTER) reg->A = value; else set_byte(addr, value);
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0; 
}

// Rotate one bit right(memory or accumulator)
// affect N Z V
int ROR(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint16_t addr = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero & ~FlagCarry;
    switch (arg.instr.mode)
    {
    case REGISTER:
        value = reg->A;
        break;
    case ZERO_PAGE:
        addr = arg.other[0];
        value = get_byte(addr);
        break;
    case ZERO_PAGE_X:
        value = get_byte((uint16_t)(arg.other[0] + reg->X) & 0xFF);
        addr = (uint16_t)(arg.other[0] + reg->X);
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case ABSOLUTE_X:
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X;
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0] + reg->X);
        break;
    default: 
        return -1;
    }
    if ((value & 0x01) == 0x01) p |= FlagCarry;
    value = (value >> 1) + (reg->P & FlagCarry == FlagCarry ? 0x80 : 0);
    if (value == 0) p |= FlagZero;
    p |= value & FlagNegative;
    if (arg.instr.mode == REGISTER) reg->A = value; else set_byte(addr, value);
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0; 
}

// Test bits in memory with accumulator(A^M, M7->N, M6->V),
// bit 6 and 7 are transferred to the status register,
// if the result of A^M is zero then Z=1, otherwise Z=0,
// affect N Z V
int BIT(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t value = 0;
    uint16_t addr = 0;
    uint8_t p = reg->P & ~FlagNegative & ~FlagOverflow & ~FlagZero;
    switch (arg.instr.mode)
    {
    case ZERO_PAGE:
        addr = arg.other[0];
        value = get_byte(addr);
        break;
    case ABSOLUTE:
        value = get_byte(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        addr = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    default:
        return -1;
    }
    if ((value & reg->A) == 0) p |= FlagZero;
    if ((value & 0x80) == 0x80) p |= FlagNegative;
    if ((value & 0x40) == 0x40) p |= FlagOverflow;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0; 
}

// Jump to new location,
int JMP(exector_arg_t arg) {
    register_t* reg = get_reg();
    switch (arg.instr.mode)
    {
    case ABSOLUTE:
        reg->PC = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
        break;
    case INDIRECT:
        reg->PC = get_word(((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0]);
        break;
    default:
        return -1;
    }
    return 0;
}

// Jump to Subroutine with saving return address
int JSR(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length - 1;
    // push return address
    set_byte(STACK_TOP + reg->SP--, (uint8_t)(reg->PC >> 8));
    set_byte(STACK_TOP + reg->SP--, (uint8_t)reg->PC);
    reg->PC = ((uint16_t)arg.other[1] << 8) + (uint16_t)arg.other[0];
    return 0;
}

int RTS(exector_arg_t arg) {
    register_t* reg = get_reg();
    // pop return address
    uint8_t low_addr = get_byte(STACK_TOP + ++reg->SP);
    uint8_t high_addr = get_byte(STACK_TOP + ++reg->SP);
    reg->PC = ((uint16_t)high_addr << 8) + (uint16_t)low_addr + 1;
    return 0;
}

// Branch on Carry Clear(C=0)
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BCC(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;    
    if ((reg->P & FlagCarry) == 0) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}

// Branch on Carry Set(C=1)
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BCS(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;    
    if ((reg->P & FlagCarry) == FlagCarry) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}

// Branch if Equal (Z=1)
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BEQ(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;
    if ((reg->P & FlagZero) == FlagZero) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}

// Branch if Not Equal(Z=0)
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BNE(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;
    if ((reg->P & FlagZero) == 0) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}

// Branch if Minus(N=1)
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BMI(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;
    if ((reg->P & FlagNegative) == FlagNegative) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}

// Branch if result positive(N=0) 
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BPL(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;
    if ((reg->P & FlagNegative) == 0) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}

// Branch on Overflow clear(V=0)
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BVC(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;
    if ((reg->P & FlagOverflow) == 0) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}

// Branch on Overflow set(V=1)
// add 1 clock cycle if branch occures to same page,
// add 2 clock cycle if branch occurs to different page
int BVS(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;
    if ((reg->P & FlagOverflow) == FlagOverflow) reg->PC += (uint16_t)(int8_t)arg.other[0];
    return 0;
}


// Push accumulator on stack
int PHA(exector_arg_t arg) {
    register_t* reg = get_reg();
    set_byte(STACK_TOP + reg->SP, reg->A);
    reg->SP -= 1;
    reg->PC += arg.instr.length;
    return 0;
}

// Push Processor Status on stack
int PHP(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P |= FlagBreakCammand | FlagUnused;  // must be on(hardware control)
    set_byte(STACK_TOP + reg->SP, reg->P);
    reg->SP -= 1;
    reg->PC += arg.instr.length;
    return 0;
}

// Pull accumulator from stack
// affect N Z
int PLA(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    reg->SP += 1;
    reg->A = get_byte(STACK_TOP + reg->SP);
    if (reg->A == 0) p |= FlagZero;
    if ((reg->A & FlagNegative) == FlagNegative) p |= FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Pull Processor Status from stack
int PLP(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->SP += 1;
    reg->P = get_byte(STACK_TOP + reg->SP) | FlagUnused; // P5 always on
    reg->PC += arg.instr.length;
    return 0;
}

// Clear Carry flag
int CLC(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P &= ~FlagCarry;
    reg->PC += arg.instr.length;
    return 0;
}
// Set Carry flag
int SEC(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P |= FlagCarry;
    reg->PC += arg.instr.length;
    return 0;
}
// Clear Interrupt disable bit
int CLI(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P &= ~FlagInterruptDisabled;
    reg->PC += arg.instr.length;
    return 0;
}
// Set Interrupt disable bit
int SEI(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P |= FlagInterruptDisabled;
    reg->PC += arg.instr.length;
    return 0;
}
// Clear Overflow flag
int CLV(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P &= ~FlagOverflow;
    reg->PC += arg.instr.length;
    return 0;
}
// Clear Decimal mode
int CLD(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P &= ~FlagDecimalMode;
    reg->PC += arg.instr.length;
    return 0;
}
// Set Decimal flag
int SED(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->P |= FlagDecimalMode;
    reg->PC += arg.instr.length;
    return 0;
}

// Force Interrupt
int BRK(exector_arg_t arg) {
    register_t* reg = get_reg();
    // push pc to stack, 
    reg->PC += arg.instr.length + 1;
    set_byte(STACK_TOP + reg->SP--, (uint8_t)(reg->PC >> 8));
    set_byte(STACK_TOP + reg->SP--, (uint8_t)reg->PC);
    // push p register, set B=1, U=1
    uint8_t p = reg->P | FlagUnused;  // FlagUnused must be on(hardware control)
    if (reg->IRQ == 0) p |= FlagBreakCammand;
    set_byte(STACK_TOP + reg->SP--, p);
    // set p register
    reg->P |= FlagInterruptDisabled;  // disable interrupt
    // set pc from BRK interrupt vector
    reg->PC = get_word(INTERRUPT_VECTOR_BRK);
    return 0;
}

// Retrun from Interrupt
int RTI(exector_arg_t arg) {
    register_t* reg = get_reg();
    // pop p register
    reg->P = get_byte(STACK_TOP + ++reg->SP);
    reg->P &= ~FlagBreakCammand;
    // pop pc register
    uint8_t low_addr = get_byte(STACK_TOP + ++reg->SP);
    uint8_t high_addr = get_byte(STACK_TOP + ++reg->SP);
    reg->PC = (((uint16_t)high_addr << 8) + (uint16_t)low_addr);
    return 0;
}

// Transfer Index X to Stack Pointer
int TXS(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->SP = reg->X;
    reg->PC += arg.instr.length;
    return 0;
}
// Transfer Stack Pointer to Index X
// affect N Z
int TSX(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    reg->X = reg->SP;
    if (reg->X == 0) p |= FlagZero;
    p |= reg->X & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Transfer Index X to Accumulator
// affect N Z
int TXA(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    reg->A = reg->X;
    if (reg->A == 0) p |= FlagZero;
    p |= reg->A & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}
// Transfer Accumulator to Index X
// affect N Z
int TAX(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    reg->X = reg->A;
    if (reg->X == 0) p |= FlagZero;
    p |= reg->X & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Transfer Index Y to Accumulator
// affect N Z
int TYA(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    reg->A = reg->Y;
    if (reg->A == 0) p |= FlagZero;
    p |= reg->A & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// Transfer Accumulator to Index Y
int TAY(exector_arg_t arg) {
    register_t* reg = get_reg();
    uint8_t p = reg->P & ~FlagNegative & ~FlagZero;
    reg->Y = reg->A;
    if (reg->Y == 0) p |= FlagZero;
    p |= reg->Y & FlagNegative;
    reg->P = p;
    reg->PC += arg.instr.length;
    return 0;
}

// no operation
int NOP(exector_arg_t arg) {
    register_t* reg = get_reg();
    reg->PC += arg.instr.length;
    return 0;
}

// not define now
int NDF(exector_arg_t arg) { return 1; }

instruction_t instructions[256] = {
    { .name="BRK", .length=1, .cycle=7, .exec_func=BRK, .mode=IMPLIED },       // opcode: 0x00
    { .name="ORA", .length=2, .cycle=6, .exec_func=ORA, .mode=INDIRECT_X },    // opcode: 0x01
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x02
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x03
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x04
    { .name="ORA", .length=2, .cycle=3, .exec_func=ORA, .mode=ZERO_PAGE },     // opcode: 0x05, ORA Zpg
    { .name="ASL", .length=2, .cycle=5, .exec_func=ASL, .mode=ZERO_PAGE },     // opcode: 0x06, ASL Zpg
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x07
    { .name="PHP", .length=1, .cycle=1, .exec_func=PHP, .mode=IMPLIED },       // opcode: 0x08
    { .name="ORA", .length=2, .cycle=2, .exec_func=ORA, .mode=IMMEDIATE },     // opcode: 0x09, ORA imm
    { .name="ASL", .length=1, .cycle=2, .exec_func=ASL, .mode=REGISTER },      // opcode: 0x0A, ASL A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x0B
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x0C
    { .name="ORA", .length=3, .cycle=4, .exec_func=ORA, .mode=ABSOLUTE },      // opcode: 0x0D
    { .name="ASL", .length=3, .cycle=6, .exec_func=ASL, .mode=ABSOLUTE },      // opcode: 0x0E, ASL Abs
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x0F
    { .name="BPL", .length=2, .cycle=2, .exec_func=BPL, .mode=RELATIVE },      // opcode: 0x10
    { .name="ORA", .length=2, .cycle=5, .exec_func=ORA, .mode=INDIRECT_Y },    // opcode: 0x11
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x12
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x13
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x14
    { .name="ORA", .length=2, .cycle=4, .exec_func=ORA, .mode=ZERO_PAGE_X },   // opcode: 0x15
    { .name="ASL", .length=2, .cycle=6, .exec_func=ASL, .mode=ZERO_PAGE_X },   // opcode: 0x16, ASL ZpgX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x17
    { .name="CLC", .length=1, .cycle=2, .exec_func=CLC, .mode=IMPLIED },       // opcode: 0x18
    { .name="ORA", .length=3, .cycle=4, .exec_func=ORA, .mode=ABSOLUTE_Y },    // opcode: 0x19
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x1A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x1B
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x1C
    { .name="ORA", .length=3, .cycle=4, .exec_func=ORA, .mode=ABSOLUTE_X },    // opcode: 0x1D
    { .name="ASL", .length=3, .cycle=7, .exec_func=ASL, .mode=ABSOLUTE_X },    // opcode: 0x1E, AND AbsX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x1F
    { .name="JSR", .length=3, .cycle=6, .exec_func=JSR, .mode=ABSOLUTE },      // opcode: 0x20, JSR Abs
    { .name="AND", .length=2, .cycle=6, .exec_func=AND, .mode=INDIRECT_X },    // opcode: 0x21, AND IndX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x22
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x23
    { .name="BIT", .length=2, .cycle=3, .exec_func=BIT, .mode=ZERO_PAGE },     // opcode: 0x24, BIT Zpg
    { .name="AND", .length=2, .cycle=3, .exec_func=AND, .mode=ZERO_PAGE },     // opcode: 0x25, AND Zpg
    { .name="ROL", .length=2, .cycle=5, .exec_func=ROL, .mode=ZERO_PAGE },     // opcode: 0x26, ROL Zpg
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x27
    { .name="PLP", .length=1, .cycle=4, .exec_func=PLP, .mode=IMPLIED },       // opcode: 0x28
    { .name="AND", .length=2, .cycle=2, .exec_func=AND, .mode=IMMEDIATE },     // opcode: 0x29, AND imm
    { .name="ROL", .length=1, .cycle=2, .exec_func=ROL, .mode=REGISTER },      // opcode: 0x2A, ROL A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x2B
    { .name="BIT", .length=3, .cycle=4, .exec_func=BIT, .mode=ABSOLUTE },      // opcode: 0x2C, BIT Abs
    { .name="AND", .length=3, .cycle=4, .exec_func=AND, .mode=ABSOLUTE },      // opcode: 0x2D, AND Abs
    { .name="ROL", .length=3, .cycle=6, .exec_func=ROL, .mode=ABSOLUTE },      // opcode: 0x2E, ROL, Abs
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x2F
    { .name="BMI", .length=2, .cycle=2, .exec_func=BMI, .mode=RELATIVE },      // opcode: 0x30
    { .name="AND", .length=2, .cycle=5, .exec_func=AND, .mode=INDIRECT_Y },    // opcode: 0x31, AND IdnY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x32
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x33
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x34
    { .name="AND", .length=2, .cycle=4, .exec_func=AND, .mode=ZERO_PAGE_X },   // opcode: 0x35, AND ZpgX
    { .name="ROL", .length=2, .cycle=6, .exec_func=ROL, .mode=ZERO_PAGE_X },   // opcode: 0x36, ROL ZpgX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x37
    { .name="SEC", .length=1, .cycle=2, .exec_func=SEC, .mode=IMPLIED },       // opcode: 0x38
    { .name="AND", .length=3, .cycle=4, .exec_func=AND, .mode=ABSOLUTE_Y },    // opcode: 0x39, AND AbsY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x3A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x3B
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x3C
    { .name="AND", .length=3, .cycle=4, .exec_func=AND, .mode=ABSOLUTE_X },    // opcode: 0x3D, AND AbsX
    { .name="ROL", .length=3, .cycle=7, .exec_func=ROL, .mode=ABSOLUTE_X },    // opcode: 0x3E, ROL AbsX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x3F
    { .name="RTI", .length=1, .cycle=6, .exec_func=RTI, .mode=IMPLIED },       // opcode: 0x40
    { .name="EOR", .length=2, .cycle=6, .exec_func=EOR, .mode=INDIRECT_X },    // opcode: 0x41, EOR IndX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x42
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x43
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x44
    { .name="EOR", .length=2, .cycle=3, .exec_func=EOR, .mode=ZERO_PAGE },     // opcode: 0x45, EOR Zpg
    { .name="LSR", .length=2, .cycle=5, .exec_func=LSR, .mode=ZERO_PAGE },     // opcode: 0x46, LSR Zpg
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x47
    { .name="PHA", .length=1, .cycle=3, .exec_func=PHA, .mode=IMPLIED },       // opcode: 0x48
    { .name="EOR", .length=2, .cycle=2, .exec_func=EOR, .mode=IMMEDIATE },     // opcode: 0x49, EOR imm
    { .name="LSR", .length=1, .cycle=2, .exec_func=LSR, .mode=REGISTER },      // opcode: 0x4A, LSR A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x4B
    { .name="JMP", .length=3, .cycle=3, .exec_func=JMP, .mode=ABSOLUTE },      // opcode: 0x4C, JMP Abs
    { .name="EOR", .length=3, .cycle=4, .exec_func=EOR, .mode=ABSOLUTE },      // opcode: 0x4D, EOR Abs
    { .name="LSR", .length=3, .cycle=6, .exec_func=LSR, .mode=ABSOLUTE },      // opcode: 0x4E, LSR Abs
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x4F
    { .name="EOR", .length=2, .cycle=2, .exec_func=BVC, .mode=RELATIVE },      // opcode: 0x50
    { .name="EOR", .length=2, .cycle=5, .exec_func=EOR, .mode=INDIRECT_Y },    // opcode: 0x51, EOR IndY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x52
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x53
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x54
    { .name="EOR", .length=2, .cycle=4, .exec_func=EOR, .mode=ZERO_PAGE_X },   // opcode: 0x55, EOR ZpgX
    { .name="LSR", .length=2, .cycle=6, .exec_func=LSR, .mode=ZERO_PAGE_X },   // opcode: 0x56, LSR Zpg, X
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x57
    { .name="CLI", .length=1, .cycle=2, .exec_func=CLI, .mode=IMPLIED },       // opcode: 0x58
    { .name="EOR", .length=3, .cycle=4, .exec_func=EOR, .mode=ABSOLUTE_Y },    // opcode: 0x59, EOR AbsY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x5A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x5B
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x5C
    { .name="EOR", .length=3, .cycle=4, .exec_func=EOR, .mode=ABSOLUTE_X },    // opcode: 0x5D, EOR AbsX
    { .name="LSR", .length=3, .cycle=7, .exec_func=LSR, .mode=ABSOLUTE_X },    // opcode: 0x5E, LSR Abs,X
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x5F
    { .name="RTS", .length=1, .cycle=6, .exec_func=RTS, .mode=IMPLIED },       // opcode: 0x60
    { .name="ADC", .length=2, .cycle=6, .exec_func=ADC, .mode=INDIRECT_X },    // opcode: 0x61, ADC IndX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x62
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x63
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x64
    { .name="ADC", .length=2, .cycle=3, .exec_func=ADC, .mode=ZERO_PAGE },     // opcode: 0x65, ADC Zpg
    { .name="ROR", .length=2, .cycle=5, .exec_func=ROR, .mode=ZERO_PAGE },     // opcode: 0x66, ROR Zpg
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x67
    { .name="PLA", .length=1, .cycle=4, .exec_func=PLA, .mode=IMPLIED },       // opcode: 0x68
    { .name="ADC", .length=2, .cycle=2, .exec_func=ADC, .mode=IMMEDIATE },     // opcode: 0x69, ADC imm
    { .name="ROR", .length=1, .cycle=2, .exec_func=ROR, .mode=REGISTER },      // opcode: 0x6A, ROR A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x6B
    { .name="JMP", .length=3, .cycle=5, .exec_func=JMP, .mode=INDIRECT },      // opcode: 0x6C, JMP Ind
    { .name="ADC", .length=3, .cycle=4, .exec_func=ADC, .mode=ABSOLUTE },      // opcode: 0x6D, ADC Abs
    { .name="ROR", .length=3, .cycle=6, .exec_func=ROR, .mode=ABSOLUTE },      // opcode: 0x6E, ROR Abs
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x6F
    { .name="BVS", .length=2, .cycle=2, .exec_func=BVS, .mode=RELATIVE },      // opcode: 0x70
    { .name="ADC", .length=2, .cycle=5, .exec_func=ADC, .mode=INDIRECT_Y },    // opcode: 0x71, ADC IndY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x72
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x73
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x74
    { .name="ADC", .length=2, .cycle=4, .exec_func=ADC, .mode=ZERO_PAGE_X },   // opcode: 0x75, ADC ZpgX
    { .name="ROR", .length=2, .cycle=6, .exec_func=ROR, .mode=ZERO_PAGE_X },   // opcode: 0x76, ROR ZpgX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x77
    { .name="SEI", .length=1, .cycle=2, .exec_func=SEI },                      // opcode: 0x78
    { .name="ADC", .length=3, .cycle=4, .exec_func=ADC, .mode=ABSOLUTE_Y },    // opcode: 0x79, ADC AbsY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x7A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x7B
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x7C
    { .name="ADC", .length=3, .cycle=4, .exec_func=ADC, .mode=ABSOLUTE_X },    // opcode: 0x7D, ADC AbsX
    { .name="ROR", .length=3, .cycle=7, .exec_func=ROR, .mode=ABSOLUTE_X },    // opcode: 0x7E, ROR AbsX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x7F
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x80
    { .name="STA", .length=2, .cycle=6, .exec_func=STA, .mode=INDIRECT_X },    // opcode: 0x81, STA IndX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x82
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x83
    { .name="STY", .length=2, .cycle=3, .exec_func=STY, .mode=ZERO_PAGE },     // opcode: 0x84, STY Zpg
    { .name="STA", .length=2, .cycle=3, .exec_func=STA, .mode=ZERO_PAGE },     // opcode: 0x85, STA Zpg
    { .name="STX", .length=2, .cycle=3, .exec_func=STX, .mode=ZERO_PAGE },     // opcode: 0x86, STX Zpg
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x87
    { .name="DEY", .length=1, .cycle=2, .exec_func=DEY, .mode=IMPLIED },       // opcode: 0x88
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x89
    { .name="TXA", .length=1, .cycle=2, .exec_func=TXA, .mode=IMPLIED },       // opcode: 0x8A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x8B
    { .name="STY", .length=3, .cycle=4, .exec_func=STY, .mode=ABSOLUTE },      // opcode: 0x8C, STY Abs
    { .name="STA", .length=3, .cycle=4, .exec_func=STA, .mode=ABSOLUTE },      // opcode: 0x8D, STA Abs
    { .name="STX", .length=3, .cycle=4, .exec_func=STX, .mode=ABSOLUTE },      // opcode: 0x8E, STX Abs
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x8F
    { .name="BCC", .length=2, .cycle=2, .exec_func=BCC, .mode=RELATIVE },      // opcode: 0x90
    { .name="STA", .length=2, .cycle=6, .exec_func=STA, .mode=INDIRECT_Y },    // opcode: 0x91, STA IndY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x92
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x93
    { .name="STY", .length=2, .cycle=4, .exec_func=STY, .mode=ZERO_PAGE_X },   // opcode: 0x94, STY ZpgX
    { .name="STA", .length=2, .cycle=4, .exec_func=STA, .mode=ZERO_PAGE_X },   // opcode: 0x95, STA ZpgX
    { .name="STX", .length=2, .cycle=4, .exec_func=STX, .mode=ZERO_PAGE_Y },   // opcode: 0x96, STX ZpgY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x97
    { .name="TYA", .length=1, .cycle=2, .exec_func=TYA, .mode=IMPLIED },       // opcode: 0x98
    { .name="STA", .length=3, .cycle=5, .exec_func=STA, .mode=ABSOLUTE_Y },    // opcode: 0x99, STA AbsY
    { .name="TXS", .length=1, .cycle=2, .exec_func=TXS, .mode=IMPLIED },       // opcode: 0x9A
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x9B
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x9C
    { .name="STA", .length=3, .cycle=5, .exec_func=STA, .mode=ABSOLUTE_X },    // opcode: 0x9D, STA AbsX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x9E
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0x9F
    { .name="LDY", .length=2, .cycle=2, .exec_func=LDY, .mode=IMMEDIATE },     // opcode: 0xA0
    { .name="LDA", .length=2, .cycle=2, .exec_func=LDA, .mode=INDIRECT_X },    // opcode: 0xA1
    { .name="LDX", .length=2, .cycle=2, .exec_func=LDX, .mode=IMMEDIATE },     // opcode: 0xA2
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xA3
    { .name="LDY", .length=2, .cycle=3, .exec_func=LDY, .mode=ZERO_PAGE },     // opcode: 0xA4
    { .name="LDA", .length=2, .cycle=3, .exec_func=LDA, .mode=ZERO_PAGE },     // opcode: 0xA5
    { .name="LDX", .length=2, .cycle=3, .exec_func=LDX, .mode=ZERO_PAGE },     // opcode: 0xA6
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xA7
    { .name="TAY", .length=1, .cycle=2, .exec_func=TAY, .mode=IMPLIED },       // opcode: 0xA8
    { .name="LDA", .length=2, .cycle=2, .exec_func=LDA, .mode=IMMEDIATE },     // opcode: 0xA9
    { .name="TAX", .length=1, .cycle=2, .exec_func=TAX, .mode=IMPLIED },       // opcode: 0xAA
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xAB
    { .name="LDY", .length=3, .cycle=4, .exec_func=LDY, .mode=ABSOLUTE },      // opcode: 0xAC
    { .name="LDA", .length=3, .cycle=4, .exec_func=LDA, .mode=ABSOLUTE },      // opcode: 0xAD
    { .name="LDX", .length=3, .cycle=4, .exec_func=LDX, .mode=ABSOLUTE },      // opcode: 0xAE
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xAF
    { .name="BCS", .length=2, .cycle=2, .exec_func=BCS, .mode=RELATIVE },      // opcode: 0xB0
    { .name="LDA", .length=2, .cycle=5, .exec_func=LDA, .mode=INDIRECT_Y },    // opcode: 0xB1
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xB2
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xB3
    { .name="LDY", .length=2, .cycle=4, .exec_func=LDY, .mode=ZERO_PAGE_X },   // opcode: 0xB4
    { .name="LDA", .length=2, .cycle=4, .exec_func=LDA, .mode=ZERO_PAGE_X },   // opcode: 0xB5
    { .name="LDX", .length=2, .cycle=4, .exec_func=LDX, .mode=ZERO_PAGE_Y },   // opcode: 0xB6
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xB7
    { .name="CLV", .length=1, .cycle=2, .exec_func=CLV, .mode=IMPLIED },       // opcode: 0xB8
    { .name="LDA", .length=3, .cycle=4, .exec_func=LDA, .mode=ABSOLUTE_Y },    // opcode: 0xB9
    { .name="TSX", .length=1, .cycle=2, .exec_func=TSX, .mode=IMPLIED },       // opcode: 0xBA
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xBB
    { .name="LDY", .length=3, .cycle=4, .exec_func=LDY, .mode=ABSOLUTE_X },    // opcode: 0xBC
    { .name="LDA", .length=3, .cycle=4, .exec_func=LDA, .mode=ABSOLUTE_X },    // opcode: 0xBD
    { .name="LDX", .length=3, .cycle=4, .exec_func=LDX, .mode=ABSOLUTE_Y },    // opcode: 0xBE
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xBF
    { .name="CPY", .length=2, .cycle=2, .exec_func=CPY, .mode=IMMEDIATE },     // opcode: 0xC0, CPY imm
    { .name="CMP", .length=2, .cycle=6, .exec_func=CMP, .mode=INDIRECT_X },    // opcode: 0xC1, CMP IndX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xC2
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xC3
    { .name="CPY", .length=2, .cycle=3, .exec_func=CPY, .mode=ZERO_PAGE },     // opcode: 0xC4, CPY Zpg
    { .name="CMP", .length=2, .cycle=3, .exec_func=CMP, .mode=ZERO_PAGE },     // opcode: 0xC5, CMP,Zpg
    { .name="DEC", .length=2, .cycle=5, .exec_func=DEC, .mode=ZERO_PAGE },     // opcode: 0xC6, DEC Zpg
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xC7
    { .name="INY", .length=1, .cycle=2, .exec_func=INY, .mode=IMPLIED },       // opcode: 0xC8
    { .name="CMP", .length=2, .cycle=2, .exec_func=CMP, .mode=IMMEDIATE },     // opcode: 0xC9, CMP imm
    { .name="DEX", .length=1, .cycle=2, .exec_func=DEX, .mode=IMPLIED },       // opcode: 0xCA
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xCB
    { .name="CPY", .length=3, .cycle=4, .exec_func=CPY, .mode=ABSOLUTE },      // opcode: 0xCC
    { .name="CMP", .length=3, .cycle=4, .exec_func=CMP, .mode=ABSOLUTE },      // opcode: 0xCD, CMP Abs
    { .name="DEC", .length=3, .cycle=6, .exec_func=DEC, .mode=ABSOLUTE },      // opcode: 0xCE, Abs
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xCF
    { .name="BNE", .length=2, .cycle=2, .exec_func=BNE, .mode=RELATIVE },      // opcode: 0xD0
    { .name="CMP", .length=2, .cycle=5, .exec_func=CMP, .mode=INDIRECT_Y },    // opcode: 0xD1, CMP IndY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xD2
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xD3
    { .name="NDF", .length=0, .cycle=0 ,.exec_func=NDF },                      // opcode: 0xD4
    { .name="CMP", .length=2, .cycle=4, .exec_func=CMP, .mode=ZERO_PAGE_X },   // opcode: 0xD5, CMP ZpgX
    { .name="DEC", .length=2, .cycle=6, .exec_func=DEC, .mode=ZERO_PAGE_X },   // opcode: 0xD6, DEC ZpgX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xD7
    { .name="CLD", .length=1, .cycle=2, .exec_func=CLD, .mode=IMPLIED },       // opcode: 0xD8
    { .name="CMP", .length=3, .cycle=4, .exec_func=CMP, .mode=ABSOLUTE_Y },    // opcode: 0xD9, CMP AbsY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xDA
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xDB
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xDC
    { .name="CMP", .length=3, .cycle=4, .exec_func=CMP, .mode=ABSOLUTE_X },    // opcode: 0xDD, CMP AbsX
    { .name="DEC", .length=3, .cycle=7, .exec_func=DEC, .mode=ABSOLUTE_X },    // opcode: 0xDE, DEC AbsX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xDF
    { .name="CPX", .length=2, .cycle=2, .exec_func=CPX, .mode=IMMEDIATE },     // opcode: 0xE0, CPX imm
    { .name="SBC", .length=2, .cycle=6, .exec_func=SBC, .mode=INDIRECT_X },    // opcode: 0xE1, SBC IndX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xE2
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xE3
    { .name="CPX", .length=2, .cycle=3, .exec_func=CPX, .mode=ZERO_PAGE },     // opcode: 0xE4, CPX Zpg
    { .name="SBC", .length=2, .cycle=3, .exec_func=SBC, .mode=ZERO_PAGE },     // opcode: 0xE5, SBC Zpg
    { .name="INC", .length=2, .cycle=5, .exec_func=INC, .mode=ZERO_PAGE },     // opcode: 0xE6, INC Zpg
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xE7
    { .name="INX", .length=1, .cycle=2, .exec_func=INX, .mode=IMPLIED },       // opcode: 0xE8
    { .name="SBC", .length=2, .cycle=2, .exec_func=SBC, .mode=IMMEDIATE },     // opcode: 0xE9, SBC imm
    { .name="NOP", .length=1, .cycle=2, .exec_func=NOP, .mode=IMPLIED },       // opcode: 0xEA
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xEB
    { .name="CPX", .length=3, .cycle=4, .exec_func=CPX, .mode=ABSOLUTE },      // opcode: 0xEC, CPX Abs
    { .name="SBC", .length=3, .cycle=4, .exec_func=SBC, .mode=ABSOLUTE },      // opcode: 0xED, SBC Abs
    { .name="INC", .length=3, .cycle=6, .exec_func=INC, .mode=ABSOLUTE },      // opcode: 0xEE, INC Abs
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xEF
    { .name="BEQ", .length=2, .cycle=2, .exec_func=BEQ, .mode=RELATIVE },      // opcode: 0xF0
    { .name="SBC", .length=2, .cycle=5, .exec_func=SBC, .mode=INDIRECT_Y },    // opcode: 0xF1, SBC IndY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xF2
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xF3
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xF4
    { .name="SBC", .length=2, .cycle=4, .exec_func=SBC, .mode=ZERO_PAGE_X },   // opcode: 0xF5, SBC ZpgX
    { .name="INC", .length=2, .cycle=6, .exec_func=INC, .mode=ZERO_PAGE_X },   // opcode: 0xF6, INC ZpgX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xF7
    { .name="SED", .length=1, .cycle=2, .exec_func=SED, .mode=IMPLIED },       // opcode: 0xF8
    { .name="SBC", .length=3, .cycle=4, .exec_func=SBC, .mode=ABSOLUTE_Y },    // opcode: 0xF9, SBC AbsY
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xFA
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xFB
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xFC
    { .name="SBC", .length=3, .cycle=4, .exec_func=SBC, .mode=ABSOLUTE_X },    // opcode: 0xFD, SBC AbsX
    { .name="INC", .length=3, .cycle=7, .exec_func=INC, .mode=ABSOLUTE_X },    // opcode: 0xFE, INC AbsX
    { .name="NDF", .length=0, .cycle=0, .exec_func=NDF },                      // opcode: 0xFF
};

instruction_t decode_instruction(uint8_t opcode) {
    return instructions[opcode];
}

void print_asm(exector_arg_t arg) {
    switch (arg.instr.mode)
    {
    case REGISTER:
        printf("%s\n", arg.instr.name);
        break;
    case IMMEDIATE:
        printf("%s #$%02X\n", arg.instr.name, arg.other[0]);
        break;
    case IMPLIED:
        printf("%s\n", arg.instr.name);
        break;
    case RELATIVE:
        printf("%s %02X\n", arg.instr.name, arg.other[0]);
        break;
    case ZERO_PAGE:
        printf("%s $%02X\n", arg.instr.name, arg.other[0]);
        break;
    case ZERO_PAGE_X:
        printf("%s $%02X, X\n", arg.instr.name, arg.other[0]);
        break;
    case ZERO_PAGE_Y:
        printf("%s $%02X, Y\n", arg.instr.name, arg.other[0]);
        break;
    case ABSOLUTE:
        printf("%s $%02X%02X\n", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case ABSOLUTE_X:
        printf("%s $%02X%02X, X\n", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case ABSOLUTE_Y:
        printf("%s $%02X%02X, Y\n", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case INDIRECT:
        printf("%s ($%02X%02X)\n", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case INDIRECT_X:
        printf("%s ($%02X, X)\n", arg.instr.name, arg.other[0]);
        break;
    case INDIRECT_Y:
        printf("%s ($%02X), Y\n", arg.instr.name, arg.other[0]);
        break;
    default:
        printf("%s\n", arg.instr.name);
    }
}