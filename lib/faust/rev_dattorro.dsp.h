/* ------------------------------------------------------------
name: "rev_dattorro"
Code generated with Faust 2.37.3 (https://faust.grame.fr)
Compilation options: -lang c -es 1 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 


#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>
#include <stdlib.h>


#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

typedef struct {
	int fSampleRate;
	float fConst0;
	FAUSTFLOAT fHslider0;
	float fConst1;
	float fRec11[2];
	int IOTA;
	float fVec0[32];
	float fRec10[3];
	FAUSTFLOAT fHslider1;
	float fRec12[2];
	float fVec1[256];
	float fRec8[2];
	float fVec2[128];
	float fRec6[2];
	float fVec3[512];
	float fRec4[2];
	float fVec4[512];
	float fRec2[2];
	FAUSTFLOAT fHslider2;
	float fRec13[2];
	FAUSTFLOAT fHslider3;
	float fRec17[2];
	FAUSTFLOAT fHslider4;
	float fRec20[2];
	float fVec5[1024];
	float fRec18[2];
	float fVec6[8192];
	float fRec16[2];
	float fVec7[4096];
	float fRec14[2];
	float fVec8[4096];
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
} mydsp;

mydsp* newmydsp() { 
	mydsp* dsp = (mydsp*)calloc(1, sizeof(mydsp));
	return dsp;
}

void deletemydsp(mydsp* dsp) { 
	free(dsp);
}

void metadatamydsp(MetaGlue* m) { 
	m->declare(m->metaInterface, "basics.lib/name", "Faust Basic Element Library");
	m->declare(m->metaInterface, "basics.lib/version", "0.2");
	m->declare(m->metaInterface, "compile_options", "-lang c -es 1 -single -ftz 0");
	m->declare(m->metaInterface, "filename", "rev_dattorro.dsp");
	m->declare(m->metaInterface, "maths.lib/author", "GRAME");
	m->declare(m->metaInterface, "maths.lib/copyright", "GRAME");
	m->declare(m->metaInterface, "maths.lib/license", "LGPL with exception");
	m->declare(m->metaInterface, "maths.lib/name", "Faust Math Library");
	m->declare(m->metaInterface, "maths.lib/version", "2.5");
	m->declare(m->metaInterface, "name", "rev_dattorro");
	m->declare(m->metaInterface, "platform.lib/name", "Generic Platform Library");
	m->declare(m->metaInterface, "platform.lib/version", "0.2");
	m->declare(m->metaInterface, "reverbs.lib/dattorro_rev:author", "Jakob Zerbian");
	m->declare(m->metaInterface, "reverbs.lib/dattorro_rev:license", "MIT-style STK-4.3 license");
	m->declare(m->metaInterface, "reverbs.lib/name", "Faust Reverb Library");
	m->declare(m->metaInterface, "reverbs.lib/version", "0.2");
	m->declare(m->metaInterface, "routes.lib/name", "Faust Signal Routing Library");
	m->declare(m->metaInterface, "routes.lib/version", "0.2");
	m->declare(m->metaInterface, "signals.lib/name", "Faust Signal Routing Library");
	m->declare(m->metaInterface, "signals.lib/version", "0.1");
}

int getSampleRatemydsp(mydsp* dsp) {
	return dsp->fSampleRate;
}

int getNumInputsmydsp(mydsp* dsp) {
	return 2;
}
int getNumOutputsmydsp(mydsp* dsp) {
	return 2;
}

void classInitmydsp(int sample_rate) {
}

void instanceResetUserInterfacemydsp(mydsp* dsp) {
	dsp->fHslider0 = (FAUSTFLOAT)0.5f;
	dsp->fHslider1 = (FAUSTFLOAT)0.5f;
	dsp->fHslider2 = (FAUSTFLOAT)0.5f;
	dsp->fHslider3 = (FAUSTFLOAT)0.5f;
	dsp->fHslider4 = (FAUSTFLOAT)0.5f;
	dsp->fHslider5 = (FAUSTFLOAT)0.5f;
}

void instanceClearmydsp(mydsp* dsp) {
	/* C99 loop */
	{
		int l0;
		for (l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			dsp->fRec11[l0] = 0.0f;
		}
	}
	dsp->IOTA = 0;
	/* C99 loop */
	{
		int l1;
		for (l1 = 0; (l1 < 32); l1 = (l1 + 1)) {
			dsp->fVec0[l1] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l2;
		for (l2 = 0; (l2 < 3); l2 = (l2 + 1)) {
			dsp->fRec10[l2] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l3;
		for (l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			dsp->fRec12[l3] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l4;
		for (l4 = 0; (l4 < 256); l4 = (l4 + 1)) {
			dsp->fVec1[l4] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l5;
		for (l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			dsp->fRec8[l5] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l6;
		for (l6 = 0; (l6 < 128); l6 = (l6 + 1)) {
			dsp->fVec2[l6] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l7;
		for (l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			dsp->fRec6[l7] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l8;
		for (l8 = 0; (l8 < 512); l8 = (l8 + 1)) {
			dsp->fVec3[l8] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l9;
		for (l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			dsp->fRec4[l9] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l10;
		for (l10 = 0; (l10 < 512); l10 = (l10 + 1)) {
			dsp->fVec4[l10] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l11;
		for (l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			dsp->fRec2[l11] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l12;
		for (l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			dsp->fRec13[l12] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l13;
		for (l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			dsp->fRec17[l13] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l14;
		for (l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			dsp->fRec20[l14] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l15;
		for (l15 = 0; (l15 < 1024); l15 = (l15 + 1)) {
			dsp->fVec5[l15] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l16;
		for (l16 = 0; (l16 < 2); l16 = (l16 + 1)) {
			dsp->fRec18[l16] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l17;
		for (l17 = 0; (l17 < 8192); l17 = (l17 + 1)) {
			dsp->fVec6[l17] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l18;
		for (l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			dsp->fRec16[l18] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l19;
		for (l19 = 0; (l19 < 4096); l19 = (l19 + 1)) {
			dsp->fVec7[l19] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l20;
		for (l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			dsp->fRec14[l20] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l21;
		for (l21 = 0; (l21 < 4096); l21 = (l21 + 1)) {
			dsp->fVec8[l21] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l22;
		for (l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			dsp->fRec0[l22] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l23;
		for (l23 = 0; (l23 < 1024); l23 = (l23 + 1)) {
			dsp->fVec9[l23] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l24;
		for (l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			dsp->fRec24[l24] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l25;
		for (l25 = 0; (l25 < 8192); l25 = (l25 + 1)) {
			dsp->fVec10[l25] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l26;
		for (l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			dsp->fRec23[l26] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l27;
		for (l27 = 0; (l27 < 2048); l27 = (l27 + 1)) {
			dsp->fVec11[l27] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l28;
		for (l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			dsp->fRec21[l28] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l29;
		for (l29 = 0; (l29 < 2048); l29 = (l29 + 1)) {
			dsp->fVec12[l29] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l30;
		for (l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			dsp->fRec1[l30] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l31;
		for (l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			dsp->fRec26[l31] = 0.0f;
		}
	}
}

void instanceConstantsmydsp(mydsp* dsp, int sample_rate) {
	dsp->fSampleRate = sample_rate;
	dsp->fConst0 = (44.0999985f / fminf(192000.0f, fmaxf(1.0f, (float)dsp->fSampleRate)));
	dsp->fConst1 = (1.0f - dsp->fConst0);
}

void instanceInitmydsp(mydsp* dsp, int sample_rate) {
	instanceConstantsmydsp(dsp, sample_rate);
	instanceResetUserInterfacemydsp(dsp);
	instanceClearmydsp(dsp);
}

void initmydsp(mydsp* dsp, int sample_rate) {
	classInitmydsp(sample_rate);
	instanceInitmydsp(dsp, sample_rate);
}

void buildUserInterfacemydsp(mydsp* dsp, UIGlue* ui_interface) {
	ui_interface->openVerticalBox(ui_interface->uiInterface, "rev_dattorro");
	ui_interface->declare(ui_interface->uiInterface, &dsp->fHslider5, "0", "");
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "D/W", &dsp->fHslider5, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.0f, (FAUSTFLOAT)1.0f, (FAUSTFLOAT)9.99999975e-05f);
	ui_interface->declare(ui_interface->uiInterface, &dsp->fHslider2, "1", "");
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "Decay", &dsp->fHslider2, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.0f, (FAUSTFLOAT)1.0f, (FAUSTFLOAT)9.99999975e-05f);
	ui_interface->declare(ui_interface->uiInterface, &dsp->fHslider3, "2", "");
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "Damp", &dsp->fHslider3, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.0f, (FAUSTFLOAT)1.0f, (FAUSTFLOAT)9.99999975e-05f);
	ui_interface->declare(ui_interface->uiInterface, &dsp->fHslider0, "3", "");
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "BW", &dsp->fHslider0, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.0f, (FAUSTFLOAT)1.0f, (FAUSTFLOAT)9.99999975e-05f);
	ui_interface->declare(ui_interface->uiInterface, &dsp->fHslider4, "4", "");
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "DDiff", &dsp->fHslider4, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.00999999978f, (FAUSTFLOAT)0.99000001f, (FAUSTFLOAT)9.99999975e-05f);
	ui_interface->declare(ui_interface->uiInterface, &dsp->fHslider1, "5", "");
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "IDiff", &dsp->fHslider1, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.00999999978f, (FAUSTFLOAT)0.99000001f, (FAUSTFLOAT)9.99999975e-05f);
	ui_interface->closeBox(ui_interface->uiInterface);
}

void computemydsp(mydsp* dsp, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
	FAUSTFLOAT* input0 = inputs[0];
	FAUSTFLOAT* input1 = inputs[1];
	FAUSTFLOAT* output0 = outputs[0];
	FAUSTFLOAT* output1 = outputs[1];
	float fSlow0 = (dsp->fConst0 * (float)dsp->fHslider0);
	float fSlow1 = (dsp->fConst0 * (float)dsp->fHslider1);
	float fSlow2 = (dsp->fConst0 * (float)dsp->fHslider2);
	float fSlow3 = (dsp->fConst0 * (float)dsp->fHslider3);
	float fSlow4 = (dsp->fConst0 * (float)dsp->fHslider4);
	float fSlow5 = (dsp->fConst0 * (float)dsp->fHslider5);
	/* C99 loop */
	{
		int i0;
		for (i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			dsp->fRec11[0] = (fSlow0 + (dsp->fConst1 * dsp->fRec11[1]));
			float fTemp0 = (float)input0[i0];
			float fTemp1 = (float)input1[i0];
			dsp->fVec0[(dsp->IOTA & 31)] = (fTemp0 + fTemp1);
			dsp->fRec10[0] = (((1.0f - dsp->fRec11[0]) * dsp->fRec10[2]) + (0.5f * (dsp->fRec11[0] * dsp->fVec0[((dsp->IOTA - 24) & 31)])));
			dsp->fRec12[0] = (fSlow1 + (dsp->fConst1 * dsp->fRec12[1]));
			float fTemp2 = (dsp->fRec10[0] - (dsp->fRec12[0] * dsp->fRec8[1]));
			dsp->fVec1[(dsp->IOTA & 255)] = fTemp2;
			dsp->fRec8[0] = dsp->fVec1[((dsp->IOTA - 142) & 255)];
			float fRec9 = (dsp->fRec12[0] * fTemp2);
			float fTemp3 = ((fRec9 + dsp->fRec8[1]) - (dsp->fRec12[0] * dsp->fRec6[1]));
			dsp->fVec2[(dsp->IOTA & 127)] = fTemp3;
			dsp->fRec6[0] = dsp->fVec2[((dsp->IOTA - 107) & 127)];
			float fRec7 = (dsp->fRec12[0] * fTemp3);
			float fTemp4 = ((fRec7 + dsp->fRec6[1]) - (dsp->fRec12[0] * dsp->fRec4[1]));
			dsp->fVec3[(dsp->IOTA & 511)] = fTemp4;
			dsp->fRec4[0] = dsp->fVec3[((dsp->IOTA - 379) & 511)];
			float fRec5 = (dsp->fRec12[0] * fTemp4);
			float fTemp5 = ((fRec5 + dsp->fRec4[1]) - (dsp->fRec12[0] * dsp->fRec2[1]));
			dsp->fVec4[(dsp->IOTA & 511)] = fTemp5;
			dsp->fRec2[0] = dsp->fVec4[((dsp->IOTA - 277) & 511)];
			float fRec3 = (dsp->fRec12[0] * fTemp5);
			dsp->fRec13[0] = (fSlow2 + (dsp->fConst1 * dsp->fRec13[1]));
			dsp->fRec17[0] = (fSlow3 + (dsp->fConst1 * dsp->fRec17[1]));
			float fTemp6 = (1.0f - dsp->fRec17[0]);
			dsp->fRec20[0] = (fSlow4 + (dsp->fConst1 * dsp->fRec20[1]));
			float fTemp7 = ((dsp->fRec20[0] * dsp->fRec18[1]) + dsp->fRec1[1]);
			dsp->fVec5[(dsp->IOTA & 1023)] = fTemp7;
			dsp->fRec18[0] = dsp->fVec5[((dsp->IOTA - 908) & 1023)];
			float fRec19 = (0.0f - (dsp->fRec20[0] * fTemp7));
			dsp->fVec6[(dsp->IOTA & 8191)] = (fRec19 + dsp->fRec18[1]);
			dsp->fRec16[0] = ((dsp->fRec17[0] * dsp->fRec16[1]) + (fTemp6 * dsp->fVec6[((dsp->IOTA - 4217) & 8191)]));
			float fTemp8 = ((dsp->fRec16[0] * dsp->fRec13[0]) - (dsp->fRec20[0] * dsp->fRec14[1]));
			dsp->fVec7[(dsp->IOTA & 4095)] = fTemp8;
			dsp->fRec14[0] = dsp->fVec7[((dsp->IOTA - 2656) & 4095)];
			float fRec15 = (dsp->fRec20[0] * fTemp8);
			dsp->fVec8[(dsp->IOTA & 4095)] = (fRec15 + dsp->fRec14[1]);
			dsp->fRec0[0] = (dsp->fRec2[1] + (fRec3 + (dsp->fRec13[0] * dsp->fVec8[((dsp->IOTA - 2656) & 4095)])));
			float fTemp9 = ((dsp->fRec20[0] * dsp->fRec24[1]) + dsp->fRec0[1]);
			dsp->fVec9[(dsp->IOTA & 1023)] = fTemp9;
			dsp->fRec24[0] = dsp->fVec9[((dsp->IOTA - 672) & 1023)];
			float fRec25 = (0.0f - (dsp->fRec20[0] * fTemp9));
			dsp->fVec10[(dsp->IOTA & 8191)] = (fRec25 + dsp->fRec24[1]);
			dsp->fRec23[0] = ((dsp->fRec17[0] * dsp->fRec23[1]) + (fTemp6 * dsp->fVec10[((dsp->IOTA - 4453) & 8191)]));
			float fTemp10 = ((dsp->fRec13[0] * dsp->fRec23[0]) - (dsp->fRec20[0] * dsp->fRec21[1]));
			dsp->fVec11[(dsp->IOTA & 2047)] = fTemp10;
			dsp->fRec21[0] = dsp->fVec11[((dsp->IOTA - 1800) & 2047)];
			float fRec22 = (dsp->fRec20[0] * fTemp10);
			dsp->fVec12[(dsp->IOTA & 2047)] = (fRec22 + dsp->fRec21[1]);
			dsp->fRec1[0] = ((fRec3 + dsp->fRec2[1]) + (dsp->fRec13[0] * dsp->fVec12[((dsp->IOTA - 1800) & 2047)]));
			dsp->fRec26[0] = (fSlow5 + (dsp->fConst1 * dsp->fRec26[1]));
			float fTemp11 = (1.0f - dsp->fRec26[0]);
			output0[i0] = (FAUSTFLOAT)((dsp->fRec0[0] * dsp->fRec26[0]) + (fTemp0 * fTemp11));
			output1[i0] = (FAUSTFLOAT)((fTemp1 * fTemp11) + (dsp->fRec26[0] * dsp->fRec1[0]));
			dsp->fRec11[1] = dsp->fRec11[0];
			dsp->IOTA = (dsp->IOTA + 1);
			dsp->fRec10[2] = dsp->fRec10[1];
			dsp->fRec10[1] = dsp->fRec10[0];
			dsp->fRec12[1] = dsp->fRec12[0];
			dsp->fRec8[1] = dsp->fRec8[0];
			dsp->fRec6[1] = dsp->fRec6[0];
			dsp->fRec4[1] = dsp->fRec4[0];
			dsp->fRec2[1] = dsp->fRec2[0];
			dsp->fRec13[1] = dsp->fRec13[0];
			dsp->fRec17[1] = dsp->fRec17[0];
			dsp->fRec20[1] = dsp->fRec20[0];
			dsp->fRec18[1] = dsp->fRec18[0];
			dsp->fRec16[1] = dsp->fRec16[0];
			dsp->fRec14[1] = dsp->fRec14[0];
			dsp->fRec0[1] = dsp->fRec0[0];
			dsp->fRec24[1] = dsp->fRec24[0];
			dsp->fRec23[1] = dsp->fRec23[0];
			dsp->fRec21[1] = dsp->fRec21[0];
			dsp->fRec1[1] = dsp->fRec1[0];
			dsp->fRec26[1] = dsp->fRec26[0];
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif
