/*
**	fft_czt.h -- header file for fft.c and czt.c
**
**	Public domain by MIYASAKA Masaru <alkaid@coral.ocn.ne.jp> (Sep 15, 2003)
**
**	[TAB4]
*/

#ifndef FFT_CZT_H
#define FFT_CZT_H

#include <Accelerate/Accelerate.h>

	/* CZT�v�Z�p ���\�ێ��\���� */
typedef struct {
	FFTSetup		fftsetup;		/* ������ FFT�v�Z�p */
	bool	no_czt;
	int		m;
	int		samples;		/* �W�{�_�̐� */
	int		samples_out;	/* �o�͂���W�{�_�̐� */
	int		samples_ex;	/* 2�̐�����ŁA(samples + samples_out) <= samples_ex
						 * �ł���ŏ��̐� */
	DSPSplitComplex w;		/* �d�݃f�[�^ - �v�f����(samples) */
	DSPSplitComplex v;		/* �C���p���X�����f�[�^ - �v�f����(samples_ex) */
	DSPSplitComplex t;		/* ��Ɨp�̈� - �v�f����(samples_ex) */
} czt_struct;

	/* czt.c */
int czt_init(czt_struct *cztp, int n, int no);
void czt_end(czt_struct *cztp);
void czt(czt_struct *cztp, int inv, DSPSplitComplex *input);

int estimatebasefreq(short *src, int length);

#endif	/* FFT_CZT_H */
