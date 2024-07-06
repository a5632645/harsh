#include "fdn_reverb.h"

#ifdef __cplusplus
extern "C" {
#endif

	void FDNInit(FDNData* pdat)
	{
		memset(pdat, 0, sizeof(FDNData));//简单粗暴
		FreqShiftInit(&pdat->freqshiftl);
		FreqShiftInit(&pdat->freqshiftr);
		FreqShiftSetFreq(&pdat->freqshiftl, 0);
		FreqShiftSetFreq(&pdat->freqshiftr, 0);
	}

	StereoFloat FDNProc(FDNData* pdat, StereoFloat input)
	{
		float matl[FDN_DELAY_NUM];//计算乘mat之后的反馈
		float matr[FDN_DELAY_NUM];
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//fdbk乘矩阵
		{
			matl[i] = 0;
			matr[i] = 0;
			for (int n = 0; n < FDN_DELAY_NUM; ++n)
			{
				matl[i] = pdat->mat[i][n] * pdat->fdbkl[n];
				matr[i] = pdat->mat[i][n] * pdat->fdbkr[n];
			}
		}

		//pdat->totfbl = FreqShiftProc(&pdat->freqshiftl, pdat->totfbl);//freq shift
		//pdat->totfbr = FreqShiftProc(&pdat->freqshiftr, pdat->totfbr);//不如不加

		for (int i = 0; i < FDN_DELAY_NUM; ++i)//输入延迟线
		{
			int input_posl = (pdat->posl[i] + pdat->dlytimel[i]) % FDN_MAX_DELAY;
			int input_posr = (pdat->posr[i] + pdat->dlytimer[i]) % FDN_MAX_DELAY;
			float fdbkl = matl[i] * pdat->g1[i] + pdat->fdbkl[i] * pdat->g2[i] - pdat->totfbl * pdat->g3;//三种反馈
			float fdbkr = matr[i] * pdat->g1[i] + pdat->fdbkr[i] * pdat->g2[i] - pdat->totfbr * pdat->g3;
			pdat->datal[i][input_posl] = input.l * pdat->b + fdbkl * (1.0 - pdat->sep) + fdbkr * (pdat->sep);
			pdat->datar[i][input_posr] = input.r * pdat->b + fdbkr * (1.0 - pdat->sep) + fdbkl * (pdat->sep);
		}

		StereoFloat out;
		out.l = 0;
		out.r = 0;
		pdat->totfbl = 0;
		pdat->totfbr = 0;
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//输出
		{
			int output_posl = pdat->posl[i] % FDN_MAX_DELAY;
			int output_posr = pdat->posr[i] % FDN_MAX_DELAY;
			float outl = pdat->datal[i][output_posl];
			float outr = pdat->datar[i][output_posr];
			out.l += outl * pdat->c;//输出
			out.r += outr * pdat->c;
			pdat->fdbkl[i] = outl;
			pdat->fdbkr[i] = outr;
			pdat->totfbl += outl;
			pdat->totfbr += outr;
		}
		for (int i = 0; i < FDN_DELAY_NUM; ++i)//更新pos
		{
			pdat->posl[i] = pdat->posl[i] + 1;
			pdat->posr[i] = pdat->posr[i] + 1;
		}

		out.l = out.l * pdat->wet + input.l * pdat->dry;
		out.r = out.r * pdat->wet + input.r * pdat->dry;

		//out.l = FreqShiftProc(&pdat->freqshiftl, input.l);//test freq shift
		//out.r = FreqShiftProc(&pdat->freqshiftr, input.r);

		return out;
	}

	void SetFDNRoomSize(FDNData* pdat, float size_l, float size_r, float mode)
	{
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			//int bl = __delay_M16[i] * size_l;//FDN_DELAY_NUM=16时可以用 
			//int br = __delay_M16[i] * size_r;
			float a = (float)(i + 1) / FDN_DELAY_NUM;
			int bl = (pow(mode + 1.0, a) - 1.0) * size_l * FDN_MAX_DELAY;//这个也是玄学 
			int br = (pow(mode + 1.0, a) - 1.0) * size_r * FDN_MAX_DELAY;
			if (bl >= FDN_MAX_DELAY)bl = FDN_MAX_DELAY - 1;
			if (br >= FDN_MAX_DELAY)br = FDN_MAX_DELAY - 1;
			if (bl < 0)bl = 0;
			if (br < 0)br = 0;

			pdat->dlytimel[i] = bl;
			pdat->dlytimer[i] = br;
		}
	}

	void SetFDNSeparate(FDNData* pdat, float separate)
	{
		pdat->sep = separate;
	}

	void SetFDNDryWet(FDNData* pdat, float dry, float wet)
	{
		pdat->dry = dry;
		pdat->wet = wet;
	}

	void SetFDNFeedback(FDNData* pdat, float feedback1, float feedback2, float feedback3)
	{

		pdat->b = 1.0;//关于b和c咋调，文档也没有说明。。。
		pdat->c = 1.0;
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			pdat->g1[i] = feedback1;
			pdat->g2[i] = exp((double)pdat->dlytimel[i] / 48000.0 * log(feedback2) / log(2.71828182));
			//如果feedback是一样的话，对于不同长度的delay，每个delay的衰减时间不一样
			//我这个是为了平衡衰减时间
		}
		pdat->g3 = feedback3;
	}

	void SetFDNFreqShift(FDNData* pdat, float freq)
	{
		FreqShiftSetFreq(&pdat->freqshiftl, freq);
		FreqShiftSetFreq(&pdat->freqshiftr, freq);
	}

	void FDNApplyHadamardMatrix(FDNData* pdat)
	{
		if (FDN_DELAY_NUM == 4)
		{
			for (int i = 0; i < FDN_DELAY_NUM; ++i)
			{
				for (int j = 0; j < FDN_DELAY_NUM; ++j)
				{
					pdat->mat[i][j] = __hadamard_mat4x4[i][j] * 0.5;//这个0.5也是为了归一化
				}
			}
		}

	}

	void FDNApplyRandomMatrix(FDNData* pdat)
	{
		float tmp[FDN_DELAY_NUM][FDN_DELAY_NUM];
		float power = 0;
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			for (int j = 0; j < FDN_DELAY_NUM; ++j)
			{
				float rnd = (float)(rand() % 10000) / 10000.0 * (rand() % 2 ? 1 : -1);
				power += rnd * rnd;//用来计算能量
				tmp[i][j] = rnd;
			}
		}
		power = sqrt(power);//这个矩阵现在的能量
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			for (int j = 0; j < FDN_DELAY_NUM; ++j)
			{
				pdat->mat[i][j] = tmp[i][j] / power;//归一化
			}
		}
	}

	void FDNApplyHouseholderMatrix(FDNData* pdat)
	{
		for (int i = 0; i < FDN_DELAY_NUM; ++i)
		{
			for (int j = 0; j < FDN_DELAY_NUM; ++j)
			{
				if (i != j)
					pdat->mat[i][j] = -1 * 0.5;
				else
					pdat->mat[i][j] = 1 * 0.5;
			}
		}
	}
#ifdef __cplusplus
}
#endif