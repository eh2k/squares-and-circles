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