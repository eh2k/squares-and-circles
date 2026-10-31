#include "rosic_Open303.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

Open303::Open303(const rosic::WaveTable *saw, const rosic::WaveTable *square)
{
  tuning           =   440.0;
  ampScaler        =     1.0;
  oscFreq          =   440.0;
  sampleRate       = SAMPLE_RATE;
  level            =   -12.0;
  accent           =     0.0;
  character        = &characterOpen303;
  pendingCharacter = NULL;
  slideTime        = character->slideTime;
  cutoff           =  1000.0;
  resonance        =     0.5;
  envMod           =    25.0;
  normalDecay      =  1000.0;
  accentAttack     = character->accentAttack;
  accentGain       =     0.0;
  accentHeld       =     0.0;
  pitchWheelFactor =     1.0;
  gateLevel        =     0.0;
  sweepCap         =     0.0;
  gateHoldLeft     =     0;
  gateOn           = false;
  controlChanged   = true;
  currentNote      =    -1;
  currentVel       =     0;
  idle             = true;

  oscillator.setWaveTables(saw, saw); // the pulse is made from the saw table

  mainEnv.setNormalizeSum(false);
  ampLevel = 0.0;
  accentLevel = 0.0;

  highpass1.setMode(OnePoleFilter::HIGHPASS);
  highpass2.setMode(OnePoleFilter::HIGHPASS);
  allpass.setMode(OnePoleFilter::ALLPASS);
  notch.setMode(BiquadFilter::BANDREJECT);
  ampDeClicker.setMode(BiquadFilter::LOWPASS12);
  ampDeClicker.setGain(amp2dB(sqrt(0.5)));

  // tweakables:
  allpass.setCutoff(14.008);
  notch.setFrequency(7.5164);
  notch.setBandwidth(4.7);
  filter.setMode(TeeBeeFilter::TB_303);

  setSampleRate(sampleRate); // and the character's settings
}

Open303::~Open303()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void Open303::setSampleRate(real_t newSampleRate, int oversampling /*= 1*/)
{
  sampleRate = newSampleRate; // [eh2k] no oversampling
  mainEnv.setSampleRate(newSampleRate);
  pitchSlewLimiter.setSampleRate((float)newSampleRate);
  pitchSlewLimiter2.setSampleRate((float)newSampleRate);
  rc1.setSampleRate((float)newSampleRate);
  rc2.setSampleRate((float)newSampleRate);
  rc4.setSampleRate((float)newSampleRate);
  highpass1.setSampleRate(newSampleRate);
  highpass2.setSampleRate(newSampleRate);
  allpass.setSampleRate(newSampleRate);
  notch.setSampleRate(newSampleRate);
  ampDeClicker.setSampleRate(newSampleRate);
  oscillator.setSampleRate(newSampleRate);
  filter.setSampleRate(newSampleRate);
  calculateCharacter();
}

void Open303::setCharacter(const Character *newCharacter)
{
  if( idle ) // nothing sounds yet
    applyCharacter(newCharacter);
  else
    pendingCharacter = newCharacter == character ? NULL : newCharacter;
}

void Open303::applyCharacter(const Character *newCharacter)
{
  pendingCharacter = NULL;
  if( newCharacter == character )
    return;
  character    = newCharacter;
  slideTime    = character->slideTime;
  accentAttack = character->accentAttack;
  calculateCharacter();
}

// a one-pole's coefficient for a time constant in ms (0: at once); [eh2k] not inlined: smaller
__attribute__((noinline)) static real_t onePoleCoeff(real_t ms, real_t sampleRate)
{
  return ms > 0.0 ? 1.0 - expf(-1.0 / (0.001 * ms * sampleRate)) : 1.0;
}

void Open303::calculateCharacter()
{
  const Character *ch = character;
  ampCoeff          = onePoleCoeff(ch->ampDecay, sampleRate);
  accentLevelCoeff  = expf(-1.0 / (0.001 * ch->accentLevelDecay * sampleRate));
  gateAttack        = onePoleCoeff(ch->gateAttack, sampleRate);
  gateRelease       = onePoleCoeff(ch->gateRelease, sampleRate);
  gateReleaseAccent = onePoleCoeff(ch->gateReleaseAccent, sampleRate);
  noteRelease       = accentGain > 0.0 ? gateReleaseAccent : gateRelease;
  gatedEnv          = ch->ampEnvGated + ch->ampAccentGated*accentHeld;
  accentThreshold      = ch->accentLevelThreshold;
  accentThresholdScale = 1.0 / (1.0 - ch->accentLevelThreshold);
  envShape          = ch->envShape;
  envShapeNorm      = ch->envShapeNorm;
  cutMin            = ch->cutoffMin;
  cutMax            = ch->cutoffMax;
  ampTarget         = ch->ampTarget;
  gainAccent        = ch->gainAccent;
  hardGate          = ch->hardGate;
  sweepSamples      = 0.001 * ch->accentC * sampleRate;
  accentCircuit     = sweepSamples > 0.0;
  accentDiode       = ch->accentDiode;
  gateHoldSamples   = (int)(0.001 * ch->gateHold * sampleRate);
  ampSmoothing      = ch->ampSmoothing > 0.0;
  rc1.setTimeConstant(ch->envAttack);
  rc2.setTimeConstant(accentAttack);
  rc4.setTimeConstant(ch->accentSweepAttack);
  highpass1.setCutoff(ch->highpass1);
  highpass2.setCutoff(ch->highpass2);
  filter.setFeedbackHighpassCutoff(ch->feedbackHighpass);
  if( ch->ampSmoothing > 0.0 )
    ampDeClicker.setFrequency(ch->ampSmoothing);
  oscillator.setPulse(ch->pulseWidth, ch->pulseSag, ch->pulseGain, ch->pulseSmoothing, ch->tableOffset);
  setSlideTime(slideTime);
  calculateControl();
}

// [eh2k] the knobs: calculateControl() once before the next sample or note, for any number of them, and only
// when one changed (an app sets them all every block)
void Open303::setCutoff(real_t newCutoff)
{
  if( newCutoff != cutoff )
    controlChange();
  cutoff = newCutoff;
}

void Open303::setResonance(real_t newResonance)
{
  if( 0.01 * newResonance != resonance )
    controlChange();
  resonance = 0.01 * newResonance;
}

void Open303::setEnvMod(real_t newEnvMod)
{
  if( newEnvMod != envMod )
    controlChange();
  envMod = newEnvMod;
}

void Open303::setDecay(real_t newDecay)
{
  if( newDecay != normalDecay )
    controlChange();
  normalDecay = newDecay;
}

void Open303::setAccent(real_t newAccent)
{
  if( 0.01 * newAccent != accent )
    controlChange();
  accent = 0.01 * newAccent;
}

void Open303::setVolume(real_t newLevel)
{
  level     = newLevel;
  ampScaler = dB2amp(level);
}

void Open303::setSlideTime(real_t newSlideTime)
{
  if( newSlideTime >= 0.0 )
  {
    slideTime = newSlideTime;
    pitchSlewLimiter.setTimeConstant((float)slideTime);
    pitchSlewLimiter2.setTimeConstant((float)(character->slide2*slideTime));
  }
}

void Open303::setPitchBend(real_t newPitchBend)
{
  pitchWheelFactor = pitchOffsetToFreqFactor(newPitchBend);
}

//-------------------------------------------------------------------------------------------------
// others:

void Open303::noteOn(int noteNumber, int velocity, real_t detune)
{
  if( velocity == 0 ) // velocity zero indicates note-off events
  {
    MidiNoteEvent releasedNote(noteNumber, 0);
    noteList.remove(releasedNote);
    if( noteList.empty() )
    {
      currentNote = -1;
      currentVel  = 0;
    }
    else
    {
      currentNote = noteList.front().getKey();
      currentVel  = noteList.front().getVelocity();
    }
    releaseNote(noteNumber);
  }
  else // velocity was not zero, so this is an actual note-on
  {
    // check if the note-list is empty (indicating that currently no note is playing) - if so,
    // trigger a new note, otherwise, slide to the new note:
    if( noteList.empty() )
      triggerNote(noteNumber, velocity >= 100);
    else
      slideToNote(noteNumber, velocity >= 100);

    currentNote = noteNumber;
    currentVel  = 64;

    // and we need to add the new note to our list, of course:
    MidiNoteEvent newNote(noteNumber, velocity);
    noteList.push_front(newNote);
  }
  idle = false;
}

void Open303::allNotesOff()
{
  noteList.clear();
  gateOn      = false;
  currentNote = -1;
  currentVel  = 0;
}

void Open303::triggerNote(real_t noteNumber, bool hasAccent)
{
  if( pendingCharacter != NULL ) // a new character starts with a note
    applyCharacter(pendingCharacter);
  if( controlChanged ) // the knobs set before this note
    calculateControl();
  if( gateOn && character->legatoSlides ) // the previous note is still held: slide
  {
    slideToNote(noteNumber, hasAccent);
    return;
  }
  accentGain  = hasAccent ? 1.0 : 0.0;
  accentHeld  = hasAccent ? accent : 0.0;
  noteRelease = hasAccent ? gateReleaseAccent : gateRelease;
  gatedEnv    = character->ampEnvGated + character->ampAccentGated*accentHeld;
  const bool knobDecay = character->accentDecayFromKnob && accent > 0.0101; // the knob above 0 (1 %)
  mainEnv.setDecayTimeConstant(hasAccent && !knobDecay ? character->accentDecay : normalDecayTau);
  oscFreq = pitchToFreq(noteNumber, tuning);
  pitchSlewLimiter.setState(oscFreq);
  pitchSlewLimiter2.setState(oscFreq);
  mainEnv.trigger();
  ampLevel = 1.0;
  if( hasAccent )
    accentLevel = 1.0;
  gateOn = true;
  idle   = false;
}

void Open303::slideToNote(real_t noteNumber, bool hasAccent)
{
  oscFreq = pitchToFreq(noteNumber, tuning);
  gateOn  = true;
  idle    = false;
}

void Open303::releaseNote(real_t noteNumber)
{
  if( gateOn ) // the amp's gate stays on a little longer
    gateHoldLeft = gateHoldSamples;
  // check if the note-list is empty now. if so, trigger a release, otherwise slide to the note
  // at the beginning of the list (this is the most recent one which is still in the list). this
  // initiates a slide back to the most recent note that is still being held:
  if( noteList.empty() )
    gateOn = false;
  else
    oscFreq = pitchToFreq(currentNote, tuning); // initiate slide back
}

// the knobs as positions 0...1 (the app's mappings undone), and the control's terms from them
void Open303::calculateControl()
{
  controlChanged = false;
  const real_t zero = 0.0, one = 1.0;
  const real_t c = clip((real_t)(logf(cutoff * (1.0 / 313.8152786059267)) * 0.4921045673846009), zero, one); // / ln(2394.41 / 313.815)
  const real_t q = clip(resonance, zero, one);
  const real_t m = clip((real_t)(0.01 * envMod), zero, one);
  const real_t d = clip((real_t)log10f(normalDecay * (1.0 / 200.0)), zero, one);
  const real_t a = clip(accent, zero, one);

  const Character *ch = character;
  normalDecayTau = expf(ch->decay[0] + d*(ch->decay[1] + d*ch->decay[2]) + c*(ch->decay[3] + m*ch->decay[4]));

  const real_t *b = ch->cutoffBase;
  cutBase   = b[0] + c*(b[1] + c*(b[2] + c*b[3])) + q*(b[4] + q*b[5] + c*b[6])
              + m*(b[7] + b[8]*c + b[9]*q + b[10]*m + b[11]*c*c) + m*m*c*(b[12] + b[13]*c + b[14]*m*c);
  const real_t *s = ch->cutoffEnv;
  cutEnv    = s[0] + s[1]*c + s[2]*q + m*(s[3] + s[4]*c + s[5]*q + s[6]*m) + s[7]*c*c + m*m*c*(s[8] + s[9]*c);
  const real_t *p = ch->cutoffAccent;
  cutAccent = (accentCircuit ? one : a)*(p[0] + p[1]*c + p[2]*q + p[3]*m + p[4]*a); // the circuit has a in it
  if( accentCircuit ) // its coefficients at this resonance, the resistances relative to the pot's (and the mixing
  {                   // resistor's): rw from the wiper to the capacitor, g the conductance into the wiper
    const real_t rw = one - q, g1 = one / (ch->accentR1 + q), g = g1 + one, den = one + rw*g;
    sweepOn       = one / (one + rw);      // the diode conducts while s > sweepOn * the capacitor's voltage
    sweepIn       = g1 / g;                // the capacitor charges towards sweepIn * s
    sweepS        = g1*rw / den;           // the wiper = sweepS * s + sweepC * the capacitor's voltage
    sweepC        = one / den;
    sweepCoeffOn  = g / (g + sweepSamples*den);          // one-poles of C den / g and C (rw + 1), 1/(1 + tau) for
    sweepCoeffOff = one / (one + sweepSamples*(rw + one)); // 1 - e^(-1/tau): tau is thousands of samples
  }
  const real_t *r = ch->resonance;
  if( ch->resonanceSkew ) // Open303's: TeeBeeFilter::setResonance()
  {
    resBase = (1.0 - expf(-3.0*q)) * (1.0 / (1.0 - 0.049787068367863944)); // 1 - e^-3
    resEnv  = 0.0;
  }
  else
  {
    resBase = r[0] + q*(r[1] + q*(r[2] + q*r[3])) + c*(r[4] + q*(r[5] + q*r[6])) + c*c*(r[9] + r[10]*q)
              + m*(r[11] + r[12]*q);
    resEnv  = q*(r[7] + r[8]*q);
  }
  const real_t *g = ch->gain;
  gainBase  = g[0] + q*(g[1] + q*g[2]) + c*(g[3] + g[4]*q + g[5]*c) + m*(g[8] + g[9]*q);
  gainEnv   = g[6] + g[7]*q;
  ampAccent = ch->ampAccent[0]*(a + ch->ampAccent[1]*a*a);
}
