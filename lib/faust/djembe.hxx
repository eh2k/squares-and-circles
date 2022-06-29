/* ------------------------------------------------------------
name: "djembe"
Code generated with Faust 2.37.3 (https://faust.grame.fr)
Compilation options: -lang cpp -es 1 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __djembe_H__
#define  __djembe_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

static float djembe_faustpower2_f(float value) {
	return (value * value);
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS djembe
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class djembe : public dsp {
	
 private:
	
	int iRec3[2];
	int fSampleRate;
	float fConst1;
	float fConst2;
	FAUSTFLOAT fHslider0;
	float fConst3;
	float fRec4[2];
	float fRec2[3];
	float fRec1[3];
	FAUSTFLOAT fButton0;
	float fVec0[2];
	int iRec5[2];
	float fConst4;
	FAUSTFLOAT fHslider1;
	float fRec6[2];
	float fConst6;
	float fConst7;
	FAUSTFLOAT fHslider2;
	float fRec7[2];
	float fConst8;
	float fRec0[3];
	float fConst10;
	float fConst11;
	float fRec8[3];
	float fConst13;
	float fConst14;
	float fRec9[3];
	float fConst16;
	float fConst17;
	float fRec10[3];
	float fConst19;
	float fConst20;
	float fRec11[3];
	float fConst22;
	float fConst23;
	float fRec12[3];
	float fConst25;
	float fConst26;
	float fRec13[3];
	float fConst28;
	float fConst29;
	float fRec14[3];
	float fConst31;
	float fConst32;
	float fRec15[3];
	float fConst34;
	float fConst35;
	float fRec16[3];
	float fConst37;
	float fConst38;
	float fRec17[3];
	float fConst40;
	float fConst41;
	float fRec18[3];
	float fConst43;
	float fConst44;
	float fRec19[3];
	float fConst46;
	float fConst47;
	float fRec20[3];
	float fConst49;
	float fConst50;
	float fRec21[3];
	float fConst52;
	float fConst53;
	float fRec22[3];
	float fConst55;
	float fConst56;
	float fRec23[3];
	float fConst58;
	float fConst59;
	float fRec24[3];
	float fConst61;
	float fConst62;
	float fRec25[3];
	float fConst64;
	float fConst65;
	float fRec26[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("compile_options", "-lang cpp -es 1 -single -ftz 0");
		m->declare("envelopes.lib/ar:author", "Yann Orlarey, Stéphane Letz");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "0.1");
		m->declare("filename", "djembe.dsp");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/highpass:author", "Julius O. Smith III");
		m->declare("filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "0.3");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.5");
		m->declare("name", "djembe");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.2");
		m->declare("physmodels.lib/name", "Faust Physical Models Library");
		m->declare("physmodels.lib/version", "0.1");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.2");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.1");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 1;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = (3.14159274f / fConst0);
		fConst2 = (44.0999985f / fConst0);
		fConst3 = (1.0f - fConst2);
		fConst4 = (0.00200000009f * fConst0);
		float fConst5 = std::pow(0.00100000005f, (5.55555534f / fConst0));
		fConst6 = (0.0f - (2.0f * fConst5));
		fConst7 = (6.28318548f / fConst0);
		fConst8 = djembe_faustpower2_f(fConst5);
		float fConst9 = std::pow(0.00100000005f, (2.77777767f / fConst0));
		fConst10 = (0.0f - (2.0f * fConst9));
		fConst11 = djembe_faustpower2_f(fConst9);
		float fConst12 = std::pow(0.00100000005f, (1.66666663f / fConst0));
		fConst13 = (0.0f - (2.0f * fConst12));
		fConst14 = djembe_faustpower2_f(fConst12);
		float fConst15 = std::pow(0.00100000005f, (1.75438595f / fConst0));
		fConst16 = (0.0f - (2.0f * fConst15));
		fConst17 = djembe_faustpower2_f(fConst15);
		float fConst18 = std::pow(0.00100000005f, (1.85185182f / fConst0));
		fConst19 = (0.0f - (2.0f * fConst18));
		fConst20 = djembe_faustpower2_f(fConst18);
		float fConst21 = std::pow(0.00100000005f, (1.96078432f / fConst0));
		fConst22 = (0.0f - (2.0f * fConst21));
		fConst23 = djembe_faustpower2_f(fConst21);
		float fConst24 = std::pow(0.00100000005f, (2.08333325f / fConst0));
		fConst25 = (0.0f - (2.0f * fConst24));
		fConst26 = djembe_faustpower2_f(fConst24);
		float fConst27 = std::pow(0.00100000005f, (2.22222233f / fConst0));
		fConst28 = (0.0f - (2.0f * fConst27));
		fConst29 = djembe_faustpower2_f(fConst27);
		float fConst30 = std::pow(0.00100000005f, (2.38095236f / fConst0));
		fConst31 = (0.0f - (2.0f * fConst30));
		fConst32 = djembe_faustpower2_f(fConst30);
		float fConst33 = std::pow(0.00100000005f, (2.56410265f / fConst0));
		fConst34 = (0.0f - (2.0f * fConst33));
		fConst35 = djembe_faustpower2_f(fConst33);
		float fConst36 = std::pow(0.00100000005f, (3.030303f / fConst0));
		fConst37 = (0.0f - (2.0f * fConst36));
		fConst38 = djembe_faustpower2_f(fConst36);
		float fConst39 = std::pow(0.00100000005f, (3.33333325f / fConst0));
		fConst40 = (0.0f - (2.0f * fConst39));
		fConst41 = djembe_faustpower2_f(fConst39);
		float fConst42 = std::pow(0.00100000005f, (3.70370364f / fConst0));
		fConst43 = (0.0f - (2.0f * fConst42));
		fConst44 = djembe_faustpower2_f(fConst42);
		float fConst45 = std::pow(0.00100000005f, (4.16666651f / fConst0));
		fConst46 = (0.0f - (2.0f * fConst45));
		fConst47 = djembe_faustpower2_f(fConst45);
		float fConst48 = std::pow(0.00100000005f, (4.76190472f / fConst0));
		fConst49 = (0.0f - (2.0f * fConst48));
		fConst50 = djembe_faustpower2_f(fConst48);
		float fConst51 = std::pow(0.00100000005f, (6.66666651f / fConst0));
		fConst52 = (0.0f - (2.0f * fConst51));
		fConst53 = djembe_faustpower2_f(fConst51);
		float fConst54 = std::pow(0.00100000005f, (8.33333302f / fConst0));
		fConst55 = (0.0f - (2.0f * fConst54));
		fConst56 = djembe_faustpower2_f(fConst54);
		float fConst57 = std::pow(0.00100000005f, (11.1111107f / fConst0));
		fConst58 = (0.0f - (2.0f * fConst57));
		fConst59 = djembe_faustpower2_f(fConst57);
		float fConst60 = std::pow(0.00100000005f, (16.666666f / fConst0));
		fConst61 = (0.0f - (2.0f * fConst60));
		fConst62 = djembe_faustpower2_f(fConst60);
		float fConst63 = std::pow(0.00100000005f, (33.3333321f / fConst0));
		fConst64 = (0.0f - (2.0f * fConst63));
		fConst65 = djembe_faustpower2_f(fConst63);
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.5f);
		fButton0 = FAUSTFLOAT(0.0f);
		fHslider1 = FAUSTFLOAT(0.5f);
		fHslider2 = FAUSTFLOAT(0.5f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iRec3[l0] = 0;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec4[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 3); l2 = (l2 + 1)) {
			fRec2[l2] = 0.0f;
		}
		for (int l3 = 0; (l3 < 3); l3 = (l3 + 1)) {
			fRec1[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fVec0[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			iRec5[l5] = 0;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec6[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec7[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			fRec0[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 3); l9 = (l9 + 1)) {
			fRec8[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 3); l10 = (l10 + 1)) {
			fRec9[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 3); l11 = (l11 + 1)) {
			fRec10[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
			fRec11[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
			fRec12[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 3); l14 = (l14 + 1)) {
			fRec13[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 3); l15 = (l15 + 1)) {
			fRec14[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 3); l16 = (l16 + 1)) {
			fRec15[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 3); l17 = (l17 + 1)) {
			fRec16[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 3); l18 = (l18 + 1)) {
			fRec17[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 3); l19 = (l19 + 1)) {
			fRec18[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 3); l20 = (l20 + 1)) {
			fRec19[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 3); l21 = (l21 + 1)) {
			fRec20[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 3); l22 = (l22 + 1)) {
			fRec21[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 3); l23 = (l23 + 1)) {
			fRec22[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 3); l24 = (l24 + 1)) {
			fRec23[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 3); l25 = (l25 + 1)) {
			fRec24[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 3); l26 = (l26 + 1)) {
			fRec25[l26] = 0.0f;
		}
		for (int l27 = 0; (l27 < 3); l27 = (l27 + 1)) {
			fRec26[l27] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual djembe* clone() {
		return new djembe();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("djembe");
		ui_interface->addHorizontalSlider("Freq", &fHslider2, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1000.0f), FAUSTFLOAT(0.00999999978f));
		ui_interface->addHorizontalSlider("Pos", &fHslider0, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(9.99999975e-06f));
		ui_interface->addHorizontalSlider("Sharpness", &fHslider1, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(5.0f), FAUSTFLOAT(9.99999975e-06f));
		ui_interface->addButton("trigger", &fButton0);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = (fConst2 * float(fHslider0));
		float fSlow1 = float(fButton0);
		float fSlow2 = (fConst2 * float(fHslider1));
		float fSlow3 = (fConst2 * float(fHslider2));
		for (int i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			iRec3[0] = (((1103515245 * iRec3[1]) & 2147483647) + 12345);
			fRec4[0] = (fSlow0 + (fConst3 * fRec4[1]));
			float fTemp0 = std::tan((fConst1 * ((500.0f * fRec4[0]) + 40.0f)));
			float fTemp1 = (1.0f / fTemp0);
			float fTemp2 = djembe_faustpower2_f(fTemp0);
			float fTemp3 = (((fTemp1 + 1.41421354f) / fTemp0) + 1.0f);
			fRec2[0] = ((4.65661287e-10f * float(iRec3[0])) - (((fRec2[2] * (((fTemp1 + -1.41421354f) / fTemp0) + 1.0f)) + (2.0f * (fRec2[1] * (1.0f - (1.0f / fTemp2))))) / fTemp3));
			float fTemp4 = std::tan((fConst1 * ((15000.0f * fRec4[0]) + 500.0f)));
			float fTemp5 = (1.0f / fTemp4);
			float fTemp6 = (((fTemp5 + 1.41421354f) / fTemp4) + 1.0f);
			fRec1[0] = (((((fRec2[1] * (0.0f - (2.0f / fTemp2))) + (fRec2[0] / fTemp2)) + (fRec2[2] / fTemp2)) / fTemp3) - (((fRec1[2] * (((fTemp5 + -1.41421354f) / fTemp4) + 1.0f)) + (2.0f * (fRec1[1] * (1.0f - (1.0f / djembe_faustpower2_f(fTemp4)))))) / fTemp6));
			fVec0[0] = fSlow1;
			iRec5[0] = (((iRec5[1] + (iRec5[1] > 0)) * (fSlow1 <= fVec0[1])) + (fSlow1 > fVec0[1]));
			fRec6[0] = (fSlow2 + (fConst3 * fRec6[1]));
			float fTemp7 = (float(iRec5[0]) / std::max<float>(1.0f, (fConst4 * fRec6[0])));
			float fTemp8 = (3.0f * (((fRec1[2] + (fRec1[0] + (2.0f * fRec1[1]))) * std::max<float>(0.0f, std::min<float>(fTemp7, (2.0f - fTemp7)))) / fTemp6));
			fRec7[0] = (fSlow3 + (fConst3 * fRec7[1]));
			fRec0[0] = (fTemp8 - ((fConst6 * (fRec0[1] * std::cos((fConst7 * (fRec7[0] + 2800.0f))))) + (fConst8 * fRec0[2])));
			fRec8[0] = (fTemp8 - ((fConst10 * (fRec8[1] * std::cos((fConst7 * (fRec7[0] + 1600.0f))))) + (fConst11 * fRec8[2])));
			fRec9[0] = (fTemp8 - ((fConst13 * (fRec9[1] * std::cos((fConst7 * fRec7[0])))) + (fConst14 * fRec9[2])));
			fRec10[0] = (fTemp8 - ((fConst16 * (fRec10[1] * std::cos((fConst7 * (fRec7[0] + 200.0f))))) + (fConst17 * fRec10[2])));
			fRec11[0] = (fTemp8 - ((fConst19 * (fRec11[1] * std::cos((fConst7 * (fRec7[0] + 400.0f))))) + (fConst20 * fRec11[2])));
			fRec12[0] = (fTemp8 - ((fConst22 * (fRec12[1] * std::cos((fConst7 * (fRec7[0] + 600.0f))))) + (fConst23 * fRec12[2])));
			fRec13[0] = (fTemp8 - ((fConst25 * (fRec13[1] * std::cos((fConst7 * (fRec7[0] + 800.0f))))) + (fConst26 * fRec13[2])));
			fRec14[0] = (fTemp8 - ((fConst28 * (fRec14[1] * std::cos((fConst7 * (fRec7[0] + 1000.0f))))) + (fConst29 * fRec14[2])));
			fRec15[0] = (fTemp8 - ((fConst31 * (fRec15[1] * std::cos((fConst7 * (fRec7[0] + 1200.0f))))) + (fConst32 * fRec15[2])));
			fRec16[0] = (fTemp8 - ((fConst34 * (fRec16[1] * std::cos((fConst7 * (fRec7[0] + 1400.0f))))) + (fConst35 * fRec16[2])));
			fRec17[0] = (fTemp8 - ((fConst37 * (fRec17[1] * std::cos((fConst7 * (fRec7[0] + 1800.0f))))) + (fConst38 * fRec17[2])));
			fRec18[0] = (fTemp8 - ((fConst40 * (fRec18[1] * std::cos((fConst7 * (fRec7[0] + 2000.0f))))) + (fConst41 * fRec18[2])));
			fRec19[0] = (fTemp8 - ((fConst43 * (fRec19[1] * std::cos((fConst7 * (fRec7[0] + 2200.0f))))) + (fConst44 * fRec19[2])));
			fRec20[0] = (fTemp8 - ((fConst46 * (fRec20[1] * std::cos((fConst7 * (fRec7[0] + 2400.0f))))) + (fConst47 * fRec20[2])));
			fRec21[0] = (fTemp8 - ((fConst49 * (fRec21[1] * std::cos((fConst7 * (fRec7[0] + 2600.0f))))) + (fConst50 * fRec21[2])));
			fRec22[0] = (fTemp8 - ((fConst52 * (fRec22[1] * std::cos((fConst7 * (fRec7[0] + 3000.0f))))) + (fConst53 * fRec22[2])));
			fRec23[0] = (fTemp8 - ((fConst55 * (fRec23[1] * std::cos((fConst7 * (fRec7[0] + 3200.0f))))) + (fConst56 * fRec23[2])));
			fRec24[0] = (fTemp8 - ((fConst58 * (fRec24[1] * std::cos((fConst7 * (fRec7[0] + 3400.0f))))) + (fConst59 * fRec24[2])));
			fRec25[0] = (fTemp8 - ((fConst61 * (fRec25[1] * std::cos((fConst7 * (fRec7[0] + 3600.0f))))) + (fConst62 * fRec25[2])));
			fRec26[0] = (fTemp8 - ((fConst64 * (fRec26[1] * std::cos((fConst7 * (fRec7[0] + 3800.0f))))) + (fConst65 * fRec26[2])));
			output0[i0] = FAUSTFLOAT((0.0500000007f * (((((((0.00444444455f * (fRec0[0] - fRec0[2])) + (((((((0.0123456791f * (fRec8[0] - fRec8[2])) + ((((((((fRec9[0] + (0.25f * (fRec10[0] - fRec10[2]))) + (0.111111112f * (fRec11[0] - fRec11[2]))) + (0.0625f * (fRec12[0] - fRec12[2]))) + (0.0399999991f * (fRec13[0] - fRec13[2]))) + (0.027777778f * (fRec14[0] - fRec14[2]))) + (0.0204081628f * (fRec15[0] - fRec15[2]))) + (0.015625f * (fRec16[0] - fRec16[2]))) + (0.00999999978f * (fRec17[0] - fRec17[2])))) + (0.00826446246f * (fRec18[0] - fRec18[2]))) + (0.0069444445f * (fRec19[0] - fRec19[2]))) + (0.00591715984f * (fRec20[0] - fRec20[2]))) + (0.00510204071f * (fRec21[0] - fRec21[2]))) + (0.00390625f * (fRec22[0] - fRec22[2])))) + (0.00346020772f * (fRec23[0] - fRec23[2]))) + (0.00308641978f * (fRec24[0] - fRec24[2]))) + (0.00277008303f * (fRec25[0] - fRec25[2]))) + (0.00249999994f * (fRec26[0] - fRec26[2]))) - fRec9[2])));
			iRec3[1] = iRec3[0];
			fRec4[1] = fRec4[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fVec0[1] = fVec0[0];
			iRec5[1] = iRec5[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec12[2] = fRec12[1];
			fRec12[1] = fRec12[0];
			fRec13[2] = fRec13[1];
			fRec13[1] = fRec13[0];
			fRec14[2] = fRec14[1];
			fRec14[1] = fRec14[0];
			fRec15[2] = fRec15[1];
			fRec15[1] = fRec15[0];
			fRec16[2] = fRec16[1];
			fRec16[1] = fRec16[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec22[2] = fRec22[1];
			fRec22[1] = fRec22[0];
			fRec23[2] = fRec23[1];
			fRec23[1] = fRec23[0];
			fRec24[2] = fRec24[1];
			fRec24[1] = fRec24[0];
			fRec25[2] = fRec25[1];
			fRec25[1] = fRec25[0];
			fRec26[2] = fRec26[1];
			fRec26[1] = fRec26[0];
		}
	}

};

#endif
