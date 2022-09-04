#include "base/FaustEngine.hxx"
#include "faust/djembe.hxx"
#include "faust/rev_dattorro.hxx"

void init_faust()
{
    //(char[sizeof(mydsp)])"";

    machine::add<FaustEngine<djembe, machine::TRIGGER_INPUT>>(machine::DRUM, "Djembe");
    machine::add<FaustEngine<rev_dattorro, machine::AUDIO_PROCESSOR>>(machine::FX, "Rev-Dattorro");
}

MACHINE_INIT(init_faust);
