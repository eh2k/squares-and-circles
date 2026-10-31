/*
 * Copyright (C)2021 - E.Heidt
 * Licensed for personal, non-commercial use only
 */

// http://www.spinsemi.com/knowledge_base/arch.html
// http://www.spinsemi.com/Products/appnotes/spn1001/AN-0001.pdf
// http://www.spinsemi.com/Products/datasheets/spn1001-dev/SPINAsmUserManual.pdf
// http://www.spinsemi.com/programs.php
// http://www.spinsemi.com/knowledge_base/cheat.html
// http://www.spinsemi.com/knowledge_base/inst_syntax.html
// http://www.spinsemi.com/forum/viewtopic.php?f=3&t=110&p=2227&hilit=CHO#p2227

#pragma once

#include <inttypes.h>
#include <cmath>
#include <cstring>
#include <array>
#include <vector>

constexpr int SIN0_RATE = 0x00;
constexpr int SIN0_RANGE = 0x01;
constexpr int SIN1_RATE = 0x02;
constexpr int SIN1_RANGE = 0x03;
constexpr int RMP0_RATE = 0x04;
constexpr int RMP0_RANGE = 0x05;
constexpr int RMP1_RATE = 0x06;
constexpr int RMP1_RANGE = 0x07;
constexpr int POT0 = 0x10;
constexpr int POT1 = 0x11;
constexpr int POT2 = 0x12;
constexpr int ADCL = 0x14;
constexpr int ADCR = 0x15;
constexpr int DACL = 0x16;
constexpr int DACR = 0x17;
constexpr int ADDR_PTR = 0x18;
constexpr int REG0 = 0x20;
constexpr int REG31 = 0x3f;
constexpr int MAX_DELAY_ADDR = 32767;

constexpr int SKP_NEG = 0x01;
constexpr int SKP_GEZ = 0x02;
constexpr int SKP_ZRO = 0x04;
constexpr int SKP_ZRC = 0x08;
constexpr int SKP_RUN = 0x10;

constexpr int CHO_LFO_SIN0 = 0;
constexpr int CHO_LFO_SIN1 = 1;
constexpr int CHO_LFO_RMP0 = 2;
constexpr int CHO_LFO_RMP1 = 3;
constexpr int CHO_LFO_4 = 4;
constexpr int CHO_LFO_5 = 5;
constexpr int CHO_LFO_6 = 6;
constexpr int CHO_LFO_7 = 7;
constexpr int CHO_LFO_COS0 = 8;
constexpr int CHO_LFO_COS1 = 9;
constexpr int CHO_SIN = 0x00;
constexpr int CHO_COS = 0x01;
constexpr int CHO_REG = 0x02;
constexpr int CHO_COMPC = 0x04;
constexpr int CHO_COMPA = 0x08;
constexpr int CHO_RPTR2 = 0x10;
constexpr int CHO_NA = 0x20;

#ifndef INLINE
#define INLINE inline
#endif

#if defined(__ARM_ARCH_7EM__)
inline int32_t SSAT24(int32_t value)
{
    int32_t result;
    __asm volatile("ssat %0, %1, %2"
                   : "=r"(result)
                   : "I"(24), "r"(value));
    return result;
}

inline int32_t MUL(int32_t x, int32_t y)
{
    static constexpr int z = 8;
    int32_t t, t2;
    asm("smull    %[t], %[t2], %[a], %[b]\n\t"
        "mov      %[t2], %[t2], asl %[c]\n\t"
        "orr      %[t], %[t2], %[t], lsr %[d]\n\t"
        : [t] "=&r"(t), [t2] "=&r"(t2)
        : [a] "r"(x), [b] "r"(y),
          [c] "Mr"((z) + 1), [d] "Mr"(31 - (z)));
    return t;
}

#else
template <typename T>
inline T clamp(T value, T min, T max)
{
    return value > max ? max : value < min ? min
                                           : value;
}

#define SSAT24(newValue) clamp(newValue, -(F), (F)-1)
#define MUL(a, b) (((int64_t)(a) * (b)) / FixedPoint::F)

#endif

class FixedPoint
{
protected:
    int32_t value; // : 24;
public:
    static constexpr int F = (1 << 23);
    static constexpr int MAX = (F - 1);

    FixedPoint() : value(0)
    {
    }

    FixedPoint(int value) : value(value)
    {
    }

    FixedPoint(const FixedPoint &reg)
    {
        value = reg.value; //(reg.getValue());
    }

    INLINE static FixedPoint fromFloat(float value)
    {
        return FixedPoint(value * F);
    }

    INLINE float toFloat() const
    {
        return (float)this->value / F;
    }

    INLINE void set(int newValue)
    {
        value = SSAT24(newValue);
    }

    INLINE void set(const FixedPoint &newValue)
    {
        value = newValue.value;
    }

    INLINE int getValue() const
    {
        return value;
    }

    INLINE static int fixSign(int newValue)
    {
        if (newValue & 0xFF800000)
            newValue |= 0xFF000000;
        else
            newValue &= 0x00FFFFFF;

        return newValue;
    }

    INLINE static int mul(const FixedPoint &a, const FixedPoint &b)
    {
        return MUL(a.getValue(), b.getValue());
    }

    INLINE static int mul_add(const FixedPoint &a, const FixedPoint &b, const FixedPoint &offset)
    {
        return MUL(a.getValue(), b.getValue()) + offset.getValue();
    }
};

#ifdef FV1_DUMP_STATE
#include <iomanip>
std::ostream &operator<<(std::ostream &os, const FixedPoint &m)
{
    return os << std::hex << "(" << std::setfill('0') << std::setw(8) << m.getValue() << "|" << m.toFloat() << ")";
}
#endif

class Reg : public FixedPoint
{
public:
    Reg() : FixedPoint() {}
    Reg(const FixedPoint &value) : FixedPoint(value) {}

    INLINE void clear()
    {
        value = 0;
    }

    INLINE void add(const FixedPoint &b)
    {
        set(getValue() + b.getValue());
    }

    INLINE void sub(const FixedPoint &b)
    {
        set(getValue() - b.getValue());
    }

    INLINE void mul(const FixedPoint &b)
    {
        set(FixedPoint::mul(*this, b));
    }

    INLINE void abs()
    {
        if (value < 0)
            value = 0 - value;
    }

    INLINE void not_()
    {
        set(FixedPoint::fixSign(~getValue()));
    }

    INLINE void and_(int mask)
    {
        set(FixedPoint::fixSign(getValue() & mask));
    }

    INLINE void or_(int mask)
    {
        set(FixedPoint::fixSign(getValue() | mask));
    }

    INLINE void xor_(int mask)
    {
        set(FixedPoint::fixSign(getValue() ^ mask));
    }
};

class SinLFO
{
    Reg sin_ = FixedPoint(0);
    Reg cos_ = FixedPoint(-FixedPoint::MAX);

    const Reg *regRange;
    const Reg *regRate;

public:
    INLINE void init(const Reg &rate, const Reg &range)
    {
        regRange = &range;
        regRate = &rate;
    }

    INLINE void increment()
    {
        FixedPoint coeff(regRate->getValue() >> 8);
        cos_.add(FixedPoint::mul(sin_, coeff));
        sin_.sub(FixedPoint::mul(cos_, coeff));
    }

    INLINE void jam()
    {
        sin_.set(FixedPoint(0));
        cos_.set(FixedPoint(-FixedPoint::MAX));
    }

    INLINE int cos()
    {
        return FixedPoint::mul(cos_, *regRange);
    }

    INLINE int sin()
    {
        return FixedPoint::mul(sin_, *regRange);
    }

    INLINE int getValue(bool cosVal = false)
    {
        return cosVal ? cos() : sin();
    }
};

class RampLFO
{
    static constexpr int AMP_4096 = 0x3fffff;
    //	const int AMP_2048 = 0x1fffff;
    //	const int AMP_1024 = 0x0fffff;
    //	const int AMP_512 =  0x07ffff;

    int pos = 0;

    const Reg *regRange;
    const Reg *regRate;

public:
    static constexpr int AMP_OFFSET = 21;

    INLINE void init(const Reg &rate, const Reg &range)
    {
        regRange = &range;
        regRate = &rate;
    }

    INLINE void increment()
    {
        int freq = regRate->getValue() >> 12;
        pos = (pos - freq) & this->getRange();
    }

    INLINE void jam()
    {
        pos = 0;
    }

    INLINE int getValue(bool ptr2value = false)
    {
        int range = this->getRange();

        if (ptr2value)
            return (pos + (range / 2)) & range;
        else
            return pos;
    }

    INLINE int getRange()
    {
        return AMP_4096 >> (regRange->getValue() >> (AMP_OFFSET));
    }

    INLINE int getXFade()
    {
        int range = this->getRange();

        int halfAmp = range >> 1;
        int xfade = pos > halfAmp ? range - pos : pos;

        return xfade << (regRange->getValue() >> AMP_OFFSET) << 0; // push Amplitude to MAX ?
    }
};

class DelayMemory
{
    int16_t mem[MAX_DELAY_ADDR + 1] = {};
    unsigned int p = 0;

public:
    INLINE int get(int offset, Reg &lr)
    {
        int val = mem[(offset + p) & MAX_DELAY_ADDR] << 8;
        lr.set(val);
        return val;
    }

    INLINE void set(int offset, int value)
    {
        mem[(offset + p) & MAX_DELAY_ADDR] = value >> 8;
    }

    INLINE void decrPtr()
    {
        --p;
    }
};

struct OP;

// EXP: i < 0 is log2/16 in Q23, 2^(i*16) in Q23. Integer part as a shift, fraction by a degree-5 polynomial of
// 2^x on [0,1) (rel. error 1e-7): max 1 LSB off exp2f(), no libm call
inline int exp2i(int i)
{
    int n = i >> 19; // -16..-1
    float x = (float)(i & ((1 << 19) - 1)) * (1.0f / (1 << 19));
    float p = 0.9999998970f + x * (0.6931545998f + x * (0.2401408531f + x * (0.0558631622f + x * (0.0089462657f + x * 0.0018951140f))));
    return (int)(p * FixedPoint::F) >> -n;
}

// LOG: i > 0 in Q23, log2(i)/16 in Q23. Exponent from the leading zeros, log2 of the mantissa 1+t by a degree-7
// polynomial on [0,1): max 0.83 LSB off the exact value
inline int log2i(int i)
{
    int p = 31 - __builtin_clz(i); // 0..23
    float t = (float)((i << (23 - p)) & (FixedPoint::F - 1)) * (1.0f / FixedPoint::F);
    float l = t * (1.4426522058f + t * (-0.7203875172f + t * (0.4725032758f + t * (-0.3231237344f + t * (0.1904292492f + t * (-0.0768531431f + t * 0.0147795801f))))));
    return ((p - 23) << 19) + (int)(l * (1 << 19) + 0.5f);
}

class FV1
{
private:
    Reg acc;
    Reg pacc;
    Reg lr;
    Reg regs[REG31 + 1] = {};
    DelayMemory delay;
    SinLFO sinLFO[2] = {};
    RampLFO rampLFO[2] = {};
    bool firstRun = true;

    void (*program)(FV1 *fv1) = nullptr;

public:
    void *rom;

    INLINE int ACC()
    {
        return acc.getValue();
    }

    INLINE void ACC2PACC()
    {
        pacc.set(acc);
    }

    INLINE const FixedPoint &REG(int reg)
    {
        return regs[reg];
    }

    INLINE void REG_SET(int reg, int value)
    {
        regs[reg].set(value);
    }

    INLINE void SOF(const FixedPoint &scale, const FixedPoint &offset)
    {
        ACC2PACC();
        this->acc.set(FixedPoint::mul_add(this->acc, scale, offset));
    }

    INLINE void AND(int mask)
    {
        ACC2PACC();
        this->acc.and_(mask);
    }

    INLINE void OR(int mask)
    {
        ACC2PACC();
        this->acc.or_(mask);
    }

    INLINE void XOR(int mask)
    {
        ACC2PACC();
        this->acc.xor_(mask);
    }

    INLINE void LOG(const FixedPoint &scale, const FixedPoint &offset)
    {
        ACC2PACC();
        int l;

        if (this->acc.getValue() > 0)
            l = log2i(this->acc.getValue());
        else if (this->acc.getValue() < 0)
            l = log2i(-this->acc.getValue());
        else
            l = -FixedPoint::MAX;

        this->acc.set(FixedPoint::mul_add(l, scale, offset));
        // this->acc.set(l);
        // this->acc.mul(scale);
        // this->acc.add(offset);
    }

    INLINE void EXP(const FixedPoint &scale, const FixedPoint &offset)
    {
        _EXP();
        this->acc.set(FixedPoint::mul_add(this->acc, scale, offset));
        // this->acc.mul(scale);
        // this->acc.add(offset);
    }

    INLINE void _EXP()
    {
        ACC2PACC();
        if (this->ACC() >= 0)
        {
            this->acc.set(FixedPoint(FixedPoint::MAX));
        }
        else
        {
            this->acc.set(exp2i(this->acc.getValue()));
        }
    }

    INLINE int SKP(int flags, int nskip)
    {
        bool skip = false;

        if (flags & SKP_RUN)
            skip |= firstRun == false;
        if (flags & SKP_ZRO)
            skip |= this->ACC() == 0;
        if (flags & SKP_GEZ)
            skip |= this->ACC() > 0;
        if (flags & SKP_NEG)
            skip |= this->ACC() < 0;
        if (flags & SKP_ZRC)
            skip |= (this->ACC() & FixedPoint::F) != (this->pacc.getValue() & FixedPoint::F);

        if (skip)
            return nskip;

        return 0;
    }

    INLINE void RDAX(int addr, const FixedPoint &scale)
    {
        _RDAX(&regs[addr], scale);
    }

    INLINE void _RDAX(Reg *addr, const FixedPoint &scale)
    {
        ACC2PACC();
        // Reg reg(*addr);
        // reg.mul(scale);
        // this->acc.add(reg);
        this->acc.set(FixedPoint::mul_add(*addr, scale, acc));
    }

    INLINE void _RDAX(Reg *addr)
    {
        ACC2PACC();
        this->acc.add(*addr);
    }

    INLINE void WRAX(int addr, const FixedPoint &scale)
    {
        _WRAX(&regs[addr], scale);
    }

    INLINE void _WRAX(Reg *addr, const FixedPoint &scale)
    {
        ACC2PACC();
        addr->set(this->acc);
        this->acc.mul(scale);
    }

    INLINE void _WRAX(Reg *addr)
    {
        ACC2PACC();
        addr->set(this->acc);
    }

    INLINE void MAXX(int addr, const FixedPoint &scale)
    {
        _MAXX(&regs[addr], scale);
    }

    INLINE void _MAXX(Reg *addr, const FixedPoint &scale)
    {
        ACC2PACC();
        Reg temp(*addr);
        temp.abs();
        temp.mul(scale);

        auto a = this->ACC();
        if (a < 0)
            a = -a;
        auto b = temp.getValue();
        if (b < 0)
            b = -b;
        this->acc.set(a > b ? a : b);
    }

    INLINE void MULX(int addr)
    {
        _MULX(&regs[addr]);
    }

    INLINE void _MULX(Reg *addr)
    {
        ACC2PACC();
        this->acc.mul(*addr);
    }

    INLINE void RDFX(int addr, const FixedPoint &scale)
    {
        _RDFX(&regs[addr], scale);
    }

    INLINE void _RDFX(Reg *addr, const FixedPoint &scale)
    {
        ACC2PACC();
        // this->acc.sub(*addr);
        // this->acc.mul(scale);
        // this->acc.add(*addr);
        this->acc.set(FixedPoint::mul_add(this->acc.getValue() - addr->getValue(), scale, *addr));
    }

    INLINE void WRLX(int addr, const FixedPoint &scale)
    {
        _WRLX(&regs[addr], scale);
    }

    INLINE void _WRLX(Reg *addr, const FixedPoint &scale) // ACC­>REG[ADDR], (PACC-­ACC) * C + PACC
    {
        int _pacc = this->pacc.getValue();
        ACC2PACC();
        addr->set(this->acc);
        // Reg tmp(this->pacc);
        // tmp.sub(this->acc);
        // tmp.mul(scale);
        // tmp.add(this->acc);
        // this->acc.set(tmp);
        this->acc.set(FixedPoint::mul_add(_pacc - this->acc.getValue(), scale, _pacc));
    }

    INLINE void WRHX(int addr, const FixedPoint &scale)
    {
        _WRHX(&regs[addr], scale);
    }

    INLINE void _WRHX(Reg *addr, const FixedPoint &scale) // ACC­>REG[ADDR], (ACC*C) + PACC
    {
        int _pacc = this->pacc.getValue();
        ACC2PACC();
        addr->set(this->acc);
        // this->acc.mul(scale);
        // this->acc.add(this->pacc);
        this->acc.set(FixedPoint::mul_add(acc, scale, _pacc));
    }

    INLINE void RDA(int addr, const FixedPoint &scale)
    {
        ACC2PACC();
        // Reg tmp(this->delay.get(addr, this->lr));
        // tmp.mul(scale);
        // this->acc.add(tmp);
        this->acc.set(FixedPoint::mul_add(this->delay.get(addr, this->lr), scale, this->acc));
    }

    INLINE void RMPA(const FixedPoint &scale)
    {
        ACC2PACC();
        Reg tmp(this->delay.get(this->REG(ADDR_PTR).getValue() >> 8, this->lr));
        tmp.mul(scale);
        this->acc.add(tmp);
    }

    INLINE void WRA(int addr, const FixedPoint &scale)
    {
        ACC2PACC();
        this->delay.set(addr, this->ACC());
        this->acc.mul(scale);
    }

    INLINE void WRAP(int addr, const FixedPoint &scale)
    {
        ACC2PACC();
        this->delay.set(addr, this->ACC());
        this->acc.mul(scale);
        this->acc.add(this->lr);
    }

    INLINE void WLDS(int lfo, int freq, int amp)
    {
        if (lfo == 0)
        {
            this->REG_SET(SIN0_RATE, freq << 14);
            this->REG_SET(SIN0_RANGE, amp << 8);
            this->sinLFO[0].jam();
        }
        else
        {
            this->REG_SET(SIN1_RATE, freq << 14);
            this->REG_SET(SIN1_RANGE, amp << 8);
            this->sinLFO[1].jam();
        }
    }

    INLINE void WLDR(int lfo, int freq, int amp)
    {
        amp <<= RampLFO::AMP_OFFSET;

        if (lfo == 0)
        {
            this->REG_SET(RMP0_RATE, freq << 8);
            this->REG_SET(RMP0_RANGE, amp);
            this->rampLFO[0].jam();
        }
        else
        {
            this->REG_SET(RMP1_RATE, freq << 8);
            this->REG_SET(RMP1_RANGE, amp);
            this->rampLFO[1].jam();
        }
    }

    INLINE void JAM(int lfo)
    {
        this->rampLFO[lfo].jam();
    }

    INLINE void CHO(int lfo, int flags, int *lfoval, int *scale)
    {
        static constexpr auto ONE = FixedPoint::MAX / 2;

        if (lfo == CHO_LFO_SIN0 || lfo == CHO_LFO_SIN1)
        {
            *lfoval = this->sinLFO[lfo].getValue(flags & CHO_COS);

            *scale = flags & CHO_COMPC ? ONE - *lfoval : *lfoval;

            if (flags & CHO_COMPA)
                *lfoval = -*lfoval;
        }
        else if (lfo == CHO_LFO_RMP0 || lfo == CHO_LFO_RMP1)
        {
            auto range = this->rampLFO[lfo - CHO_LFO_RMP0].getRange();
            *lfoval = this->rampLFO[lfo - CHO_LFO_RMP0].getValue(flags & CHO_RPTR2);

            if (flags & CHO_COMPA)
                *lfoval = range - *lfoval;

            if (flags & CHO_NA)
                *lfoval = this->rampLFO[lfo - CHO_LFO_RMP0].getXFade();

            *scale = flags & CHO_COMPC ? ONE - *lfoval : *lfoval;
        }

        // scale.set(scale.getValue() / 2);
    }

    INLINE void CHO_RDA(int lfo, int flags, int addr)
    {
        int lfoval = 0;
        int scale = 0;

        CHO(lfo, flags, &lfoval, &scale);

        if ((flags & CHO_NA) == 0)
            addr += (lfoval >> 10);

        RDA(addr, scale);
    }

    INLINE void CHO_SOF(int lfo, int flags, const FixedPoint &offset)
    {
        int lfoval = 0;
        int scale = 0;

        CHO(lfo, flags, &lfoval, &scale);
        SOF(scale, offset);
    }

    INLINE void CHO_RDAL(int lfo)
    {
        ACC2PACC();

        int lfoval = 0;

        switch (lfo)
        {
        case CHO_LFO_SIN0:
            lfoval = this->sinLFO[0].sin();
            break;
        case CHO_LFO_SIN1:
            lfoval = this->sinLFO[1].sin();
            break;
        case CHO_LFO_RMP0:
            lfoval = this->rampLFO[0].getValue();
            break;
        case CHO_LFO_RMP1:
            lfoval = this->rampLFO[1].getValue();
            break;
        case CHO_LFO_COS0:
        case CHO_LFO_4:
            lfoval = this->sinLFO[0].cos();
            break;
        case CHO_LFO_COS1:
        case CHO_LFO_5:
            lfoval = this->sinLFO[1].cos();
            break;
        default:
            break;
        }

        this->acc.set(lfoval);
    }

    INLINE void CLR()
    {
        ACC2PACC();
        this->acc.clear();
    }

    INLINE void NOT()
    {
        ACC2PACC();
        this->acc.not_();
    }

    INLINE void ABSA()
    {
        ACC2PACC();
        this->acc.abs();
    }

    INLINE void LDAX(int addr)
    {
        _LDAX(&regs[addr]);
    }

    INLINE void _LDAX(Reg *addr)
    {
        ACC2PACC();
        this->acc.set(*addr);
    }

public:
    FV1() : rom(nullptr)
    {
        this->sinLFO[0].init(this->regs[SIN0_RATE], this->regs[SIN0_RANGE]);
        this->sinLFO[1].init(this->regs[SIN1_RATE], this->regs[SIN1_RANGE]);
        this->rampLFO[0].init(this->regs[RMP0_RATE], this->regs[RMP0_RANGE]);
        this->rampLFO[1].init(this->regs[RMP1_RATE], this->regs[RMP1_RANGE]);
    }

#ifdef FV1_DUMP_STATE
    void dump(std::string &str, const std::string &endl = "")
    {
        std::ostringstream out;

        out << "PC: " << this->pc << endl
            << " ACC: " << std::hex << this->acc.getValue() << endl
            << " POT0: " << this->REG(POT0) << endl
            << " POT1: " << this->REG(POT1) << endl
            << " POT2: " << this->REG(POT2) << endl
            << " RMP0_RATE: " << this->REG(RMP0_RATE) << endl
            << " RMP0_RANGE: " << this->REG(RMP0_RANGE) << endl
            << " RMP0_AMP: " << FixedPoint(this->rampLFO[0].getRange()) << endl
            << " RMP0_VALUE: " << FixedPoint(this->rampLFO[0].getValue()) << endl
            << " RMP0_XFADE " << FixedPoint(this->rampLFO[0].getXFade()) << endl
            << " RMP1_RATE: " << FixedPoint(this->REG(RMP1_RATE)) << endl
            << " RMP1_RANGE: " << FixedPoint(this->REG(RMP1_RANGE)) << endl
            << " RMP1_VALUE: " << FixedPoint(this->rampLFO[1].getValue()) << endl
            << " RMP1_XFADE " << FixedPoint(this->rampLFO[1].getXFade()) << endl
            << " SIN0_RATE: " << this->REG(SIN0_RATE) << endl
            << " SIN0_RANGE: " << this->REG(SIN0_RANGE) << endl
            << " SIN0_SIN: " << FixedPoint(this->sinLFO[0].getValue()) << endl
            << " SIN1_RATE: " << this->REG(SIN1_RATE) << endl
            << " SIN1_RANGE: " << this->REG(SIN1_RANGE) << endl
            << " SIN1_SIN: " << FixedPoint(this->sinLFO[1].getValue()) << endl
            << " DACL: " << this->REG(DACL) << endl
            << " DACR: " << this->REG(DACR) << endl;

        for (int i = REG0; i < REG31; i++)
            out << "REG" << i << ": " << this->regs[i] << endl;

        out << endl;

        str += out.str();
    }
#endif

    void execute(float inL, float inR, float pot0, float pot1, float pot2, float &outL, float &outR)
    {
        execute(&inL, &inR, pot0, pot1, pot2, &outL, &outR, 1);
    }

    void execute(const float *inL, const float *inR, float pot0, float pot1, float pot2, float *outL, float *outR, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            this->REG_SET(ADCL, FixedPoint::fromFloat(inL[i]).getValue());
            this->REG_SET(ADCR, FixedPoint::fromFloat(inR[i]).getValue());
            this->REG_SET(POT0, FixedPoint::fromFloat(pot0).getValue());
            this->REG_SET(POT1, FixedPoint::fromFloat(pot1).getValue());
            this->REG_SET(POT2, FixedPoint::fromFloat(pot2).getValue());

            program(this);

            firstRun = false;

            this->delay.decrPtr();

            this->sinLFO[0].increment();
            this->sinLFO[1].increment();
            this->rampLFO[0].increment();
            this->rampLFO[1].increment();

            outL[i] = this->REG(DACL).toFloat();
            outR[i] = this->REG(DACR).toFloat();
        }
    }

    void setFx(void (*program)(FV1 *fv1))
    {

        this->firstRun = true;
        this->acc.clear();
        this->pacc.clear();

        for (int i = REG0; i < REG31; i++)
            this->regs[i].clear();

        this->program = program;
    }
};