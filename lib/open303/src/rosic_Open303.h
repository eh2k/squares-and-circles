#ifndef rosic_Open303_h
#define rosic_Open303_h

#include "rosic_MidiNoteEvent.h"
#include "rosic_BlendOscillator.h"
#include "rosic_BiquadFilter.h"
#include "rosic_TeeBeeFilter.h"
#include "rosic_AnalogEnvelope.h"
#include "rosic_DecayEnvelope.h"
#include "rosic_LeakyIntegrator.h"
#include "rosic_EllipticQuarterBandFilter.h"
#ifdef SEQUENCER
#include "rosic_AcidSequencer.h"
#endif
#define FLASHMEM_WAVETABLE
#ifndef FLASHMEM_WAVETABLE
#include "wavetable_gen/rosic_MipMappedWaveTable.h"
#endif

// #include <list>
// using namespace std; // for the noteList
template <class T>
struct list
{
  T _front;

  bool empty()
  {
    return _front == T();
  }

  void clear()
  {
    _front = T();
  }

  T &front()
  {
    return _front;
  }

  void push_front(T &e)
  {
    _front = e;
  }

  void remove(T &e)
  {
    if (_front == e)
      _front = T();
  }
};

namespace rosic
{

  /**

  This is a monophonic bass-synth that aims to emulate the sound of the famous Roland TB 303 and
  goes a bit beyond.

  */

  class Open303
  {

  public:

    //-----------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    Open303(const rosic::WaveTable *saw, const rosic::WaveTable *square);

    /** Destructor. */
    ~Open303();

    //-----------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate (in Hz). */
    void setSampleRate(real_t newSampleRate, int oversampling = 4);

    /** Sets up the waveform continuously between saw and square - the input should be in the range 
    0...1 where 0 means pure saw and 1 means pure square. */
    void setWaveform(real_t newWaveform) { oscillator.setBlendFactor(newWaveform); }

    /** Sets the master tuning frequency for note A4 (usually 440 Hz). */
    void setTuning(real_t newTuning) { tuning = newTuning; }

    /** Sets the filter's nominal cutoff frequency (in Hz). */
    void setCutoff(real_t newCutoff); 

    /** Sets the resonance amount for the filter. */
    void setResonance(real_t newResonance) { filter.setResonance(newResonance); }

    /** Sets the modulation depth of the filter's cutoff frequency by the filter-envelope generator 
    (in percent). */
    void setEnvMod(real_t newEnvMod);

    /** Sets the main envelope's decay time for non-accented notes (in milliseconds). 
    Devil Fish provides range of 30...3000 ms for this parameter. On the normal 303, this 
    parameter had a range of 200...2000 ms.  */
    void setDecay(real_t newDecay) { normalDecay = newDecay; }

    /** Sets the accent (in percent).  */
    void setAccent(real_t newAccent);

    /** Sets the master volume level (in dB). */
    void setVolume(real_t newVolume);     

    //  from here: parameter settings which were not available to the user in the 303:

    /** Sets the amplitudes envelope's sustain level in decibels. Devil Fish uses the second half 
    of the range of the (amplitude) decay pot for this and lets the user adjust it between 0 
    and 100% of the full volume. In the normal 303, this parameter was fixed to zero. */
    void setAmpSustain(real_t newAmpSustain) { ampEnv.setSustainInDecibels(newAmpSustain); }

    /** Sets the cutoff frequency for the highpass before the main filter. */
    void setPreFilterHighpass(real_t newCutoff) { highpass1.setCutoff(newCutoff); }

    /** Sets the cutoff frequency for the highpass inside the feedback loop of the main filter. */
    void setFeedbackHighpass(real_t newCutoff) { filter.setFeedbackHighpassCutoff(newCutoff); }

    /** Sets the cutoff frequency for the highpass after the main filter. */
    void setPostFilterHighpass(real_t newCutoff) { highpass2.setCutoff(newCutoff); }

    /** Sets the slide-time (in ms). The TB-303 had a slide time of 60 ms. */
    void setSlideTime(real_t newSlideTime);

    /** Sets the filter envelope's attack time for non-accented notes (in milliseconds). 
    Devil Fish provides range of 0.3...30 ms for this parameter. */
    void setNormalAttack(real_t newNormalAttack) 
    { 
      normalAttack = newNormalAttack; 
      rc1.setTimeConstant(normalAttack);
    }

    /** Sets the filter envelope's attack time for accented notes (in milliseconds). In the 
    Devil Fish, accented notes have a fixed attack time of 3 ms.  */
    void setAccentAttack(real_t newAccentAttack) 
    { 
      accentAttack = newAccentAttack; 
      rc2.setTimeConstant(accentAttack);
    }

    /** Sets the filter envelope's decay time for accented notes (in milliseconds). 
    Devil Fish provides range of 30...3000 ms for this parameter. On the normal 303, this 
    parameter was fixed to 200 ms.  */
    void setAccentDecay(real_t newAccentDecay) { accentDecay = newAccentDecay; }

    /** Sets the amplitudes envelope's decay time (in milliseconds). Devil Fish provides range of 
    16...3000 ms for this parameter. On the normal 303, this parameter was fixed to 
    approximately 3-4 seconds.  */
    void setAmpDecay(real_t newAmpDecay) { ampEnv.setDecay(newAmpDecay); }

    /** Sets the amplitudes envelope's release time (in milliseconds). On the normal 303, this 
    parameter was fixed to .....  */
    void setAmpRelease(real_t newAmpRelease) 
    { 
      normalAmpRelease = newAmpRelease;
      ampEnv.setRelease(newAmpRelease); 
    }

    //-----------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the waveform as a continuous value between 0...1 where 0 means pure saw and 1 means 
    pure square. */
    real_t getWaveform() const { return oscillator.getBlendFactor(); }

    /** Sets the master tuning frequency for note A4 (usually 440 Hz). */
    real_t getTuning() const { return tuning; }

    /** Returns the filter's nominal cutoff frequency (in Hz). */
    real_t getCutoff() const { return cutoff; }

    /** Returns the filter's resonance amount (in percent) */
    real_t getResonance() const { return filter.getResonance(); }

    /** Returns the modulation depth of the filter's cutoff frequency by the filter-envelope 
    generator (in percent). */
    real_t getEnvMod() const { return envMod; }

    /** Returns the filter envelope's decay time for non-accented notes (in milliseconds). */
    real_t getDecay() const { return normalDecay; }

    /** Returns the accent (in percent). */
    real_t getAccent() const { return 100.0 * accent; }

    /** Returns the master volume level (in dB). */
    real_t getVolume() const { return level; }

    //  from here: parameters which were not available to the user in the 303:

    /** Returns the amplitudes envelope's sustain level (in dB). */
    real_t getAmpSustain() const { return amp2dB(ampEnv.getSustain()); }

    /** Returns the cutoff frequency for the highpass before the main filter. */
    real_t getPreFilterHighpass() const { return highpass1.getCutoff(); }

    /** Retruns the cutoff frequency for the highpass inside the feedback loop of the main 
    filter. */
    real_t getFeedbackHighpass() const { return filter.getFeedbackHighpassCutoff(); }

    /** Returns the cutoff frequency for the highpass after the main filter. */
    real_t getPostFilterHighpass() const { return highpass2.getCutoff(); }

    /** Returns the slide-time (in ms). */
    real_t getSlideTime() const { return slideTime; }

    /** Returns the filter envelope's attack time for non-accented notes (in milliseconds). */
    real_t getNormalAttack() const { return normalAttack; }

    /** Returns the filter envelope's attack time for non-accented notes (in milliseconds). */
    real_t getAccentAttack() const { return accentAttack; }

    /** Returns the filter envelope's decay time for non-accented notes (in milliseconds). */
    real_t getAccentDecay() const { return accentDecay; }

    /** Returns the amplitudes envelope's decay time (in milliseconds). */
    real_t getAmpDecay() const { return ampEnv.getDecay(); }

    /** Returns the amplitudes envelope's release time (in milliseconds). */
    real_t getAmpRelease() const { return normalAmpRelease; }

    //-----------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates onse output sample at a time. */
    INLINE real_t getSample(); 

    //-----------------------------------------------------------------------------------------------
    // event handling:

    /** Accepts note-on events (note offs are also handled here as note ons with velocity zero). */ 
    void noteOn(int noteNumber, int velocity, real_t detune);

    /** Turns all possibly running notes off. */
    void allNotesOff();

    /** Sets the pitchbend value in semitones. */ 
    void setPitchBend(real_t newPitchBend);  

    //-----------------------------------------------------------------------------------------------
    // embedded objects: 
    BlendOscillator           oscillator;
    TeeBeeFilter              filter;
    AnalogEnvelope            ampEnv; 
    DecayEnvelope             mainEnv;
    LeakyIntegrator           pitchSlewLimiter;
    //LeakyIntegrator           ampDeClicker;
    BiquadFilter              ampDeClicker;
    LeakyIntegrator           rc1, rc2;
    OnePoleFilter             highpass1, highpass2, allpass; 
    BiquadFilter              notch;
    EllipticQuarterBandFilter antiAliasFilter;
#ifdef SEQUENCER
    AcidSequencer             sequencer;
#endif

  protected:

    /** Triggers a note (called either directly in noteOn or in getSample when the sequencer is 
    used). */
    void triggerNote(int noteNumber, bool hasAccent);

    /** Slides to a note (called either directly in noteOn or in getSample when the sequencer is 
    used). */
    void slideToNote(int noteNumber, bool hasAccent);

    /** Releases a note (called either directly in noteOn or in getSample when the sequencer is 
    used). */
    void releaseNote(int noteNumber);

    /** Sets the decay-time of the main envelope and updates the normalizers n1, n2 accordingly. */
    void setMainEnvDecay(real_t newDecay);

    void calculateEnvModScalerAndOffset();

    /** Updates the normalizer n1 according to the time-constant of rc1 and the decay-time of the
    main envelope generator. */
    void updateNormalizer1();

    /** Updates the normalizer n2 according to the time-constant of rc2 and the decay-time of the
    main envelope generator. */
    void updateNormalizer2();

    int oversampling = 1;

    real_t tuning;           // master tunung for A4 in Hz
    real_t ampScaler;        // final volume as raw factor
    real_t oscFreq;          // frequecy of the oscillator (without pitchbend)
    real_t sampleRate;       // the (non-oversampled) sample rate
    real_t level;            // master volume level (in dB)
    real_t levelByVel;       // velocity dependence of the level (in dB)
    real_t accent;           // scales all "byVel" parameters
    real_t slideTime;        // the time to slide from one note to another (in ms)
    real_t cutoff;           // nominal cutoff frequency of the filter
    real_t envMod;           // strength of the envelope modulation in percent
    real_t envUpFraction;    // fraction of the envelope that goes upward
    real_t envOffset;        // offset for the normalized envelope ('bipolarity' parameter)
    real_t envScaler;        // scale-factor for the normalized envelope (derived from envMod)
    real_t normalAttack;     // attack time for the filter envelope on non-accented notes
    real_t accentAttack;     // attack time for the filter envelope on accented notes
    real_t normalDecay;      // decay time for the filter envelope on non-accented notes
    real_t accentDecay;      // decay time for the filter envelope on accented notes
    real_t normalAmpRelease; // amp-env release time for non-accented notes
    real_t accentAmpRelease; // amp-env release time for accented notes
    real_t accentGain;       // between 0.0...1.0 - to scale the 3rd amp-envelope on accents
    real_t pitchWheelFactor; // scale factor for oscillator frequency from pitch-wheel
    real_t n1, n2;           // normalizers for the RCs that are driven by the MEG
    int    currentNote;      // note which is currently played (-1 if none)
    int    currentVel;       // velocity of currently played note
    int    noteOffCountDown; // a countdown variable till next note-off in sequencer mode
    bool   slideToNextNote;  // indicate that we need to slide to the next note in sequencer mode
    bool   idle;             // flag to indicate that we have currently nothing to do in getSample

    list<MidiNoteEvent> noteList;

  };

  //-------------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE real_t Open303::getSample()
  {
    //if( sequencer.getSequencerMode() == AcidSequencer::OFF && ampEnv.endIsReached() )
    //  return 0.0;
    if( idle )
      return 0.0;

#ifdef SEQUENCER
    // check the sequencer if we have some note to trigger:
    if( sequencer.getSequencerMode() != AcidSequencer::OFF )
    {
      noteOffCountDown--;
      if( noteOffCountDown == 0 || sequencer.isRunning() == false )
        releaseNote(currentNote);

      AcidNote *note = sequencer.getNote();
      if( note != NULL )
      {
        if( note->gate == true && currentNote != -1)
        {
          int key = note->key + 12*note->octave + currentNote;
          key = clip(key, 0, 127);

          if( !slideToNextNote )
            triggerNote(key, note->accent);
          else
            slideToNote(key, note->accent);

          AcidNote* nextNote = sequencer.getNextScheduledNote();
          if( note->slide && nextNote->gate == true )
          {
            noteOffCountDown = INT32_MAX;
            slideToNextNote  = true;
          }
          else
          {
            noteOffCountDown = sequencer.getStepLengthInSamples();
            slideToNextNote  = false;
          }
        }
      }
    }
#endif
    // calculate instantaneous oscillator frequency and set up the oscillator:
    const real_t instFreq = pitchSlewLimiter.getSample(oscFreq);

    // calculate instantaneous cutoff frequency from the nominal cutoff and all its modifiers and 
    // set up the filter:
    real_t mainEnvOut = mainEnv.getSample();
    real_t tmp1       = n1 * rc1.getSample(mainEnvOut);
    real_t tmp2       = 0.0;
    if( accentGain > 0.0 )
      tmp2 = mainEnvOut;
    tmp2 = n2 * rc2.getSample(tmp2);  
    tmp1 = envScaler * ( tmp1 - envOffset );  // seems not to work yet
    tmp2 = accentGain*tmp2;
    real_t instCutoff = cutoff * pow(2.0, tmp1+tmp2);
    filter.setCutoff(instCutoff);

    real_t ampEnvOut = ampEnv.getSample();
    //ampEnvOut += 0.45*filterEnvOut + accentGain*6.8*filterEnvOut; 
    if( ampEnv.isNoteOn() )
      ampEnvOut += 0.45*mainEnvOut + accentGain*4.0*mainEnvOut; 
    ampEnvOut = ampDeClicker.getSample(ampEnvOut);

    oscillator.setFrequency(instFreq*pitchWheelFactor);
    oscillator.calculateIncrement();
    // oversampled calculations:
    real_t tmp;
    for(int i=1; i<=oversampling; i++)
    {
      tmp  = -oscillator.getSample();         // the raw oscillator signal 
      tmp  = highpass1.getSample(tmp);        // pre-filter highpass
      tmp  = filter.getSample(tmp);           // now it's filtered
      tmp  = antiAliasFilter.getSample(tmp);  // anti-aliasing filtered
    }

    // these filters may actually operate without oversampling (but only if we reset them in
    // triggerNote - avoid clicks)
    tmp  = allpass.getSample(tmp);
    tmp  = highpass2.getSample(tmp);        
    tmp  = notch.getSample(tmp);
    tmp *= ampEnvOut;                       // amplified
    tmp *= ampScaler;

    // find out whether we may switch ourselves off for the next call:
    idle = false;
    //idle = (sequencer.getSequencerMode() == AcidSequencer::OFF && ampEnv.endIsReached() 
    //        && fabs(tmp) < 0.000001); // ampEnvOut < 0.000001;

    return tmp;
  }

}

#endif 
