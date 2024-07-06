#include "apf.h"

float PolyPhaseAPFProc(APFData* p, float sign, float a)
{ // faster
	sign += p->z2 * a;
	a = p->z2 - sign * a;
	p->z2 = p->z1;
	p->z1 = sign;
	return a;
}
