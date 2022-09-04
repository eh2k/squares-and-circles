// Copyright (C)2021 - Eduard Heidt
//
// Author: Eduard Heidt (eh2k@gmx.de)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//

#include "base/SampleEngine.hxx"

template <>
float tsample_spec<float>::get_float(int index) const
{
    if (index < this->len && index >= 0)
        return reinterpret_cast<const float*>(this->data)[index << this->addr_shift];
    else
        return 0;
}

template <>
float tsample_spec<uint8_t>::get_float(int index) const
{
    if (index < this->len && index >= 0)
        return ((float)reinterpret_cast<const uint8_t*>(this->data)[index << this->addr_shift] - 127) / 128;
    else
        return 0;
}

template <>
float tsample_spec<int16_t>::get_float(int index) const
{
    if (index < this->len && index >= 0)
        return (float)reinterpret_cast<const int16_t*>(this->data)[index << this->addr_shift] / INT16_MAX;
    else
        return 0;
}