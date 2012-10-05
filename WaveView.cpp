/*
 *  WaveView.cpp
 *  Chip700
 *
 *  Created by osoumen on 12/10/04.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "WaveView.h"

static CFontDesc g_MessageFont("Courier", 18);

//-----------------------------------------------------------------------------
CWaveView::CWaveView(CRect &size, CFrame *pFrame, CControlListener* listener, long tag)
: CControl(size, listener, tag, 0)
{
	m_pDrawBuffer = new COffscreenContext(pFrame, size.right-size.left, size.bottom-size.top);
	
	lineColor = MakeCColor(180, 248, 255, 255);
	backColor = MakeCColor(67, 75, 88, 255);
	isWaveLoaded = false;
	looppoint = 0;
	datanum = 0;
	converting = false;
}

//-----------------------------------------------------------------------------
CWaveView::~CWaveView()
{
	delete m_pDrawBuffer;
}

//-----------------------------------------------------------------------------
bool CWaveView::onDrop(void **ptrItems, long nbItems, long type, CPoint &where)
{
//	if (nbItems > 0 && type == kDropFiles) {
//		X_fierGUI	*editor = (X_fierGUI *)getEditor();
//		editor->fileDropped((FSSpec *)ptrItems[0]);
//	}
	
	return true;
}

//------------------------------------------------------------------------
void CWaveView::draw(CDrawContext *pContext)
{
	if (converting)
	{
		pContext->setFrameColor(kBlackCColor);
		pContext->setFillColor(backColor);
		pContext->drawRect(getVisibleSize(), kDrawFilledAndStroked);
		
		CRect oldClip;
		pContext->getClipRect(oldClip);
		CRect newClip(size);
		newClip.offset(10, 32);
		newClip.bound(oldClip);
		pContext->setClipRect(newClip);
		pContext->setFont(&g_MessageFont);
		pContext->setFontColor(lineColor);
		
#if VSTGUI_USES_UTF8
		pContext->drawStringUTF8("Converting to ADPCM...", newClip, kLeftText, true);
#else
		pContext->drawString("Converting to ADPCM...", newClip, true, kLeftText);
#endif
		pContext->setClipRect(oldClip);
	}
	else
	{
		if (isWaveLoaded)
		{
			m_pDrawBuffer->copyFrom(pContext, size);
			//���[�v�|�C���g�̈ʒu��`��
			if (looppoint > 0) {
				pContext->setFrameColor(kYellowCColor);
				CPoint	point(size.left + getWidth()*looppoint/datanum, size.top + 2);
				pContext->moveTo(point);
				point.offset(0, getHeight() - 3);
				pContext->lineTo(point);
			}
		}
		else
		{
			pContext->setFrameColor(kBlackCColor);
			pContext->setFillColor(backColor);
			pContext->drawRect(getVisibleSize(), kDrawFilledAndStroked);
			
			CRect oldClip;
			pContext->getClipRect(oldClip);
			CRect newClip(size);
			newClip.offset(10, 32);
			newClip.bound(oldClip);
			pContext->setClipRect(newClip);
			pContext->setFont(&g_MessageFont);
			pContext->setFontColor(lineColor);
			
#if VSTGUI_USES_UTF8
			pContext->drawStringUTF8("Drop audio file here.", newClip, kLeftText, true);
#else
			pContext->drawString("Drop audio file here.", newClip, true, kLeftText);
#endif
			pContext->setClipRect(oldClip);
		}
	}
}

//------------------------------------------------------------------------
void CWaveView::setWave(float *wavedata, long frames)
{
	CRect	r(0, 0, m_pDrawBuffer->getWidth(), m_pDrawBuffer->getHeight());
	m_pDrawBuffer->setFrameColor(kBlackCColor);
	m_pDrawBuffer->setFillColor(backColor);
	m_pDrawBuffer->drawRect(r, kDrawFilledAndStroked);
	
	float	*dataptr=wavedata;
	float	*dataend=wavedata+frames;
	float	pixelPerFrame=(float)m_pDrawBuffer->getWidth()/(float)frames;
	
	//dataptr++;
	m_pDrawBuffer->setFrameColor(lineColor);
	m_pDrawBuffer->setDrawMode(kAntialias);
	
	CPoint p;
	float	center = m_pDrawBuffer->getHeight()/2 - 1;	//�O�g�P�h�b�g���]���̂���
	float	x=1,y=0;
	float	xLoc=1.0f;
	float	max=.0f,min=.0f;
	while (dataptr < dataend) {
		float	temp = *dataptr * center;
		if (max==.0f) max=temp;
		if (min==.0f) min=temp;
		if (temp > max) max = temp;
		if (temp < min) min = temp;
		if (xLoc > 1.0f) {
			double	xadd;
			xLoc = modf(xLoc, &xadd);
			y = center - min + 2;	//�O�g�P�h�b�g���]���̂���
			p.x = x;
			p.y = y;
			if ( x == 1 )
			{
				m_pDrawBuffer->moveTo(p);
			}
			else
			{
				m_pDrawBuffer->lineTo(p);
			}
				
			if (max != min) {
				y = center-max;
				m_pDrawBuffer->lineTo(p);
			}
			max=0.0f;
			min=0.0f;
			
			x += xadd;
		}
		dataptr++;
		xLoc += pixelPerFrame;
	}
	if (max != 0.0f) {
		y = center-min;
		m_pDrawBuffer->lineTo(p);
		if (max != min) {
			y = center-max;
			m_pDrawBuffer->lineTo(p);
		}
	}
	isWaveLoaded = true;
	converting = false;
	datanum = frames;
	
	setDirty();
}

//------------------------------------------------------------------------
void CWaveView::setBackColor(CColor color)
{
	backColor = color;
}

//------------------------------------------------------------------------
void CWaveView::setLineColor(CColor color)
{
	lineColor = color;
}

//------------------------------------------------------------------------
void CWaveView::beginConverting()
{
	converting = true;
	setDirty();
}

//------------------------------------------------------------------------
void CWaveView::endConverting()
{
	converting = false;
	setDirty();
}

//------------------------------------------------------------------------
void CWaveView::setLooppoint(int loopStartSamp)
{
	looppoint = loopStartSamp;
	setDirty();
}