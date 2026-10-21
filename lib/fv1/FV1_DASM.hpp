/*
 * Copyright (C)2021 - E.Heidt
 * Licensed for personal, non-commercial use only
 */

#pragma once

#include <stdio.h>
#include <set>
#define SPN_PARSER
#define private public
#include "FV1.hpp"

constexpr uint32_t OP_RDA = 0x00;
constexpr uint32_t OP_RMPA = 0x01;
constexpr uint32_t OP_WRA = 0x02;
constexpr uint32_t OP_WRAP = 0x03;
constexpr uint32_t OP_RDAX = 0x04;
constexpr uint32_t OP_RDFX = 0x05;
constexpr uint32_t OP_WRAX = 0x06;
constexpr uint32_t OP_WRHX = 0x07;
constexpr uint32_t OP_WRLX = 0x08;
constexpr uint32_t OP_MAXX = 0x09;
constexpr uint32_t OP_MULX = 0x0a;
constexpr uint32_t OP_LOG = 0x0b;
constexpr uint32_t OP_EXP = 0x0c;
constexpr uint32_t OP_SOF = 0x0d;
constexpr uint32_t OP_AND = 0x0e;
constexpr uint32_t OP_OR = 0x0f;
constexpr uint32_t OP_XOR = 0x10;
constexpr uint32_t OP_SKP = 0x11;
constexpr uint32_t OP_WLDS = 0x12;
constexpr uint32_t OP_JAM = 0x13;
constexpr uint32_t OP_CHO_RDA = 0x00000014;

constexpr uint32_t OP_WLDR = 0x40000012;
constexpr uint32_t OP_CHO_SOF = 0x80000014;
constexpr uint32_t OP_CHO_RDAL = 0xc0000014;
// const int OP_CLR = 0x0e;
// const int OP_NOT = 0xffffff10;
// const int OP_ABSA = 0x09;
// const int OP_LDAX = 0x05;

namespace FV1dasm
{
    static void dasmOP(uint32_t op, int32_t *instruction)
    {
        struct _
        {
            static int af(unsigned int op, unsigned int mask)
            {
                int32_t r = op & mask;

                while ((mask & 0b10000000000000000000000000000000) == 0)
                {
                    mask <<= 1;
                    r <<= 1;
                }

                r /= (1 << 8);
                // r = FixedPoint::fixSign(r);
                return r;
            }

            static int af2(unsigned int op, unsigned int mask)
            {
                return af(op, mask) * 2;
            }

            static int ai(unsigned int op, unsigned int mask)
            {
                uint32_t r = op & mask;

                while ((mask & 0x1) == 0)
                {
                    mask >>= 1;
                    r >>= 1;
                }

                return r;
            }
        };
        uint32_t cmd = op & 0b00000000000000000000000000011111;
        switch (cmd)
        {
        case OP_SKP:
            instruction[0] = cmd;
            instruction[1] = _::ai(op, 0b11111000000000000000000000000000);
            instruction[2] = _::ai(op, 0b00000111111000000000000000000000);
            break;
        case OP_SOF:
        case OP_LOG:
        case OP_EXP:
            instruction[0] = cmd;
            instruction[1] = _::af2(op, 0b11111111111111110000000000000000);
            instruction[2] = _::af(op, 0b00000000000000001111111111100000);
            break;
        case OP_AND:
        case OP_OR:
        case OP_XOR:
            instruction[0] = cmd;
            instruction[1] = _::ai(op, 0b11111111111111111111111100000000);
            break;
        case OP_RDAX:
        case OP_WRAX:
        case OP_MAXX:
        case OP_RDFX:
        case OP_WRLX:
        case OP_WRHX:
        case OP_MULX:
            instruction[0] = cmd;
            instruction[1] = _::ai(op, 0b00000000000000000000011111100000);
            instruction[2] = _::af2(op, 0b11111111111111110000000000000000);
            break;
        case OP_RDA:
        case OP_WRA:
        case OP_WRAP:
            instruction[0] = cmd;
            instruction[1] = _::ai(op, 0b00000000000111111111111111100000);
            instruction[2] = _::af2(op, 0b11111111111000000000000000000000);
            break;
        case OP_RMPA:
            instruction[0] = cmd;
            instruction[1] = _::af2(op, 0b11111111111000000000000000000000);
            break;
        case OP_WLDS:
            // case OP_WLDR:
            instruction[0] = cmd;
            if ((op & OP_WLDR) == OP_WLDR)
            {
                instruction[0] |= OP_WLDR;
                instruction[1] = _::ai(op, 0b00100000000000000000000000000000);
                instruction[2] = _::ai(op, 0b00011111111111111110000000000000);
                instruction[3] = _::ai(op, 0b00000000000000000000000001100000);
            }
            else
            {
                instruction[1] = _::ai(op, 0b00100000000000000000000000000000);
                instruction[2] = _::ai(op, 0b00011111111100000000000000000000);
                instruction[3] = _::ai(op, 0b00000000000011111111111111100000);
            }
            break;
        case OP_JAM:
            instruction[0] = cmd;
            instruction[1] = _::ai(op, 0b00000000000000000000000001000000);
            break;
        case OP_CHO_RDA:
        case OP_CHO_SOF:
        case OP_CHO_RDAL:
            instruction[0] = ((int)(op & 0xC0000000) | cmd);
            instruction[1] = _::ai(op, 0b00000000011000000000000000000000);
            instruction[2] = _::ai(op, 0b00111111000000000000000000000000);
            instruction[3] = _::ai(op, 0b00000000000111111111111111100000);
            break;
        default:
            break;
        }
    }

#ifndef MAKE_S

    char *replace_char(char *str, char in, char out)
    {
        char *p = str;

        while (*p != '\0')
        {
            if (*p == in)
                *p = out;
            ++p;
        }

        return str;
    }

    static uint32_t asmOP(uint32_t op, int a, int b = 0, int c = 0)
    {
        uint32_t args[4];
        args[0] = op;
        args[1] = (uint32_t)a;
        args[2] = (uint32_t)b;
        args[3] = (uint32_t)c;

        struct _
        {
            static int af2(uint32_t arg, uint32_t mask)
            {
                uint32_t m = 0b10000000000000000000000000000000;
                uint32_t r = arg << 7;

                while ((m & mask) == 0)
                {
                    m >>= 1;
                    r >>= 1;
                }

                return r & mask;
            }

            static int af(uint32_t arg, uint32_t mask)
            {
                return af2(arg << 1, mask);
            }

            static int ai(uint32_t arg, uint32_t mask)
            {
                uint32_t m = 0x1;
                uint32_t r = arg;

                while ((m & mask) == 0)
                {
                    m <<= 1;
                    r <<= 1;
                }

                return r & mask;
            }
        };

        uint32_t cmd = args[0] & 0b00000000000000000000000000011111;
        switch (cmd)
        {
        case OP_SKP:
            cmd |= _::ai(args[1], 0b11111000000000000000000000000000);
            cmd |= _::ai(args[2], 0b00000111111000000000000000000000);
            break;
        case OP_SOF:
        case OP_LOG:
        case OP_EXP:
            cmd |= _::af2(args[1], 0b11111111111111110000000000000000);
            cmd |= _::af(args[2], 0b00000000000000001111111111100000);
            break;
        case OP_AND:
        case OP_OR:
        case OP_XOR:
            cmd |= _::ai(args[1], 0b11111111111111111111111100000000);
            break;
        case OP_RDAX:
        case OP_WRAX:
        case OP_MAXX:
        case OP_RDFX:
        case OP_WRLX:
        case OP_WRHX:
        case OP_MULX:
            cmd |= _::ai(args[1], 0b00000000000000000000011111100000);
            cmd |= _::af2(args[2], 0b11111111111111110000000000000000);
            break;
        case OP_RDA:
        case OP_WRA:
        case OP_WRAP:
            cmd |= _::ai(args[1], 0b00000000000111111111111111100000);
            cmd |= _::af2(args[2], 0b11111111111000000000000000000000);
            break;
        case OP_RMPA:
            cmd |= _::af2(args[1], 0b11111111111000000000000000000000);
            break;
        case OP_WLDS:
            // case OP_WLDR:
            if ((args[0] & OP_WLDR) == OP_WLDR)
            {
                cmd |= OP_WLDR;
                cmd |= _::ai(args[1], 0b00100000000000000000000000000000);
                cmd |= _::ai(args[2], 0b00011111111111111110000000000000);
                cmd |= _::ai(args[3], 0b00000000000000000000000001100000);
            }
            else
            {
                cmd |= _::ai(args[1], 0b00100000000000000000000000000000);
                cmd |= _::ai(args[2], 0b00011111111100000000000000000000);
                cmd |= _::ai(args[3], 0b00000000000011111111111111100000);
            }
            break;
        case OP_JAM:
            cmd |= _::ai(args[1], 0b00000000000000000000000001000000);
            cmd |= 0b00000000000000000000000010000000;
            break;
        case OP_CHO_RDA:
        case OP_CHO_SOF:
        case OP_CHO_RDAL:
            cmd |= ((int)(args[0] & 0xC0000000) | cmd);
            cmd |= _::ai(args[1], 0b00000000011000000000000000000000);
            cmd |= _::ai(args[2], 0b00111111000000000000000000000000);
            cmd |= _::ai(args[3], 0b00000000000111111111111111100000);
            break;
        default:
            break;
        }

        return __builtin_bswap32(cmd);
    }

    void printASM(const int32_t *a, char *p)
    {
        switch ((uint32_t)a[0])
        {
        case OP_SKP:
            sprintf(p, "SKP %d, %d", a[1], a[2]);
            break;
        case OP_WLDS:
            sprintf(p, "WLDS %d, %d, %d", a[1], a[2], a[3]);
            break;
        case OP_RDAX:
            sprintf(p, "RDAX %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_SOF:
            sprintf(p, "SOF %f, %f", FixedPoint(a[1]).toFloat(), FixedPoint(a[2]).toFloat());
            break;
        case OP_LOG:
            sprintf(p, "LOG %f, %f", FixedPoint(a[1]).toFloat(), FixedPoint(a[2]).toFloat());
            break;
        case OP_EXP:
            sprintf(p, "EXP %f, %f", FixedPoint(a[1]).toFloat(), FixedPoint(a[2]).toFloat());
            break;
        case OP_AND:
            sprintf(p, "AND %x", a[1]);
            break;
        case OP_OR:
            sprintf(p, "OR %x", a[1]);
            break;
        case OP_XOR:
            sprintf(p, "XOR %x", a[1]);
            break;
        case OP_WRAX:
            sprintf(p, "WRAX %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_RDFX:
            sprintf(p, "RDFX %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_MULX:
            sprintf(p, "MULX %d", a[1]);
            break;
        case OP_RMPA:
            sprintf(p, "RMPA %f", FixedPoint(a[1]).toFloat());
            break;
        case OP_WRA:
            sprintf(p, "WRA %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_CHO_RDA:
            sprintf(p, "CHO RDA %d, %d, %d", a[1], a[2], a[3]);
            break;
        case OP_WRLX:
            sprintf(p, "WRLX %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_WRHX:
            sprintf(p, "WRHX %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_RDA:
            sprintf(p, "RDA %d, %f ;(%d)", a[1], FixedPoint(a[2]).toFloat(), a[2]);
            break;
        case OP_WRAP:
            sprintf(p, "WRAP %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_CHO_SOF:
            sprintf(p, "CHO SOF %d, %d, %d", a[1], a[2], a[3]);
            break;
        case OP_CHO_RDAL:
            sprintf(p, "CHO RDAL %d, %d, %d", a[1], a[2], a[3]);
            break;
        case OP_MAXX:
            sprintf(p, "MAXX %d, %f", a[1], FixedPoint(a[2]).toFloat());
            break;
        case OP_WLDR:
            sprintf(p, "WLDR %d, %d, %d", a[1], a[2], a[3]);
            break;
        case OP_JAM:
            sprintf(p, "JAM %d", a[1]);
            break;
        default:
            sprintf(p, ";??? %x", a[0]);
            break;
        }
    }

    int printCode(const int32_t *a, char *p, int line = 0)
    {
        switch ((uint32_t)a[0])
        {
        case OP_SKP:
            sprintf(p, "if(fv1->SKP(%d, %d)) goto R%03d;", a[1], a[2], line + a[2]);
            return line + a[2];
            break;
        case OP_WLDS:
            sprintf(p, "fv1->WLDS(%d, %d, %d);", a[1], a[2], a[3]);
            break;
        case OP_RDAX:
            sprintf(p, "fv1->RDAX(%d, %d);", a[1], a[2]);
            break;
        case OP_SOF:
            sprintf(p, "fv1->SOF(%d, %d);", a[1], a[2]);
            break;
        case OP_LOG:
            sprintf(p, "fv1->LOG(%d, %d);", a[1], a[2]);
            break;
        case OP_EXP:
            sprintf(p, "fv1->EXP(%d, %d);", a[1], a[2]);
            break;
        case OP_AND:
            sprintf(p, "fv1->AND(0x%x);", a[1]);
            break;
        case OP_OR:
            sprintf(p, "fv1->OR(0x%x);", a[1]);
            break;
        case OP_XOR:
            sprintf(p, "fv1->XOR(0x%x);", a[1]);
            break;
        case OP_WRAX:
            sprintf(p, "fv1->WRAX(%d, %d);", a[1], a[2]);
            break;
        case OP_RDFX:
            sprintf(p, "fv1->RDFX(%d, %d);", a[1], a[2]);
            break;
        case OP_MULX:
            sprintf(p, "fv1->MULX(%d);", a[1]);
            break;
        case OP_RMPA:
            sprintf(p, "fv1->RMPA(%d);", a[1]);
            break;
        case OP_WRA:
            sprintf(p, "fv1->WRA(%d, %d);", a[1], a[2]);
            break;
        case OP_CHO_RDA:
            sprintf(p, "fv1->CHO_RDA(%d, %d, %d);", a[1], a[2], a[3]);
            break;
        case OP_WRLX:
            sprintf(p, "fv1->WRLX(%d, %d);", a[1], a[2]);
            break;
        case OP_WRHX:
            sprintf(p, "fv1->WRHX(%d, %d);", a[1], a[2]);
            break;
        case OP_RDA:
            sprintf(p, "fv1->RDA(%d, %d);", a[1], a[2]);
            break;
        case OP_WRAP:
            sprintf(p, "fv1->WRAP(%d, %d);", a[1], a[2]);
            break;
        case OP_CHO_SOF:
            sprintf(p, "fv1->CHO_SOF(%d, %d, %d);", a[1], a[2], a[3]);
            break;
        case OP_CHO_RDAL:
            sprintf(p, "fv1->CHO_RDAL(%d);", a[1]);
            break;
        case OP_MAXX:
            sprintf(p, "fv1->MAXX(%d, %d);", a[1], a[2]);
            break;
        case OP_WLDR:
            sprintf(p, "fv1->WLDR(%d, %d, %d);", a[1], a[2], a[3]);
            break;
        case OP_JAM:
            sprintf(p, "fv1->JAM(%d);", a[1]);
            break;
        default:
            sprintf(p, ";??? %x", a[0]);
            break;
        }
        return 0;
    }

#endif
}; // end namespace

struct OP
{
    void *label;

    union
    {
        int32_t arg0;
        Reg *reg0;
    };

    union
    {
        int32_t arg1;
        Reg *reg1;
    };

    int32_t arg2;
};

static void **_dispatch_table;

constexpr uint32_t D_OP_WLDR = OP_CHO_RDA + 1;
constexpr uint32_t D_OP_CHO_SOF = OP_CHO_RDA + 2;
constexpr uint32_t D_OP_CHO_RDAL = OP_CHO_RDA + 3;
constexpr uint32_t D_OP_END = OP_CHO_RDA + 4;
constexpr uint32_t D_OP_NOP = OP_CHO_RDA + 5;
constexpr uint32_t D_OP_SOF_SET = OP_CHO_RDA + 6;
constexpr uint32_t D_OP_SOF_ADD = OP_CHO_RDA + 7;
constexpr uint32_t D_OP_SOF_MUL = OP_CHO_RDA + 8;
constexpr uint32_t D_OP_EXP_10 = OP_CHO_RDA + 9;
constexpr uint32_t D_OP_RDAX_1 = OP_CHO_RDA + 10;
constexpr uint32_t D_OP_WRAX_1 = OP_CHO_RDA + 11;
constexpr uint32_t D_OP_WRAX_RDAX = OP_CHO_RDA + 12;
constexpr uint32_t D_OP_LDAX = OP_CHO_RDA + 13;

void execute_program(FV1 *fv1)
{
    // https:// eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables

    static void *dispatch_table[] = {
        &&_OP_RDA,
        &&_OP_RMPA,
        &&_OP_WRA,
        &&_OP_WRAP,
        &&_OP_RDAX,
        &&_OP_RDFX,
        &&_OP_WRAX,
        &&_OP_WRHX,
        &&_OP_WRLX,
        &&_OP_MAXX,
        &&_OP_MULX,
        &&_OP_LOG,
        &&_OP_EXP,
        &&_OP_SOF,
        &&_OP_AND,
        &&_OP_OR,
        &&_OP_XOR,
        &&_OP_SKP,
        &&_OP_WLDS,
        &&_OP_JAM,
        &&_OP_CHO_RDA, // 0x14

        &&_OP_WLDR,     // 0x40000012;
        &&_OP_CHO_SOF,  // 0x80000014;
        &&_OP_CHO_RDAL, // 0xc0000014;
        &&_OP_END,
        &&_OP_NOP,
        &&_OP_SOF_SET,
        &&_OP_SOF_ADD,
        &&_OP_SOF_MUL,
        &&_OP_EXP_10,
        &&_OP_RDAX_1,
        &&_OP_WRAX_1,
        &&_OP_WRAX_RDAX,
        &&_OP_LDAX};

    if (fv1 == nullptr)
    {
        _dispatch_table = dispatch_table;
        return;
    }

    auto op = (OP *)fv1->rom;

#define DISPATCH() \
    goto * (++op)->label;

    goto * op->label;

_OP_RDA:
    fv1->RDA(op->arg0, op->arg1);
    DISPATCH();
_OP_RMPA:
    fv1->RMPA(op->arg0);
    DISPATCH();
_OP_WRA:
    fv1->WRA(op->arg0, op->arg1);
    DISPATCH();
_OP_WRAP:
    fv1->WRAP(op->arg0, op->arg1);
    DISPATCH();
_OP_RDAX:
    fv1->_RDAX(op->reg0, op->arg1);
    DISPATCH();
_OP_RDFX:
    fv1->_RDFX(op->reg0, op->arg1);
    DISPATCH();
_OP_WRAX:
    fv1->_WRAX(op->reg0, op->arg1);
    DISPATCH();
_OP_WRHX:
    fv1->_WRHX(op->reg0, op->arg1);
    DISPATCH();
_OP_WRLX:
    fv1->_WRLX(op->reg0, op->arg1);
    DISPATCH();
_OP_MAXX:
    fv1->_MAXX(op->reg0, op->arg1);
    DISPATCH();
_OP_MULX:
    fv1->_MULX(op->reg0);
    DISPATCH();
_OP_LOG:
    fv1->LOG(op->arg0, op->arg1);
    DISPATCH();
_OP_EXP:
    fv1->EXP(op->arg0, op->arg1);
    DISPATCH();
_OP_SOF:
    fv1->SOF(op->arg0, op->arg1);
    DISPATCH();
_OP_AND:
    fv1->AND(op->arg0);
    DISPATCH();
_OP_OR:
    fv1->OR(op->arg0);
    DISPATCH();
_OP_XOR:
    fv1->XOR(op->arg0);
    DISPATCH();
_OP_SKP:
    op += fv1->SKP(op->arg0, op->arg1);
    DISPATCH();
_OP_WLDS:
    fv1->WLDS(op->arg0, op->arg1, op->arg2);
    DISPATCH();
_OP_JAM:
    fv1->JAM(op->arg0);
    DISPATCH();
_OP_CHO_RDA:
    fv1->CHO_RDA(op->arg0, op->arg1, op->arg2);
    DISPATCH();
_OP_WLDR:
    fv1->WLDR(op->arg0, op->arg1, op->arg2);
    DISPATCH();
_OP_CHO_RDAL:
    fv1->CHO_RDAL(op->arg0);
    DISPATCH();
_OP_CHO_SOF:
    fv1->CHO_SOF(op->arg0, op->arg1, op->arg2);
    DISPATCH();
_OP_NOP:
    DISPATCH();
_OP_END:
    return;
_OP_SOF_SET:
    fv1->acc.set(op->arg1);
    DISPATCH();
_OP_SOF_ADD:
    fv1->acc.add(op->arg1);
    DISPATCH();
_OP_SOF_MUL:
    fv1->acc.mul(op->arg0);
    DISPATCH();
_OP_EXP_10:
    fv1->_EXP();
    DISPATCH();
_OP_RDAX_1:
    fv1->_RDAX(op->reg0);
    DISPATCH();
_OP_WRAX_1:
    fv1->_WRAX(op->reg0);
    DISPATCH();
_OP_WRAX_RDAX:
    op->reg0->set(fv1->ACC());
    fv1->acc.set(FixedPoint::mul(op->reg1->getValue(), op->arg2));
    DISPATCH();
_OP_LDAX:
    fv1->_LDAX(op->reg0);
    DISPATCH();
}

extern "C" void *fv1_load(FV1 *fv1, const uint8_t *prog, void *(*malloc)(size_t size))
{
    if (malloc == nullptr)
        malloc = ::malloc;

    uint32_t *p = (uint32_t *)prog;
    auto fx = (OP *)(fv1->rom ? fv1->rom : malloc(sizeof(OP) * 129));
    if (fx == nullptr)
        return nullptr;
    fv1->rom = fx;

    execute_program(nullptr);

    int j = 0;
    if (prog != nullptr)
    {
        for (int a = 0; a < 512; a += 4)
        {
            uint32_t i = 0;
            i |= prog[a] << 24;
            i |= prog[a + 1] << 16;
            i |= prog[a + 2] << 8;
            i |= prog[a + 3] << 0;

            if (i == 0x11)
                break;

            int32_t instr[4] = {};
            FV1dasm::dasmOP(i, instr);

            if (instr[0] == OP_WLDR)
                instr[0] = D_OP_WLDR;
            else if (instr[0] == OP_CHO_SOF)
                instr[0] = D_OP_CHO_SOF;
            else if (instr[0] == OP_CHO_RDAL)
                instr[0] = D_OP_CHO_RDAL;
            else if (instr[0] == OP_SOF)
            {
                if (instr[1] == 0)
                    instr[0] = D_OP_SOF_SET;
                else if (instr[1] == 8388608)
                    instr[0] = D_OP_SOF_ADD;
                else if (instr[2] == 0)
                    instr[0] = D_OP_SOF_MUL;
            }
            else if (instr[0] == OP_EXP)
            {
                if (instr[1] == 8388608 && instr[2] == 0)
                    instr[0] = D_OP_EXP_10;
            }
            else if (instr[0] == OP_RDAX && instr[2] == 8388608)
            {
                instr[0] = D_OP_RDAX_1;
            }
            else if (instr[0] == OP_WRAX && instr[2] == 8388608)
            {
                instr[0] = D_OP_WRAX_1;
            }
            else if (instr[0] == OP_RDAX)
            {
                if (fx[j - 1].label == _dispatch_table[OP_WRAX] && fx[j - 1].arg1 == 0)
                {
                    fx[j - 1].label = _dispatch_table[D_OP_WRAX_RDAX];
                    fx[j - 1].reg1 = &fv1->regs[instr[1]];
                    fx[j - 1].arg2 = instr[2];
                    continue;
                }
            }
            else if (instr[0] == OP_RDFX && instr[2] == 0)
            {
                instr[0] = D_OP_LDAX;
            }

            fx[j++] = {_dispatch_table[instr[0]], instr[1], instr[2], instr[3]};

            switch (instr[0])
            {
            case D_OP_LDAX:
            case OP_RDAX:
            case D_OP_RDAX_1:
            case OP_WRAX:
            case D_OP_WRAX_1:
            case OP_MULX:
            case OP_RDFX:
            case OP_WRHX:
            case OP_WRLX:
            case OP_MAXX:
                fx[j - 1].reg0 = &fv1->regs[instr[1]];
                break;
            }
        }
    }
    else
    {
        while (j < 128)
            fx[j++] = {_dispatch_table[D_OP_NOP], 0, 0, 0}; //_OP_NOP
    }

    fx[j] = {_dispatch_table[D_OP_END], 0, 0, 0}; //_OP_END

    fv1->setFx(execute_program);
    return fv1->rom;
}