/* ------------------------------------------------------------
name: "djembe"
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

static float mydsp_faustpower2_f(float value) {
	return (value * value);
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

typedef struct {
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
} mydsp;

mydsp* newmydsp() { 
	mydsp* dsp = (mydsp*)calloc(1, sizeof(mydsp));
	return dsp;
}

void deletemydsp(mydsp* dsp) { 
	free(dsp);
}

void metadatamydsp(MetaGlue* m) { 
	m->declare(m->metaInterface, "compile_options", "-lang c -es 1 -single -ftz 0");
	m->declare(m->metaInterface, "envelopes.lib/ar:author", "Yann Orlarey, Stéphane Letz");
	m->declare(m->metaInterface, "envelopes.lib/author", "GRAME");
	m->declare(m->metaInterface, "envelopes.lib/copyright", "GRAME");
	m->declare(m->metaInterface, "envelopes.lib/license", "LGPL with exception");
	m->declare(m->metaInterface, "envelopes.lib/name", "Faust Envelope Library");
	m->declare(m->metaInterface, "envelopes.lib/version", "0.1");
	m->declare(m->metaInterface, "filename", "djembe.dsp");
	m->declare(m->metaInterface, "filters.lib/fir:author", "Julius O. Smith III");
	m->declare(m->metaInterface, "filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
	m->declare(m->metaInterface, "filters.lib/fir:license", "MIT-style STK-4.3 license");
	m->declare(m->metaInterface, "filters.lib/highpass:author", "Julius O. Smith III");
	m->declare(m->metaInterface, "filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
	m->declare(m->metaInterface, "filters.lib/iir:author", "Julius O. Smith III");
	m->declare(m->metaInterface, "filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
	m->declare(m->metaInterface, "filters.lib/iir:license", "MIT-style STK-4.3 license");
	m->declare(m->metaInterface, "filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
	m->declare(m->metaInterface, "filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
	m->declare(m->metaInterface, "filters.lib/lowpass:author", "Julius O. Smith III");
	m->declare(m->metaInterface, "filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
	m->declare(m->metaInterface, "filters.lib/lowpass:license", "MIT-style STK-4.3 license");
	m->declare(m->metaInterface, "filters.lib/name", "Faust Filters Library");
	m->declare(m->metaInterface, "filters.lib/tf2:author", "Julius O. Smith III");
	m->declare(m->metaInterface, "filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
	m->declare(m->metaInterface, "filters.lib/tf2:license", "MIT-style STK-4.3 license");
	m->declare(m->metaInterface, "filters.lib/tf2s:author", "Julius O. Smith III");
	m->declare(m->metaInterface, "filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
	m->declare(m->metaInterface, "filters.lib/tf2s:license", "MIT-style STK-4.3 license");
	m->declare(m->metaInterface, "filters.lib/version", "0.3");
	m->declare(m->metaInterface, "maths.lib/author", "GRAME");
	m->declare(m->metaInterface, "maths.lib/copyright", "GRAME");
	m->declare(m->metaInterface, "maths.lib/license", "LGPL with exception");
	m->declare(m->metaInterface, "maths.lib/name", "Faust Math Library");
	m->declare(m->metaInterface, "maths.lib/version", "2.5");
	m->declare(m->metaInterface, "name", "djembe");
	m->declare(m->metaInterface, "noises.lib/name", "Faust Noise Generator Library");
	m->declare(m->metaInterface, "noises.lib/version", "0.1");
	m->declare(m->metaInterface, "physmodels.lib/name", "Faust Physical Models Library");
	m->declare(m->metaInterface, "physmodels.lib/version", "0.1");
	m->declare(m->metaInterface, "platform.lib/name", "Generic Platform Library");
	m->declare(m->metaInterface, "platform.lib/version", "0.2");
	m->declare(m->metaInterface, "signals.lib/name", "Faust Signal Routing Library");
	m->declare(m->metaInterface, "signals.lib/version", "0.1");
}

int getSampleRatemydsp(mydsp* dsp) {
	return dsp->fSampleRate;
}

int getNumInputsmydsp(mydsp* dsp) {
	return 0;
}
int getNumOutputsmydsp(mydsp* dsp) {
	return 1;
}

void classInitmydsp(int sample_rate) {
}

void instanceResetUserInterfacemydsp(mydsp* dsp) {
	dsp->fHslider0 = (FAUSTFLOAT)0.5f;
	dsp->fButton0 = (FAUSTFLOAT)0.0f;
	dsp->fHslider1 = (FAUSTFLOAT)0.5f;
	dsp->fHslider2 = (FAUSTFLOAT)0.5f;
}

void instanceClearmydsp(mydsp* dsp) {
	/* C99 loop */
	{
		int l0;
		for (l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			dsp->iRec3[l0] = 0;
		}
	}
	/* C99 loop */
	{
		int l1;
		for (l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			dsp->fRec4[l1] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l2;
		for (l2 = 0; (l2 < 3); l2 = (l2 + 1)) {
			dsp->fRec2[l2] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l3;
		for (l3 = 0; (l3 < 3); l3 = (l3 + 1)) {
			dsp->fRec1[l3] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l4;
		for (l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			dsp->fVec0[l4] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l5;
		for (l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			dsp->iRec5[l5] = 0;
		}
	}
	/* C99 loop */
	{
		int l6;
		for (l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			dsp->fRec6[l6] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l7;
		for (l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			dsp->fRec7[l7] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l8;
		for (l8 = 0; (l8 < 3); l8 = (l8 + 1)) {
			dsp->fRec0[l8] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l9;
		for (l9 = 0; (l9 < 3); l9 = (l9 + 1)) {
			dsp->fRec8[l9] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l10;
		for (l10 = 0; (l10 < 3); l10 = (l10 + 1)) {
			dsp->fRec9[l10] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l11;
		for (l11 = 0; (l11 < 3); l11 = (l11 + 1)) {
			dsp->fRec10[l11] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l12;
		for (l12 = 0; (l12 < 3); l12 = (l12 + 1)) {
			dsp->fRec11[l12] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l13;
		for (l13 = 0; (l13 < 3); l13 = (l13 + 1)) {
			dsp->fRec12[l13] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l14;
		for (l14 = 0; (l14 < 3); l14 = (l14 + 1)) {
			dsp->fRec13[l14] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l15;
		for (l15 = 0; (l15 < 3); l15 = (l15 + 1)) {
			dsp->fRec14[l15] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l16;
		for (l16 = 0; (l16 < 3); l16 = (l16 + 1)) {
			dsp->fRec15[l16] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l17;
		for (l17 = 0; (l17 < 3); l17 = (l17 + 1)) {
			dsp->fRec16[l17] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l18;
		for (l18 = 0; (l18 < 3); l18 = (l18 + 1)) {
			dsp->fRec17[l18] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l19;
		for (l19 = 0; (l19 < 3); l19 = (l19 + 1)) {
			dsp->fRec18[l19] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l20;
		for (l20 = 0; (l20 < 3); l20 = (l20 + 1)) {
			dsp->fRec19[l20] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l21;
		for (l21 = 0; (l21 < 3); l21 = (l21 + 1)) {
			dsp->fRec20[l21] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l22;
		for (l22 = 0; (l22 < 3); l22 = (l22 + 1)) {
			dsp->fRec21[l22] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l23;
		for (l23 = 0; (l23 < 3); l23 = (l23 + 1)) {
			dsp->fRec22[l23] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l24;
		for (l24 = 0; (l24 < 3); l24 = (l24 + 1)) {
			dsp->fRec23[l24] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l25;
		for (l25 = 0; (l25 < 3); l25 = (l25 + 1)) {
			dsp->fRec24[l25] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l26;
		for (l26 = 0; (l26 < 3); l26 = (l26 + 1)) {
			dsp->fRec25[l26] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l27;
		for (l27 = 0; (l27 < 3); l27 = (l27 + 1)) {
			dsp->fRec26[l27] = 0.0f;
		}
	}
}

void instanceConstantsmydsp(mydsp* dsp, int sample_rate) {
	dsp->fSampleRate = sample_rate;
	float fConst0 = fminf(192000.0f, fmaxf(1.0f, (float)dsp->fSampleRate));
	dsp->fConst1 = (3.14159274f / fConst0);
	dsp->fConst2 = (44.0999985f / fConst0);
	dsp->fConst3 = (1.0f - dsp->fConst2);
	dsp->fConst4 = (0.00200000009f * fConst0);
	float fConst5 = powf(0.00100000005f, (1.66666663f / fConst0));
	dsp->fConst6 = (0.0f - (2.0f * fConst5));
	dsp->fConst7 = (6.28318548f / fConst0);
	dsp->fConst8 = mydsp_faustpower2_f(fConst5);
	float fConst9 = powf(0.00100000005f, (1.75438595f / fConst0));
	dsp->fConst10 = (0.0f - (2.0f * fConst9));
	dsp->fConst11 = mydsp_faustpower2_f(fConst9);
	float fConst12 = powf(0.00100000005f, (1.85185182f / fConst0));
	dsp->fConst13 = (0.0f - (2.0f * fConst12));
	dsp->fConst14 = mydsp_faustpower2_f(fConst12);
	float fConst15 = powf(0.00100000005f, (1.96078432f / fConst0));
	dsp->fConst16 = (0.0f - (2.0f * fConst15));
	dsp->fConst17 = mydsp_faustpower2_f(fConst15);
	float fConst18 = powf(0.00100000005f, (2.08333325f / fConst0));
	dsp->fConst19 = (0.0f - (2.0f * fConst18));
	dsp->fConst20 = mydsp_faustpower2_f(fConst18);
	float fConst21 = powf(0.00100000005f, (2.22222233f / fConst0));
	dsp->fConst22 = (0.0f - (2.0f * fConst21));
	dsp->fConst23 = mydsp_faustpower2_f(fConst21);
	float fConst24 = powf(0.00100000005f, (2.38095236f / fConst0));
	dsp->fConst25 = (0.0f - (2.0f * fConst24));
	dsp->fConst26 = mydsp_faustpower2_f(fConst24);
	float fConst27 = powf(0.00100000005f, (2.56410265f / fConst0));
	dsp->fConst28 = (0.0f - (2.0f * fConst27));
	dsp->fConst29 = mydsp_faustpower2_f(fConst27);
	float fConst30 = powf(0.00100000005f, (2.77777767f / fConst0));
	dsp->fConst31 = (0.0f - (2.0f * fConst30));
	dsp->fConst32 = mydsp_faustpower2_f(fConst30);
	float fConst33 = powf(0.00100000005f, (3.030303f / fConst0));
	dsp->fConst34 = (0.0f - (2.0f * fConst33));
	dsp->fConst35 = mydsp_faustpower2_f(fConst33);
	float fConst36 = powf(0.00100000005f, (3.33333325f / fConst0));
	dsp->fConst37 = (0.0f - (2.0f * fConst36));
	dsp->fConst38 = mydsp_faustpower2_f(fConst36);
	float fConst39 = powf(0.00100000005f, (3.70370364f / fConst0));
	dsp->fConst40 = (0.0f - (2.0f * fConst39));
	dsp->fConst41 = mydsp_faustpower2_f(fConst39);
	float fConst42 = powf(0.00100000005f, (4.16666651f / fConst0));
	dsp->fConst43 = (0.0f - (2.0f * fConst42));
	dsp->fConst44 = mydsp_faustpower2_f(fConst42);
	float fConst45 = powf(0.00100000005f, (4.76190472f / fConst0));
	dsp->fConst46 = (0.0f - (2.0f * fConst45));
	dsp->fConst47 = mydsp_faustpower2_f(fConst45);
	float fConst48 = powf(0.00100000005f, (5.55555534f / fConst0));
	dsp->fConst49 = (0.0f - (2.0f * fConst48));
	dsp->fConst50 = mydsp_faustpower2_f(fConst48);
	float fConst51 = powf(0.00100000005f, (6.66666651f / fConst0));
	dsp->fConst52 = (0.0f - (2.0f * fConst51));
	dsp->fConst53 = mydsp_faustpower2_f(fConst51);
	float fConst54 = powf(0.00100000005f, (8.33333302f / fConst0));
	dsp->fConst55 = (0.0f - (2.0f * fConst54));
	dsp->fConst56 = mydsp_faustpower2_f(fConst54);
	float fConst57 = powf(0.00100000005f, (11.1111107f / fConst0));
	dsp->fConst58 = (0.0f - (2.0f * fConst57));
	dsp->fConst59 = mydsp_faustpower2_f(fConst57);
	float fConst60 = powf(0.00100000005f, (16.666666f / fConst0));
	dsp->fConst61 = (0.0f - (2.0f * fConst60));
	dsp->fConst62 = mydsp_faustpower2_f(fConst60);
	float fConst63 = powf(0.00100000005f, (33.3333321f / fConst0));
	dsp->fConst64 = (0.0f - (2.0f * fConst63));
	dsp->fConst65 = mydsp_faustpower2_f(fConst63);
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
	ui_interface->openVerticalBox(ui_interface->uiInterface, "djembe");
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "Freq", &dsp->fHslider2, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.0f, (FAUSTFLOAT)1000.0f, (FAUSTFLOAT)0.00999999978f);
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "Pos", &dsp->fHslider0, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.0f, (FAUSTFLOAT)1.0f, (FAUSTFLOAT)9.99999975e-06f);
	ui_interface->addHorizontalSlider(ui_interface->uiInterface, "Sharpness", &dsp->fHslider1, (FAUSTFLOAT)0.5f, (FAUSTFLOAT)0.0f, (FAUSTFLOAT)5.0f, (FAUSTFLOAT)9.99999975e-06f);
	ui_interface->addButton(ui_interface->uiInterface, "trigger", &dsp->fButton0);
	ui_interface->closeBox(ui_interface->uiInterface);
}

void computemydsp(mydsp* dsp, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
	FAUSTFLOAT* output0 = outputs[0];
	float fSlow0 = (dsp->fConst2 * (float)dsp->fHslider0);
	float fSlow1 = (float)dsp->fButton0;
	float fSlow2 = (dsp->fConst2 * (float)dsp->fHslider1);
	float fSlow3 = (dsp->fConst2 * (float)dsp->fHslider2);
	/* C99 loop */
	{
		int i0;
		for (i0 = 0; (i0 < count); i0 = (i0 + 1)) {
			dsp->iRec3[0] = ((1103515245 * dsp->iRec3[1]) + 12345);
			dsp->fRec4[0] = (fSlow0 + (dsp->fConst3 * dsp->fRec4[1]));
			float fTemp0 = tanf((dsp->fConst1 * ((500.0f * dsp->fRec4[0]) + 40.0f)));
			float fTemp1 = (1.0f / fTemp0);
			float fTemp2 = mydsp_faustpower2_f(fTemp0);
			float fTemp3 = (((fTemp1 + 1.41421354f) / fTemp0) + 1.0f);
			dsp->fRec2[0] = ((4.65661287e-10f * (float)dsp->iRec3[0]) - (((dsp->fRec2[2] * (((fTemp1 + -1.41421354f) / fTemp0) + 1.0f)) + (2.0f * (dsp->fRec2[1] * (1.0f - (1.0f / fTemp2))))) / fTemp3));
			float fTemp4 = tanf((dsp->fConst1 * ((15000.0f * dsp->fRec4[0]) + 500.0f)));
			float fTemp5 = (1.0f / fTemp4);
			float fTemp6 = (((fTemp5 + 1.41421354f) / fTemp4) + 1.0f);
			dsp->fRec1[0] = (((((dsp->fRec2[1] * (0.0f - (2.0f / fTemp2))) + (dsp->fRec2[0] / fTemp2)) + (dsp->fRec2[2] / fTemp2)) / fTemp3) - (((dsp->fRec1[2] * (((fTemp5 + -1.41421354f) / fTemp4) + 1.0f)) + (2.0f * (dsp->fRec1[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp4)))))) / fTemp6));
			dsp->fVec0[0] = fSlow1;
			dsp->iRec5[0] = (((dsp->iRec5[1] + (dsp->iRec5[1] > 0)) * (fSlow1 <= dsp->fVec0[1])) + (fSlow1 > dsp->fVec0[1]));
			dsp->fRec6[0] = (fSlow2 + (dsp->fConst3 * dsp->fRec6[1]));
			float fTemp7 = ((float)dsp->iRec5[0] / fmaxf(1.0f, (dsp->fConst4 * dsp->fRec6[0])));
			float fTemp8 = (3.0f * (((dsp->fRec1[2] + (dsp->fRec1[0] + (2.0f * dsp->fRec1[1]))) * fmaxf(0.0f, fminf(fTemp7, (2.0f - fTemp7)))) / fTemp6));
			dsp->fRec7[0] = (fSlow3 + (dsp->fConst3 * dsp->fRec7[1]));
			dsp->fRec0[0] = (fTemp8 - ((dsp->fConst6 * (dsp->fRec0[1] * cosf((dsp->fConst7 * dsp->fRec7[0])))) + (dsp->fConst8 * dsp->fRec0[2])));
			dsp->fRec8[0] = (fTemp8 - ((dsp->fConst10 * (dsp->fRec8[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 200.0f))))) + (dsp->fConst11 * dsp->fRec8[2])));
			dsp->fRec9[0] = (fTemp8 - ((dsp->fConst13 * (dsp->fRec9[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 400.0f))))) + (dsp->fConst14 * dsp->fRec9[2])));
			dsp->fRec10[0] = (fTemp8 - ((dsp->fConst16 * (dsp->fRec10[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 600.0f))))) + (dsp->fConst17 * dsp->fRec10[2])));
			dsp->fRec11[0] = (fTemp8 - ((dsp->fConst19 * (dsp->fRec11[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 800.0f))))) + (dsp->fConst20 * dsp->fRec11[2])));
			dsp->fRec12[0] = (fTemp8 - ((dsp->fConst22 * (dsp->fRec12[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 1000.0f))))) + (dsp->fConst23 * dsp->fRec12[2])));
			dsp->fRec13[0] = (fTemp8 - ((dsp->fConst25 * (dsp->fRec13[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 1200.0f))))) + (dsp->fConst26 * dsp->fRec13[2])));
			dsp->fRec14[0] = (fTemp8 - ((dsp->fConst28 * (dsp->fRec14[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 1400.0f))))) + (dsp->fConst29 * dsp->fRec14[2])));
			dsp->fRec15[0] = (fTemp8 - ((dsp->fConst31 * (dsp->fRec15[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 1600.0f))))) + (dsp->fConst32 * dsp->fRec15[2])));
			dsp->fRec16[0] = (fTemp8 - ((dsp->fConst34 * (dsp->fRec16[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 1800.0f))))) + (dsp->fConst35 * dsp->fRec16[2])));
			dsp->fRec17[0] = (fTemp8 - ((dsp->fConst37 * (dsp->fRec17[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 2000.0f))))) + (dsp->fConst38 * dsp->fRec17[2])));
			dsp->fRec18[0] = (fTemp8 - ((dsp->fConst40 * (dsp->fRec18[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 2200.0f))))) + (dsp->fConst41 * dsp->fRec18[2])));
			dsp->fRec19[0] = (fTemp8 - ((dsp->fConst43 * (dsp->fRec19[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 2400.0f))))) + (dsp->fConst44 * dsp->fRec19[2])));
			dsp->fRec20[0] = (fTemp8 - ((dsp->fConst46 * (dsp->fRec20[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 2600.0f))))) + (dsp->fConst47 * dsp->fRec20[2])));
			dsp->fRec21[0] = (fTemp8 - ((dsp->fConst49 * (dsp->fRec21[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 2800.0f))))) + (dsp->fConst50 * dsp->fRec21[2])));
			dsp->fRec22[0] = (fTemp8 - ((dsp->fConst52 * (dsp->fRec22[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 3000.0f))))) + (dsp->fConst53 * dsp->fRec22[2])));
			dsp->fRec23[0] = (fTemp8 - ((dsp->fConst55 * (dsp->fRec23[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 3200.0f))))) + (dsp->fConst56 * dsp->fRec23[2])));
			dsp->fRec24[0] = (fTemp8 - ((dsp->fConst58 * (dsp->fRec24[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 3400.0f))))) + (dsp->fConst59 * dsp->fRec24[2])));
			dsp->fRec25[0] = (fTemp8 - ((dsp->fConst61 * (dsp->fRec25[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 3600.0f))))) + (dsp->fConst62 * dsp->fRec25[2])));
			dsp->fRec26[0] = (fTemp8 - ((dsp->fConst64 * (dsp->fRec26[1] * cosf((dsp->fConst7 * (dsp->fRec7[0] + 3800.0f))))) + (dsp->fConst65 * dsp->fRec26[2])));
			output0[i0] = (FAUSTFLOAT)(0.0500000007f * ((((((((((((((((((((dsp->fRec0[0] + (0.25f * (dsp->fRec8[0] - dsp->fRec8[2]))) + (0.111111112f * (dsp->fRec9[0] - dsp->fRec9[2]))) + (0.0625f * (dsp->fRec10[0] - dsp->fRec10[2]))) + (0.0399999991f * (dsp->fRec11[0] - dsp->fRec11[2]))) + (0.027777778f * (dsp->fRec12[0] - dsp->fRec12[2]))) + (0.0204081628f * (dsp->fRec13[0] - dsp->fRec13[2]))) + (0.015625f * (dsp->fRec14[0] - dsp->fRec14[2]))) + (0.0123456791f * (dsp->fRec15[0] - dsp->fRec15[2]))) + (0.00999999978f * (dsp->fRec16[0] - dsp->fRec16[2]))) + (0.00826446246f * (dsp->fRec17[0] - dsp->fRec17[2]))) + (0.0069444445f * (dsp->fRec18[0] - dsp->fRec18[2]))) + (0.00591715984f * (dsp->fRec19[0] - dsp->fRec19[2]))) + (0.00510204071f * (dsp->fRec20[0] - dsp->fRec20[2]))) + (0.00444444455f * (dsp->fRec21[0] - dsp->fRec21[2]))) + (0.00390625f * (dsp->fRec22[0] - dsp->fRec22[2]))) + (0.00346020772f * (dsp->fRec23[0] - dsp->fRec23[2]))) + (0.00308641978f * (dsp->fRec24[0] - dsp->fRec24[2]))) + (0.00277008303f * (dsp->fRec25[0] - dsp->fRec25[2]))) + (0.00249999994f * (dsp->fRec26[0] - dsp->fRec26[2]))) - dsp->fRec0[2]));
			dsp->iRec3[1] = dsp->iRec3[0];
			dsp->fRec4[1] = dsp->fRec4[0];
			dsp->fRec2[2] = dsp->fRec2[1];
			dsp->fRec2[1] = dsp->fRec2[0];
			dsp->fRec1[2] = dsp->fRec1[1];
			dsp->fRec1[1] = dsp->fRec1[0];
			dsp->fVec0[1] = dsp->fVec0[0];
			dsp->iRec5[1] = dsp->iRec5[0];
			dsp->fRec6[1] = dsp->fRec6[0];
			dsp->fRec7[1] = dsp->fRec7[0];
			dsp->fRec0[2] = dsp->fRec0[1];
			dsp->fRec0[1] = dsp->fRec0[0];
			dsp->fRec8[2] = dsp->fRec8[1];
			dsp->fRec8[1] = dsp->fRec8[0];
			dsp->fRec9[2] = dsp->fRec9[1];
			dsp->fRec9[1] = dsp->fRec9[0];
			dsp->fRec10[2] = dsp->fRec10[1];
			dsp->fRec10[1] = dsp->fRec10[0];
			dsp->fRec11[2] = dsp->fRec11[1];
			dsp->fRec11[1] = dsp->fRec11[0];
			dsp->fRec12[2] = dsp->fRec12[1];
			dsp->fRec12[1] = dsp->fRec12[0];
			dsp->fRec13[2] = dsp->fRec13[1];
			dsp->fRec13[1] = dsp->fRec13[0];
			dsp->fRec14[2] = dsp->fRec14[1];
			dsp->fRec14[1] = dsp->fRec14[0];
			dsp->fRec15[2] = dsp->fRec15[1];
			dsp->fRec15[1] = dsp->fRec15[0];
			dsp->fRec16[2] = dsp->fRec16[1];
			dsp->fRec16[1] = dsp->fRec16[0];
			dsp->fRec17[2] = dsp->fRec17[1];
			dsp->fRec17[1] = dsp->fRec17[0];
			dsp->fRec18[2] = dsp->fRec18[1];
			dsp->fRec18[1] = dsp->fRec18[0];
			dsp->fRec19[2] = dsp->fRec19[1];
			dsp->fRec19[1] = dsp->fRec19[0];
			dsp->fRec20[2] = dsp->fRec20[1];
			dsp->fRec20[1] = dsp->fRec20[0];
			dsp->fRec21[2] = dsp->fRec21[1];
			dsp->fRec21[1] = dsp->fRec21[0];
			dsp->fRec22[2] = dsp->fRec22[1];
			dsp->fRec22[1] = dsp->fRec22[0];
			dsp->fRec23[2] = dsp->fRec23[1];
			dsp->fRec23[1] = dsp->fRec23[0];
			dsp->fRec24[2] = dsp->fRec24[1];
			dsp->fRec24[1] = dsp->fRec24[0];
			dsp->fRec25[2] = dsp->fRec25[1];
			dsp->fRec25[1] = dsp->fRec25[0];
			dsp->fRec26[2] = dsp->fRec26[1];
			dsp->fRec26[1] = dsp->fRec26[0];
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif
