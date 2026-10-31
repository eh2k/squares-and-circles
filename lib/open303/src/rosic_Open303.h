#ifndef rosic_Open303_h
#define rosic_Open303_h

#include "rosic_MidiNoteEvent.h"
#include "rosic_BlendOscillator.h"
#include "rosic_BiquadFilter.h"
#include "rosic_TeeBeeFilter.h"
#include "rosic_DecayEnvelope.h"
#include "rosic_LeakyIntegrator.h"
#include "rosic_OnePoleFilter.h"
#include "rosic_Character.h"
#include "rosic_CharacterOpen303.h"

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

  [eh2k] The voice of Open303 (its wavetable oscillator, TeeBeeFilter, envelopes and one-pole
  filters) with a control whose every constant is a character (rosic_Character.h), switched with
  setCharacter(): Open303's own voicing (rosic_CharacterOpen303.h, the default) or one an app
  brings, e.g. fitted to recordings of another voice (see ../README.md). The parameters keep Open303's
  units: setCutoff() takes 314...2394 Hz, setDecay() 200...2000 ms (both read as knob
  positions), setResonance(), setEnvMod() and setAccent() percent.

  */

  class Open303
  {

  public:

    //-----------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor, with Open303's own character. The oscillator reads the saw table; the square
    (setWaveform(1)) is a pulse made from two reads of it, the second table is not used. */
    Open303(const rosic::WaveTable *saw, const rosic::WaveTable *square);

    /** Destructor. */
    ~Open303();

    //-----------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate (in Hz). */
    void setSampleRate(real_t newSampleRate, int oversampling = 1);

    /** Sets the character (rosic_Character.h; it must stay valid while it is used). A change takes
    effect with the next note (not on a slide), before the first note at once: switching in the
    middle of a note would click. */
    void setCharacter(const Character *newCharacter);

    /** Sets up the waveform: 0 saw, 1 pulse (values in between mix the two). */
    void setWaveform(real_t newWaveform) { oscillator.setBlendFactor(newWaveform); }

    /** Sets the master tuning frequency for note A4 (usually 440 Hz). */
    void setTuning(real_t newTuning) { tuning = newTuning; }

    /** Sets the cutoff knob as Open303's nominal cutoff, 314...2394 Hz. */
    void setCutoff(real_t newCutoff);

    /** Sets the resonance knob (in percent). */
    void setResonance(real_t newResonance);

    /** Sets the env mod knob (in percent). */
    void setEnvMod(real_t newEnvMod);

    /** Sets the decay knob as Open303's decay time for non-accented notes, 200...2000 ms. Accented
    notes have a decay of their own. */
    void setDecay(real_t newDecay);

    /** Sets the accent knob (in percent).  */
    void setAccent(real_t newAccent);

    /** Sets the master volume level (in dB). */
    void setVolume(real_t newVolume);

    /** Sets the slide-time: the time constant of the glide (in ms, two one-poles in series). */
    void setSlideTime(real_t newSlideTime);

    /** Sets the time constant of the RC that smooths the accent's level (in ms). */
    void setAccentAttack(real_t newAccentAttack)
    {
      accentAttack = newAccentAttack;
      rc2.setTimeConstant(accentAttack);
    }

    //-----------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the waveform as a continuous value between 0...1 where 0 means pure saw and 1 means
    pure pulse. */
    real_t getWaveform() const { return oscillator.getBlendFactor(); }

    /** Sets the master tuning frequency for note A4 (usually 440 Hz). */
    real_t getTuning() const { return tuning; }

    /** Returns the cutoff knob (in Hz, as set). */
    real_t getCutoff() const { return cutoff; }

    /** Returns the resonance knob (in percent) */
    real_t getResonance() const { return 100.0 * resonance; }

    /** Returns the env mod knob (in percent). */
    real_t getEnvMod() const { return envMod; }

    /** Returns the decay knob (in ms, as set). */
    real_t getDecay() const { return normalDecay; }

    /** Returns the accent (in percent). */
    real_t getAccent() const { return 100.0 * accent; }

    /** Returns the master volume level (in dB). */
    real_t getVolume() const { return level; }

    /** Returns the slide-time (in ms). */
    real_t getSlideTime() const { return slideTime; }

    /** Returns the time constant of the accent level's RC (in ms). */
    real_t getAccentAttack() const { return accentAttack; }

    /** Returns the character (the one sounding; a new one waits for the next note). */
    const Character *getCharacter() const { return character; }

    //-----------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates onse output sample at a time. [eh2k] With an input, that sample goes through the
    voice in place of the oscillator's (the highpass, the filter, the amp: the 303 as a filter and VCA
    for an external signal); the oscillator stands. */
    INLINE real_t getSample(const real_t *input = nullptr);

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
    DecayEnvelope             mainEnv;
    LeakyIntegrator           pitchSlewLimiter, pitchSlewLimiter2;
    LeakyIntegrator           rc1, rc2, rc4;
    OnePoleFilter             highpass1, highpass2, allpass;
    BiquadFilter              notch, ampDeClicker;

  protected:

    /** Triggers a note; while the gate is still on (a note held) it slides to it instead if the
    character says so (legatoSlides: with overlapping notes). The note may be fractional. */
    void triggerNote(real_t noteNumber, bool hasAccent);

    /** Slides to a note (no new envelopes). */
    void slideToNote(real_t noteNumber, bool hasAccent);

    /** Releases a note (the gate goes off). */
    void releaseNote(real_t noteNumber);

    /** Turns the knobs into the control's terms: cutoff, env and accent depth, resonance and gain. */
    void calculateControl();

    /** A setter changed a knob: calculateControl() before the next sample or note. */
    void controlChange() { controlChanged = true; }

    /** Sets up what depends on the character and the sample rate: time constants, filters. */
    void calculateCharacter();

    /** Makes a character the one sounding. */
    void applyCharacter(const Character *newCharacter);

    const Character *character;
    const Character *pendingCharacter; // setCharacter()'s, for the next note

    int oversampling = 1;

    real_t tuning;           // master tunung for A4 in Hz
    real_t ampScaler;        // final volume as raw factor
    real_t oscFreq;          // frequecy of the oscillator (without pitchbend)
    real_t sampleRate;       // the (non-oversampled) sample rate
    real_t level;            // master volume level (in dB)
    real_t accent;           // the accent knob, 0...1
    real_t slideTime;        // the glide's time constant (in ms)
    real_t cutoff;           // the cutoff knob (in Hz)
    real_t resonance;        // the resonance knob, 0...1
    real_t envMod;           // the env mod knob (in percent)
    real_t normalDecay;      // the decay knob (in ms)
    real_t accentAttack;     // the accent RC's time constant (in ms)
    real_t accentGain;       // 1 on an accented note, else 0
    real_t accentHeld;       // accent knob on an accented note, else 0
    real_t pitchWheelFactor; // scale factor for oscillator frequency from pitch-wheel

    // the control's terms (calculateControl): log2 of the cutoff in Hz = cutBase + cutEnv*shaped env +
    // cutAccent*accent sweep; the filter's (skewed) resonance = resBase + resEnv*env; the gain in
    // log2 = gainBase + gainEnv*env + gainAccent*accentEnv; the accent's level on the amp
    real_t cutBase, cutEnv, cutAccent, resBase, resEnv, gainBase, gainEnv, ampAccent;
    real_t normalDecayTau;   // the filter envelope's time constant for non-accented notes (in ms)
    real_t gateLevel;        // the amp's gate, smoothed
    real_t gateAttack, gateRelease, gateReleaseAccent; // its one-pole coefficients
    real_t noteRelease;      // the release of the note sounding (gateRelease or gateReleaseAccent)
    real_t gatedEnv;         // ampEnvGated + ampAccentGated * accent knob on an accented note

    // the character's values getSample() uses, copied (calculateCharacter())
    real_t accentThreshold, accentThresholdScale, envShape, envShapeNorm, cutMin, cutMax, ampTarget, gainAccent;
    bool   hardGate, ampSmoothing;
    real_t ampLevel, ampCoeff; // the amp envelope and its one-pole coefficient
    real_t accentLevel, accentLevelCoeff; // the accent level's envelope (restarted by accented notes)
    bool   accentCircuit;    // the character's: the accent's sweep as the TB-303's circuit
    real_t accentDiode;      // its diode's drop
    real_t sweepSamples;     // the time constant of its pot and capacitor in samples
    real_t sweepCap;         // its capacitor's voltage
    real_t sweepOn, sweepIn, sweepS, sweepC, sweepCoeffOn, sweepCoeffOff; // calculateControl()'s, per resonance
    int    gateHoldSamples, gateHoldLeft; // the amp's gate after the gate
    bool   gateOn;
    bool   controlChanged;   // calculateControl() is due

    int    currentNote;      // note which is currently played (-1 if none)
    int    currentVel;       // velocity of currently played note
    bool   idle;             // flag to indicate that we have currently nothing to do in getSample
    list<MidiNoteEvent> noteList;

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE real_t Open303::getSample(const real_t *input)
  {
    if( idle )
      return 0.0;

    if( controlChanged ) // [eh2k] a knob moved: once for all the setters of a block
      calculateControl();

    // calculate instantaneous oscillator frequency and set up the oscillator:
    const real_t instFreq = pitchSlewLimiter2.getSample(pitchSlewLimiter.getSample(oscFreq));

    // the filter envelope, its attack RC, the accent's level and its sweep's RC (fed only on accented notes):
    const real_t env  = mainEnv.getSample();
    const real_t env1 = rc1.getSample(env);
    accentLevel *= accentLevelCoeff;                       // the accent's level: its own envelope
    const real_t accentAmount = accentLevel > accentThreshold // above a threshold
      ? (accentLevel - accentThreshold) * accentThresholdScale : 0.0;
    const real_t env2 = rc2.getSample(accentAmount);
    real_t env2c;                                          // the accent's sweep
    if( accentCircuit ) // the TB-303's: the knob's share of the envelope through a diode into a pot and a capacitor
    {
      const real_t s = accentHeld*env - accentDiode;
      if( s > sweepCap*sweepOn ) // the diode conducts, the capacitor charges
      {
        sweepCap += (sweepIn*s - sweepCap) * sweepCoeffOn;
        env2c = sweepS*s + sweepC*sweepCap;
      }
      else                       // it drains through the wiper
      {
        sweepCap -= sweepCap * sweepCoeffOff;
        env2c = sweepCap*sweepOn;
      }
    }
    else
      env2c = rc4.getSample(accentGain * env);

    // log2 of the cutoff in Hz; the envelope adds between octaves (envShape 0) and Hz (large envShape)
    const real_t shaped = envShape > 0.0 ? log2f(1.0 + envShape*env1) * envShapeNorm : env1;
    real_t u = cutBase + cutEnv*shaped + cutAccent*env2c;
    if( u > cutMax )
      u = cutMax;
    if( u < cutMin )
      u = cutMin;
    filter.setCutoff(exp2f(u), false);
    filter.setResonanceSkewed(resBase + resEnv*env1);         // updates the coefficients

    // the amp: the gate times the amp envelope and some of the filter envelope (or that behind a
    // hard gate), and the accent's level, which goes on after the gate
    if( gateOn || gateHoldLeft > 0 )
    {
      gateLevel += (1.0 - gateLevel) * gateAttack;
      if( !gateOn )
        gateHoldLeft--;
    }
    else
      gateLevel -= gateLevel * noteRelease;
    ampLevel += (ampTarget - ampLevel) * ampCoeff; // the amp envelope: towards its target, held at 0
    if( ampLevel < 0.0 )
      ampLevel = 0.0;
    const real_t gated = gatedEnv*env1;
    real_t amp;
    if( hardGate )
      amp = gateLevel * ampLevel + (gateOn ? gated : 0.0) + ampAccent*env2;
    else
      amp = gateLevel * (ampLevel + gated) + ampAccent*env2;
    if( ampSmoothing )
      amp = ampDeClicker.getSample(amp);
    amp *= exp2f(gainBase + gainEnv*env1 + gainAccent*env2); // the gain, log2

    real_t tmp;
    if( input )                             // [eh2k] an external signal
      tmp = *input;
    else
    {
      oscillator.setFrequency(instFreq*pitchWheelFactor);
      oscillator.calculateIncrement();
      tmp = -oscillator.getSample();        // the raw oscillator signal
    }
    tmp  = highpass1.getSample(tmp);        // pre-filter highpass
    tmp  = filter.getSample(tmp);           // now it's filtered
    tmp  = allpass.getSample(tmp);
    tmp  = highpass2.getSample(tmp);
    tmp  = notch.getSample(tmp);
    tmp *= amp;                             // amplified
    tmp *= ampScaler;

    return tmp;
  }

}

#endif
