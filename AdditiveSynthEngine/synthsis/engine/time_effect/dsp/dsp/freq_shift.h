#ifndef _HDSP_FREQ_SHIFT_
#define _HDSP_FREQ_SHIFT_

#include <stdint.h>
#include <math.h>
#include "apf.h"


typedef struct _FREQ_SHIFT_DATA_
{
	APFData a0, a1, a2, a3;
	APFData a4, a5, a6, a7;

	float* costable;
	float* sintable;
	float im_z;//im delay 1
	float rot_re, rot_im;
	uint32_t pos;//±íµÄÖ¸Õë
	float spd_re, spd_im;
	double w, t;
}FreqShiftData;

static const float __fs_re[4] = { 0.4021921162426, 0.8561710882420, 0.9722909545651, 0.9952884791278 };  //hilbert iir apf³£Êý
static const float __fs_im[4] = { 0.6923878000000, 0.9360654322959, 0.9882295226860, 0.9987488452737 };

#ifdef __cplusplus
extern "C" {
#endif
	void FreqShiftInit(FreqShiftData* pdat);
	float FreqShiftProc(FreqShiftData* pdat, float in);
	void FreqShiftSetFreq(FreqShiftData* pdat, float freq);
#ifdef __cplusplus
}
#endif

#endif