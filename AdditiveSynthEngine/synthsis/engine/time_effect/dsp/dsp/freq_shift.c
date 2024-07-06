#include "freq_shift.h"

#define M_PI 3.1415926535897932384626



void FreqShiftInit(FreqShiftData* pdat)
{
	memset(pdat, 0, sizeof(FreqShiftData));//Ê¡ÊÂ
	pdat->rot_re = 1.0;
	pdat->rot_im = 0.0;
}

float FreqShiftProc(FreqShiftData* pdat, float in)
{
	float sign_re = in;
	float sign_im = in;

	sign_re = PolyPhaseAPFProc(&pdat->a0, sign_re, 0.4021921162426);
	sign_re = PolyPhaseAPFProc(&pdat->a1, sign_re, 0.8561710882420);
	sign_re = PolyPhaseAPFProc(&pdat->a2, sign_re, 0.9722909545651);
	sign_re = PolyPhaseAPFProc(&pdat->a3, sign_re, 0.9952884791278);

	sign_im = PolyPhaseAPFProc(&pdat->a4, sign_im, 0.6923878000000);
	sign_im = PolyPhaseAPFProc(&pdat->a5, sign_im, 0.9360654322959);
	sign_im = PolyPhaseAPFProc(&pdat->a6, sign_im, 0.9882295226860);
	sign_im = PolyPhaseAPFProc(&pdat->a7, sign_im, 0.9987488452737);
	float get_im = pdat->im_z;//z^-1
	pdat->im_z = sign_im;

	pdat->t += pdat->w;

	return sign_re * cos(pdat->t) - get_im * sin(pdat->t);
}

void FreqShiftSetFreq(FreqShiftData* pdat, float freq)
{
	pdat->w = freq / 48000.0 * 2.0 * M_PI;
}
