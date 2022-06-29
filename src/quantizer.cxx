
#include "machine.h"
#include "braids/quantizer.h"
#include "braids/quantizer_scales.h"
#include "braids/settings.h"

using namespace braids;
using namespace machine;

void init_quantizer()
{
    for(size_t i = 0; i < LEN_OF(braids::scales); i++)
        machine::add_quantizer_scale(braids::settings.metadata(braids::Setting::SETTING_QUANTIZER_SCALE).strings[i], (const machine::QuantizerScale&)braids::scales[i]);
}

MACHINE_INIT(init_quantizer);