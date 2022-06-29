/* ------------------------------------------------------------
name: "rev_dattorro"
Code generated with Faust 2.37.3 (https://faust.grame.fr)
Compilation options: -lang cpp -es 1 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __rev_dattorro_H__
#define  __rev_dattorro_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>


#ifndef FAUSTCLASS 
#define FAUSTCLASS rev_dattorro
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class rev_dattorro : public dsp {
	
 private:
	
	int fSampleRate;
	float fConst0;
	FAUSTFLOAT fHslider0;
	float fConst1;
	float fRec2[2];
	FAUSTFLOAT fHslider1;
	float fRec6[2];
	FAUSTFLOAT fHslider2;
	float fRec9[2];
	int IOTA;
	float fVec0[1024];
	float fRec7[2];
	float fVec1[8192];
	float fRec5[2];
	float fVec2[4096];
	float fRec3[2];
	float fVec3[4096];
	FAUSTFLOAT fHslider3;
	float fRec19[2];
	float fVec4[32];
	float fRec18[3];
	FAUSTFLOAT fHslider4;
	float fRec20[2];
	float fVec5[256];
	float fRec16[2];
	float fVec6[128];
	float fRec14[2];
	float fVec7[512];
	float fRec12[2];
	float fVec8[512];
	float fRec10[2];
	float fRec0[2];
	float fVec9[1024];
	float fRec24[2];
	float fVec10[8192];
	float fRec23[2];
	float fVec11[2048];
	float fRec21[2];
	float fVec12[2048];
	float fRec1[2];
	FAUSTFLOAT fHslider5;
	float fRec26[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.3");
		m->declare("compile_options", "-lang cpp -es 1 -single -ftz 0");
		m->declare("filename", "rev_dattorro.dsp");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.5");
		m->declare("name", "rev_dattorro");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.2");
		m->declare("reverbs.lib/dattorro_rev:author", "Jakob Zerbian");
		m->declare("reverbs.lib/dattorro_rev:license", "MIT-style STK-4.3 license");
		m->declare("reverbs.lib/name", "Faust Reverb Library");
		m->declare("reverbs.lib/version", "0.2");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.2");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.1");
	}

	virtual int getNumInputs() {
		return 2;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = (44.0999985f / std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate))));
		fConst1 = (1.0f - fConst0);
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.5f);
		fHslider1 = FAUSTFLOAT(0.5f);
		fHslider2 = FAUSTFLOAT(0.5f);
		fHslider3 = FAUSTFLOAT(0.5f);
		fHslider4 = FAUSTFLOAT(0.5f);
		fHslider5 = FAUSTFLOAT(0.5f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec2[l0] = 0.0f;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec6[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec9[l2] = 0.0f;
		}
		IOTA = 0;
		for (int l3 = 0; (l3 < 1024); l3 = (l3 + 1)) {
			fVec0[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec7[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 8192); l5 = (l5 + 1)) {
			fVec1[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec5[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 4096); l7 = (l7 + 1)) {
			fVec2[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec3[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 4096); l9 = (l9 + 1)) {
			fVec3[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec19[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 32); l11 = (l11 + 1)) {
			fVec4[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
			fRec18[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec20[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 256); l14 = (l14 + 1)) {
			fVec5[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec16[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 128); l16 = (l16 + 1)) {
			fVec6[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec14[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 512); l18 = (l18 + 1)) {
			fVec7[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec12[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 512); l20 = (l20 + 1)) {
			fVec8[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec10[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec0[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 1024); l23 = (l23 + 1)) {
			fVec9[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec24[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 8192); l25 = (l25 + 1)) {
			fVec10[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec23[l26] = 0.0f;
		}
		for (int l27 = 0; (l27 < 2048); l27 = (l27 + 1)) {
			fVec11[l27] = 0.0f;
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec21[l28] = 0.0f;
		}
		for (int l29 = 0; (l29 < 2048); l29 = (l29 + 1)) {
			fVec12[l29] = 0.0f;
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec1[l30] = 0.0f;
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fRec26[l31] = 0.0f;
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
	
	virtual rev_dattorro* clone() {
		return new rev_dattorro();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("rev_dattorro");
		ui_interface->declare(&fHslider5, "0", "");
		ui_interface->addHorizontalSlider("D/W", &fHslider5, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(9.99999975e-05f));
		ui_interface->declare(&fHslider0, "1", "");
		ui_interface->addHorizontalSlider("Decay", &fHslider0, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(9.99999975e-05f));
		ui_interface->declare(&fHslider1, "2", "");
		ui_interface->addHorizontalSlider("Damp", &fHslider1, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(9.99999975e-05f));
		ui_interface->declare(&fHslider3, "3", "");
		ui_interface->addHorizontalSlider("BW", &fHslider3, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(9.99999975e-05f));
		ui_interface->declare(&fHslider2, "4", "");
		ui_interface->addHorizontalSlider("DDiff", &fHslider2, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.00999999978f), FAUSTFLOAT(0.99000001f), FAUSTFLOAT(9.99999975e-05f));
		ui_interface->declare(&fHslider4, "5", "");
		ui_interface->addHorizontalSlider("IDDiff", &fHslider4, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.00999999978f), FAUSTFLOAT(0.99000001f), FAUSTFLOAT(9.99999975e-05f));
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = (fConst0 * float(fHslider0));
		float fSlow1 = (fConst0 * float(fHslider1));
		float fSlow2 = (fConst0 * float(fHslider2));
		float fSlow3 = (fConst0 * float(fHslider3));
		float fSlow4 = (fConst0 * float(fHslider4));
		float fSlow5 = (fConst0 * float(fHslider5));
		for (int i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			fRec2[0] = (fSlow0 + (fConst1 * fRec2[1]));
			fRec6[0] = (fSlow1 + (fConst1 * fRec6[1]));
			float fTemp0 = (1.0f - fRec6[0]);
			fRec9[0] = (fSlow2 + (fConst1 * fRec9[1]));
			float fTemp1 = ((fRec9[0] * fRec7[1]) + fRec1[1]);
			fVec0[(IOTA & 1023)] = fTemp1;
			fRec7[0] = fVec0[((IOTA - 908) & 1023)];
			float fRec8 = (0.0f - (fRec9[0] * fTemp1));
			fVec1[(IOTA & 8191)] = (fRec8 + fRec7[1]);
			fRec5[0] = ((fRec6[0] * fRec5[1]) + (fTemp0 * fVec1[((IOTA - 4217) & 8191)]));
			float fTemp2 = ((fRec5[0] * fRec2[0]) - (fRec9[0] * fRec3[1]));
			fVec2[(IOTA & 4095)] = fTemp2;
			fRec3[0] = fVec2[((IOTA - 2656) & 4095)];
			float fRec4 = (fRec9[0] * fTemp2);
			fVec3[(IOTA & 4095)] = (fRec4 + fRec3[1]);
			fRec19[0] = (fSlow3 + (fConst1 * fRec19[1]));
			float fTemp3 = float(input0[i0]);
			float fTemp4 = float(input1[i0]);
			fVec4[(IOTA & 31)] = (fTemp3 + fTemp4);
			fRec18[0] = (((1.0f - fRec19[0]) * fRec18[2]) + (0.5f * (fRec19[0] * fVec4[((IOTA - 24) & 31)])));
			fRec20[0] = (fSlow4 + (fConst1 * fRec20[1]));
			float fTemp5 = (fRec18[0] - (fRec20[0] * fRec16[1]));
			fVec5[(IOTA & 255)] = fTemp5;
			fRec16[0] = fVec5[((IOTA - 142) & 255)];
			float fRec17 = (fRec20[0] * fTemp5);
			float fTemp6 = ((fRec17 + fRec16[1]) - (fRec20[0] * fRec14[1]));
			fVec6[(IOTA & 127)] = fTemp6;
			fRec14[0] = fVec6[((IOTA - 107) & 127)];
			float fRec15 = (fRec20[0] * fTemp6);
			float fTemp7 = ((fRec15 + fRec14[1]) - (fRec20[0] * fRec12[1]));
			fVec7[(IOTA & 511)] = fTemp7;
			fRec12[0] = fVec7[((IOTA - 379) & 511)];
			float fRec13 = (fRec20[0] * fTemp7);
			float fTemp8 = ((fRec13 + fRec12[1]) - (fRec20[0] * fRec10[1]));
			fVec8[(IOTA & 511)] = fTemp8;
			fRec10[0] = fVec8[((IOTA - 277) & 511)];
			float fRec11 = (fRec20[0] * fTemp8);
			float fTemp9 = (fRec11 + fRec10[1]);
			fRec0[0] = ((fRec2[0] * fVec3[((IOTA - 2656) & 4095)]) + fTemp9);
			float fTemp10 = ((fRec9[0] * fRec24[1]) + fRec0[1]);
			fVec9[(IOTA & 1023)] = fTemp10;
			fRec24[0] = fVec9[((IOTA - 672) & 1023)];
			float fRec25 = (0.0f - (fRec9[0] * fTemp10));
			fVec10[(IOTA & 8191)] = (fRec25 + fRec24[1]);
			fRec23[0] = ((fRec6[0] * fRec23[1]) + (fTemp0 * fVec10[((IOTA - 4453) & 8191)]));
			float fTemp11 = ((fRec2[0] * fRec23[0]) - (fRec9[0] * fRec21[1]));
			fVec11[(IOTA & 2047)] = fTemp11;
			fRec21[0] = fVec11[((IOTA - 1800) & 2047)];
			float fRec22 = (fRec9[0] * fTemp11);
			fVec12[(IOTA & 2047)] = (fRec22 + fRec21[1]);
			fRec1[0] = (fTemp9 + (fRec2[0] * fVec12[((IOTA - 1800) & 2047)]));
			fRec26[0] = (fSlow5 + (fConst1 * fRec26[1]));
			float fTemp12 = (1.0f - fRec26[0]);
			output0[i0] = FAUSTFLOAT(((fRec0[0] * fRec26[0]) + (fTemp3 * fTemp12)));
			output1[i0] = FAUSTFLOAT(((fTemp4 * fTemp12) + (fRec26[0] * fRec1[0])));
			fRec2[1] = fRec2[0];
			fRec6[1] = fRec6[0];
			fRec9[1] = fRec9[0];
			IOTA = (IOTA + 1);
			fRec7[1] = fRec7[0];
			fRec5[1] = fRec5[0];
			fRec3[1] = fRec3[0];
			fRec19[1] = fRec19[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			fRec20[1] = fRec20[0];
			fRec16[1] = fRec16[0];
			fRec14[1] = fRec14[0];
			fRec12[1] = fRec12[0];
			fRec10[1] = fRec10[0];
			fRec0[1] = fRec0[0];
			fRec24[1] = fRec24[0];
			fRec23[1] = fRec23[0];
			fRec21[1] = fRec21[0];
			fRec1[1] = fRec1[0];
			fRec26[1] = fRec26[0];
		}
	}

};

#endif
