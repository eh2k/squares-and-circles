Open303 is a free and open source emulation of the famous Roland TB-303 bass synthesizer
 * https://sourceforge.net/projects/open303/
 * https://github.com/RobinSchmidt/Open303

Changes:

 * 2022-07-15: [eh2k] imported from https://svn.code.sf.net/p/open303/code/
 * 2022-07-16: [eh2k] vst/build code removed - only DspCode here
 * 2022-07-17: [eh2k] REAL_T_IS_FLOAT & real_t type
 * 2022-07-32: [eh2k] SAMPLE_RATE const + Open303::setSampleRate oversampling arg
 * 2022-08-14: [eh2k] Passing Wavatables by Open303 constructor - generate outside with MipMappedWaveTable (wavetable_gen) 
                      compileflag SEQUENCER  
 * 2026-10-23: [eh2k] characters: every constant of the control from a character (rosic_Character.h,
                      Open303::setCharacter(), Open303's own in rosic_CharacterOpen303.h), the pulse from the saw
                      table; sequencer, SAW303/SQUARE303 tables, AnalogEnvelope, EllipticQuarterBandFilter and
                      the xbm removed - see README.md
 * 2026-10-23: [eh2k] BiquadFilter::calcCoeffs() only LOWPASS12 and BANDREJECT (the app binaries' size)
 * 2026-10-23: [eh2k] the character's accent sweep can be the TB-303's circuit (accentDiode, accentR1, accentC), its
                      amp gate a hold (gateHold), appended fields; OnePoleFilter::calcCoeffs() only HIGHPASS and
                      ALLPASS, it and LeakyIntegrator::calculateCoefficient() not inlined (the app binaries' size)
