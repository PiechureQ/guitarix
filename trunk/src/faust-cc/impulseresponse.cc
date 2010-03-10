namespace impulseresponse {
// generated from file '../src/faust/impulseresponse.dsp'

FAUSTFLOAT&	fslider0=*(float*)&GxEngine::instance()->fslider20;
float 	fConst0;
FAUSTFLOAT&	fslider1=*(float*)&GxEngine::instance()->fslider21;
float 	fConst1;
float 	fVec0[3];
FAUSTFLOAT&	fslider2=*(float*)&GxEngine::instance()->fslider22;
float 	fRec0[3];
int	fSamplingFreq;

void init(int samplingFreq)
{
	fSamplingFreq = samplingFreq;
	fConst0 = (3.141593f / fSamplingFreq);
	fConst1 = (6.283185f / fSamplingFreq);
	for (int i=0; i<3; i++) fVec0[i] = 0;
	for (int i=0; i<3; i++) fRec0[i] = 0;
}

void compute(int count, float *input0, float *output0)
{
	float 	fSlow0 = expf((0 - (fConst0 * fslider0)));
	float 	fSlow1 = (2 * cosf((fConst1 * fslider1)));
	float 	fSlow2 = (0.5f * (fslider2 * (1 - (fSlow0 * fSlow0))));
	for (int i=0; i<count; i++) {
		float fTemp0 = (float)input0[i];
		fVec0[0] = fTemp0;
		fRec0[0] = ((fSlow2 * (fVec0[0] - fVec0[2])) + (fSlow0 * ((fSlow1 * fRec0[1]) - (fSlow0 * fRec0[2]))));
		output0[i] = (FAUSTFLOAT)fRec0[0];
		// post processing
		fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
		fVec0[2] = fVec0[1]; fVec0[1] = fVec0[0];
	}
}

static struct RegisterParams { RegisterParams(); } RegisterParams;
RegisterParams::RegisterParams()
{
	registerVar("IR.peak","","S","peak gain",&fslider2, 1.0f, 0.0f, 10.0f, 0.2f);
	registerVar("IR.freq","","S","frequency (Hz)",&fslider1, 440.0f, 20.0f, 12000.0f, 10.0f);
	registerVar("IR.bandwidth","","S","bandwidth (Hz)",&fslider0, 100.0f, 20.0f, 20000.0f, 10.0f);
	registerInit(init);
}

} // end namespace impulseresponse