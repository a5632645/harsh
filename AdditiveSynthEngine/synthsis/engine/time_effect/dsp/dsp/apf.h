#ifndef _HDSP_APF_
#define _HDSP_APF_

//用完projucer中文全乱码了

typedef struct
{
	float z2, z1;
} APFData;


#ifdef __cplusplus
extern "C" {
#endif

	float PolyPhaseAPFProc(APFData* p, float sign, float a);


#ifdef __cplusplus
}
#endif

#endif