/*
* Hiirofox 2024/06/24
*/
#ifndef _HDSP_FDN_REVERB_
#define _HDSP_FDN_REVERB_

#include <stdint.h>
#include <math.h>
#include "freq_shift.h"

#define FDN_DELAY_NUM 12     //fdn混响的delay line的个数
#define FDN_MAX_DELAY 12000 //fdb每个delay line的最大长度 

typedef struct _STEREO_FLOAT_
{
	float l, r;
}StereoFloat;

typedef struct _FDN_DATA_
{
	float datal[FDN_DELAY_NUM][FDN_MAX_DELAY];//存左右声道的
	float datar[FDN_DELAY_NUM][FDN_MAX_DELAY];
	float fdbkl[FDN_DELAY_NUM];//反馈的临时量
	float fdbkr[FDN_DELAY_NUM];
	float totfbl;//一起反馈(接freq shift)
	float totfbr;
	FreqShiftData freqshiftl;
	FreqShiftData freqshiftr;
	int32_t posl[FDN_DELAY_NUM];//延迟的位置
	int32_t posr[FDN_DELAY_NUM];

	//param
	int32_t dlytimel[FDN_DELAY_NUM];//延迟时间
	int32_t dlytimer[FDN_DELAY_NUM];//延迟时间
	float b;//输入
	float c;//输出
	float g1[FDN_DELAY_NUM], g2[FDN_DELAY_NUM];//反馈
	float g3;
	float mat[FDN_DELAY_NUM][FDN_DELAY_NUM];//反馈乘个矩阵
	float sep;//左右声道分离度
	//如果把反馈线部分左右互换，就可以视为在左右声道做分离
	float dry, wet;
}FDNData;

//Hadamard矩阵 
static float __hadamard_mat2x2[2][2] = { {1, 1},
										 {1,-1} };
static float __hadamard_mat4x4[4][4] = { {1, 1, 1, 1},
										 {1,-1, 1,-1},
										 {1, 1,-1,-1},
										 {1,-1,-1, 1} };

//16个延迟的大概参数
static int32_t __delay_M16[16] = { 441, 513, 593, 738, 890, 1135, 1388, 1689, 2059, 2446, 2874, 3388, 3906, 4466, 5089, 5755 };

#ifdef __cplusplus
extern "C" {
#endif

	void FDNInit(FDNData* pdat);//初始化
	StereoFloat FDNProc(FDNData* pdat, StereoFloat input);//处理单个采样
	void SetFDNRoomSize(FDNData* pdat, float size_l, float size_r, float mode);//设置房间大小[0,1]，以及分布形式[0,1]
	void SetFDNSeparate(FDNData* pdat, float separate);//设置声道分离度[0,1]
	void SetFDNDryWet(FDNData* pdat, float dry, float wet);//干湿比[0,1]
	void SetFDNFeedback(FDNData* pdat, float feedback1, float feedback2, float feedback3);//反馈[0,1]
	void SetFDNFreqShift(FDNData* pdat, float freq);//设置freq shift的量
	//以下是反馈矩阵的选择
	void FDNApplyHadamardMatrix(FDNData* pdat);//应用Hadamard矩阵(据说是很好的一种)
	void FDNApplyRandomMatrix(FDNData* pdat);  //应用随机矩阵(能允许任意FDN_DELAY_NUM)
	void FDNApplyHouseholderMatrix(FDNData* pdat);  //应用Householder矩阵(据说能最快达到一定的回声密度)
#ifdef __cplusplus
}
#endif


#endif