/*
 *  C700GUI.cpp
 *  C700
 *
 *  Created by osoumen on 12/10/01.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "C700GUI.h"
#include "ControlInstacnesDefs.h"
#include "brrcodec.h"
#include "cfileselector.h"
#include <math.h>

#include "MersenneTwister.h"

#if MAC
#include "czt.h"
#else
#include "noveclib/fft_czt.h"
#endif

#if AU
#include "plugguieditor.h"
#else
#include "aeffguieditor.h"
#endif

#include <time.h>

//-----------------------------------------------------------------------------
C700GUI::C700GUI(const CRect &inSize, CFrame *frame, CBitmap *pBackground)
: CViewContainer (inSize, frame, pBackground)
, recordWindow(NULL)
, efxAcc(NULL)
{
	//���ʃO���t�B�b�N�̓ǂݍ���
    CBitmap					*sliderHandleBitmap;
	CBitmap					*onOffButton;
	CBitmap					*bgKnob;
	CBitmap					*rocker;
	bgKnob = new CBitmap("knobBack.png");
	sliderHandleBitmap = new CBitmap("sliderThumb.png");
	onOffButton = new CBitmap("bt_check.png");
	rocker = new CBitmap("rocker_sw.png");
	
	//�R���g���[���̌�
	int numCntls = sizeof(sCntl) / sizeof(ControlInstances);
	
	//�쐬�����R���g���[���̃C���X�^���X�ւ̃|�C���^��ێ����Ă���
	for ( int i=0; i<numCntls; i++ )
	{
		CControl	*cntl;
		cntl = makeControlFrom( &sCntl[i], frame, this, sliderHandleBitmap, onOffButton, bgKnob, rocker );
		if ( cntl )
		{
			addView(cntl);
            // -1�̓^�O���ݒ��\��
            if (cntl->getTag() != -1) {
                mCntl[cntl->getTag()] = cntl;
            }
		}
	}
	
	bgKnob->forget();
	sliderHandleBitmap->forget();
	onOffButton->forget();
	rocker->forget();
	
    CRect size (0, 0, inSize.getWidth() - 114, inSize.getHeight() - 100);
    size.offset (50, 50);
    recordWindow = new RecordingSettingsGUI(size, frame, NULL);
    
	//�ȉ��e�X�g�R�[�h
#if 0
    CMyKnob				*cKnob;
	CMySlider			*cVerticalSlider;
	CLabelOnOffButton	*cCheckBox;
	CRockerSwitch		*cRockerSwitch;
	CWaveView			*cWaveView;
	CDummyCntl			*cDummyTest;
    
    //--COptionMenu--------------------------------------
	CRect size (0, 0, 50, 14);
	size.offset (10, 30);
    
	long style = kCheckStyle;
	COptionMenu *cOptionMenu = new COptionMenu(size, this, -1, 0, 0, style);
	if (cOptionMenu)
	{
        CFontRef fontDesc = new CFontDesc(kLabelFont->getName(), 9);
		cOptionMenu->setFont(fontDesc);
		cOptionMenu->setFontColor(kWhiteCColor);
		cOptionMenu->setBackColor(kBlackCColor);
		cOptionMenu->setFrameColor(kBlackCColor);
		cOptionMenu->setHoriAlign(kLeftText);
		for (int i = 0; i < 3; i++)
		{
			char txt[256];
			sprintf(txt, "Entry %d", i);
			cOptionMenu->addEntry(txt);
		}
		addView(cOptionMenu);
		//cOptionMenu->setAttribute(kCViewTooltipAttribute,strlen("COptionMenu")+1,"COptionMenu");
        fontDesc->forget();
	}
    
	//--CMyKnob--------------------------------------
	CBitmap *bgKnob = new CBitmap("knobBack.png");
	
 	CRect	size(0, 0, bgKnob->getWidth(), bgKnob->getHeight());
	size.offset(140 + 15, 15);
	CPoint	point(0, 0);
	cKnob = new CMyKnob(size, this, 801, bgKnob, 0, point);
	cKnob->setColorHandle( MakeCColor(67, 75, 88, 255) );
	cKnob->setColorShadowHandle( kTransparentCColor );
	cKnob->setInsetValue(1);
	cKnob->setMax(2.0f);
	cKnob->setMin(-2.0f);
	addView(cKnob);
	bgKnob->forget();
	cKnob->setAttribute(kCViewTooltipAttribute,strlen("CMyKnob")+1,"CMyKnob");
	
	//--CMySlider--------------------------------------
	CBitmap *sliderHandleBitmap = new CBitmap("sliderThumb.png");
	
	size(0, 0, sliderHandleBitmap->getWidth(), 128);
	size.offset(280, 70);
#if 1
	point(0, 0);
	cVerticalSlider = new CMySlider(size, this, 505, size.top, size.top + 128 - sliderHandleBitmap->getHeight(), sliderHandleBitmap, 0, point, kBottom|kVertical);
//	point(0, 0);
//	cVerticalSlider->setOffsetHandle(point);
#else
	point(0, 0);
	CPoint handleOffset(0, 0);
	cVerticalSlider = new CMySlider(size, this, 505, handleOffset, 
										   size.height() - 2 * handleOffset.v, sliderHandleBitmap, 0, point, kBottom);
#endif
	//cVerticalSlider->setFreeClick(false);
	cVerticalSlider->setMax(2.0f);
	cVerticalSlider->setMin(-2.f);
	addView(cVerticalSlider);
	cVerticalSlider->setAttribute(kCViewTooltipAttribute,strlen("CMySlider")+1,"CMySlider");
	
	sliderHandleBitmap->forget();
	
	//--CLabelOnOffButton--------------------------------------
	CBitmap *onOffButton = new CBitmap("bt_check.png");
	
	size(0, 0, 80, onOffButton->getHeight() / 2);
	size.offset(20, 20);
	cCheckBox = new CLabelOnOffButton(size, this, 808, onOffButton, "Echo");
	addView(cCheckBox);
	cCheckBox->setAttribute(kCViewTooltipAttribute,strlen("CLabelOnOffButton")+1,"CLabelOnOffButton");
	
	onOffButton->forget();
	
	//--CRockerSwitch--------------------------------------
	CBitmap *rocker = new CBitmap("rocker_sw.png");
 	size(0, 0, rocker->getWidth(), rocker->getHeight() / 3);
	size.offset(9, 70 + 29);
	point(0, 0);
	cRockerSwitch = new CRockerSwitch(size, this, 707, rocker->getHeight() / 3, rocker, point, kVertical);
	addView(cRockerSwitch);
	rocker->forget();
	cRockerSwitch->setAttribute(kCViewTooltipAttribute,strlen("CRockerSwitch")+1,"CRockerSwitch");
	
	//--CWaveView--------------------------------------
 	size(0, 0, 200, 100);
	size.offset(64, 128);
	cWaveView = new CWaveView(size, frame, this, 606);
	{
		short	testWave[1024];
		float	freq = 1.0f;
		for ( int i=0; i<1024; i++ )
		{
			testWave[i] = sin( 3.14*2*freq * i / 1024 )*32767;
			cWaveView->setWave(testWave, 1024);
			cWaveView->setLooppoint(96);
			freq += 0.03f;
		}
	}
	addView(cWaveView);
	cWaveView->setAttribute(kCViewTooltipAttribute,strlen("CWaveView")+1,"CWaveView");
	
	//--CDummyCntl--------------------------------------
 	size(0, 0, 200, 100);
	size.offset(256, 256);
	cDummyTest = new CDummyCntl(size);
	addView(cDummyTest);
	cDummyTest->setAttribute(kCViewTooltipAttribute,strlen("CDummyCntl")+1,"CDummyCntl");
#endif
}

//-----------------------------------------------------------------------------
C700GUI::~C700GUI()
{
    if (recordWindow) {
        recordWindow->forget();
    }
	//mLabelFont->forget();
	removeAll();
}

//-----------------------------------------------------------------------------
void C700GUI::valueChanged(CControl* control)
{
	//�R���g���[�������삳�ꂽ���ɌĂ΂��
	//�G�t�F�N�^�[���ɕω������p�����[�^��ݒ肷�鏈�����s��
	
	int		tag = control->getTag();
	float	value = control->getValue();
	const char	*text = NULL;
	
	//�X���C�_�[�Őݒ�o����l�ɂ͐����l�����Ȃ��̏����ȉ���؂�̂Ă�
	if ( control->isTypeOf("CMySlider") )
	{
		//�G�R�[�t�B���^�C�R���C�U������
		if ( !(tag >= kAudioUnitCustomProperty_Band1 && tag <= kAudioUnitCustomProperty_Band5) ) {
			value = (int)value;
		}
	}
	
	//�e�L�X�g�{�b�N�X�̏ꍇ�͐��l�ɕϊ�����
	if ( control->isTypeOf("CMyTextEdit") )
	{
		CMyTextEdit		*textedit = reinterpret_cast<CMyTextEdit*> (control);
		text = textedit->getText();
		sscanf(text, "%f", &value);
		if ( tag == kAudioUnitCustomProperty_LoopPoint ) {
			value = ((int)value / 16) * 16;
		}
		control->setValue(value);
		control->bounceValue();		//�l��͈͓��Ɋۂ߂�
		value = control->getValue();
		if ( tag == kAudioUnitCustomProperty_LoopPoint ) {
			value = ((int)value / 16) * 9;
		}
	}
	
	//0-2�̒l��Ɋg������
	if ( tag == kParam_velocity )
	{
		value *= 2;
	}

	if ( tag < kAudioUnitCustomProperty_Begin )
	{
        // �p�����[�^�̑���
		efxAcc->SetParameter( this, tag%1000, value );
	}
	else if ( tag < kControlCommandsFirst ) {
        // �v���p�e�B�n�̑���
		int	propertyId = ((tag-kAudioUnitCustomProperty_Begin)%1000)+kAudioUnitCustomProperty_Begin;
		switch (propertyId) {
			case kAudioUnitCustomProperty_ProgramName:
				if ( text ) {
					efxAcc->SetCStringProperty( propertyId, text );
				}
				break;
			default:
				efxAcc->SetPropertyValue( propertyId, value );
				break;
		}
	}
	else {
        // �P�@�\�̃{�^������
		switch (tag) {
			case kControlButtonCopy:
				if ( value > 0 ) {
					copyFIRParamToClipBoard();
				}
				break;
				
			case kControlButtonPreemphasis:
				break;
				
			case kControlButtonUnload:
				if ( value > 0 ) {
					BRRData		brr;
					brr.data=NULL;
					efxAcc->SetBRRData( &brr );
				}
				break;
			
			case kControlButtonLoad:
				if ( value > 0 ) {
					char	path[PATH_LEN_MAX];
					bool	isSelected;
					isSelected = getLoadFile(path, PATH_LEN_MAX, "");
					if ( isSelected ) {
						loadToCurrentProgram(path);
					}
				}
				break;
				
            case kControlButtonKhaos:
                if ( value > 0 ) {
                    loadToCurrentProgramFromKhaos();
                }
                break;
                
			case kControlButtonSave:
				if ( value > 0 ) {
					//�T���v���f�[�^�̑��݊m�F
					BRRData		brr;
					if (efxAcc->GetBRRData(&brr) == false) break;
					//�f�[�^��������ΏI������
					if (brr.data == NULL) break;
					
					//�f�t�H���g�t�@�C�����̍쐬
					char	pgname[PROGRAMNAME_MAX_LEN];
					char	defaultName[PROGRAMNAME_MAX_LEN];
					efxAcc->GetCStringProperty(kAudioUnitCustomProperty_ProgramName, pgname, PROGRAMNAME_MAX_LEN);
					if ( pgname[0] == 0 || strlen(pgname) == 0 ) {
						snprintf(defaultName, PROGRAMNAME_MAX_LEN-1, "program_%03d.brr", 
								 (int)efxAcc->GetPropertyValue(kAudioUnitCustomProperty_EditingProgram) );
					}
					else {
						snprintf(defaultName, PROGRAMNAME_MAX_LEN-1, "%s.brr", pgname);
					}
					//�ۑ��t�@�C���_�C�A���O��\��
					char	path[PATH_LEN_MAX];
					bool	isSelected;
					isSelected = getSaveFile(path, PATH_LEN_MAX, defaultName, "Save brr Sample To...");
					if ( isSelected ) {
						saveFromCurrentProgram(path);
					}
				}
				break;
				
			case kControlButtonSaveXI:
				if ( value > 0 ) {
					//�T���v���f�[�^�̑��݊m�F
					BRRData		brr;
					if ( efxAcc->GetBRRData(&brr) == false ) break;
					//�f�[�^��������ΏI������
					if (brr.data == NULL) break;
					
					//�\�[�X�t�@�C�������݂��邩�m�F����
					bool	existSrcFile = false;
					char	srcPath[PATH_LEN_MAX];
					efxAcc->GetFilePathProperty(kAudioUnitCustomProperty_SourceFileRef, srcPath, PATH_LEN_MAX);
					if ( strlen(srcPath) > 0 ) {
						//�I�[�f�B�I�t�@�C���ł��邩�m�F����
						AudioFile	srcFile(srcPath,false);
						if (srcFile.IsVarid()) {
							existSrcFile = true;
						}
					}
					//�\�[�X�t�@�C����񂪖�����ΑI���_�C�A���O���o��
					/*
					if ( existSrcFile == false ) {
						if ( getLoadFile(srcPath, PATH_LEN_MAX, "Where is Source File?") ) {
							efxAcc->SetSourceFilePath(srcPath);
						}
					}
					*/
					//�f�t�H���g�t�@�C�����̍쐬
					char	pgname[PROGRAMNAME_MAX_LEN];
					char	defaultName[PROGRAMNAME_MAX_LEN];
					efxAcc->GetCStringProperty(kAudioUnitCustomProperty_ProgramName, pgname, PROGRAMNAME_MAX_LEN);
					if ( pgname[0] == 0 || strlen(pgname) == 0 ) {
						snprintf(defaultName, PROGRAMNAME_MAX_LEN-1, "program_%03d.xi", 
								 (int)efxAcc->GetPropertyValue(kAudioUnitCustomProperty_EditingProgram) );
					}
					else {
						snprintf(defaultName, PROGRAMNAME_MAX_LEN-1, "%s.xi", pgname);
					}
					//�ۑ��t�@�C���_�C�A���O��\��
					char	savePath[PATH_LEN_MAX];
					bool	isSelected;
					isSelected = getSaveFile(savePath, PATH_LEN_MAX, defaultName, "Export XI Inst To...");
					if ( isSelected ) {
						saveFromCurrentProgramToXI(savePath);
					}
				}
				break;
				
			case kControlButtonAutoSampleRate:
				if ( value > 0 ) {
					autocalcCurrentProgramSampleRate();
				}
				break;
				
			case kControlButtonAutoKey:
				if ( value > 0 ) {
					autocalcCurrentProgramBaseKey();
				}
				break;
				
			case kControlButtonChangeLoopPoint:
			{
				int	loopPoint = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_LoopPoint);
				loopPoint -= value * 9;
				efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LoopPoint, loopPoint);
				break;
			}
				
			case kControlButtonChangeProgram:
			{
				int	programNo = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_EditingProgram);
				programNo -= value;
				efxAcc->SetPropertyValue(kAudioUnitCustomProperty_EditingProgram, programNo);
				break;
			}
			
			case kControlSelectTrack16:
			case kControlSelectTrack15:
			case kControlSelectTrack14:
			case kControlSelectTrack13:
			case kControlSelectTrack12:
			case kControlSelectTrack11:
			case kControlSelectTrack10:
			case kControlSelectTrack9:
			case kControlSelectTrack8:
			case kControlSelectTrack7:
			case kControlSelectTrack6:
			case kControlSelectTrack5:
			case kControlSelectTrack4:
			case kControlSelectTrack3:
			case kControlSelectTrack2:
			case kControlSelectTrack1:
				efxAcc->SetPropertyValue( kAudioUnitCustomProperty_EditingChannel, 15-(tag-kControlSelectTrack16) );
				break;
				
			case kControlBankDBtn:
			case kControlBankCBtn:
			case kControlBankBBtn:
			case kControlBankABtn:
				efxAcc->SetPropertyValue( kAudioUnitCustomProperty_Bank, 3-(tag-kControlBankDBtn) );
				break;
                
            case kControlButtonOpenRecordingSettings:
                if ( value > 0 ) {
                    if (recordWindow) {
                        getFrame()->setModalView(recordWindow);
                    }
				}
                break;
                
			default:
				break;
		}
	}
}

//-----------------------------------------------------------------------------
bool C700GUI::attached(CView* view)
{
	return CViewContainer::attached(view);
}

//-----------------------------------------------------------------------------
bool C700GUI::removed(CView* parent)
{
	return CViewContainer::removed(parent);
}

//-----------------------------------------------------------------------------
CMessageResult C700GUI::notify(CBaseObject* sender, const char* message)
{
	return CViewContainer::notify(sender, message);
}

//-----------------------------------------------------------------------------
CControl *C700GUI::FindControlByTag( long tag )
{
    auto itr = mCntl.find(tag);
    if (itr != mCntl.end()) {
        if (itr->second->getTag() == tag) {
            return mCntl[tag];
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------
void C700GUI::copyFIRParamToClipBoard()
{
	// FIR�p�����[�^���N���b�v�{�[�h�ɃR�s�[
	CTextLabel	*textView = reinterpret_cast<CTextLabel*> (FindControlByTag(kControlXMSNESText));
	if ( textView == NULL ) return;
	
	const char	*text = textView->getText();
	
#if MAC
	OSStatus err = noErr;
	PasteboardRef theClipboard;
	err = PasteboardCreate( kPasteboardClipboard, &theClipboard );
	err = PasteboardClear( theClipboard );
	
	CFDataRef   data = CFDataCreate( kCFAllocatorDefault, (UInt8*)text, (strlen(text)) * sizeof(char) );
	err = PasteboardPutItemFlavor( theClipboard, (PasteboardItemID)1, kUTTypeUTF8PlainText, data, 0 );
	
	CFRelease(theClipboard);
	CFRelease( data );
#else
	//Windows�̃R�s�[����
	HGLOBAL hg;
	PTSTR	strMem;

	if (!OpenClipboard(NULL)) return;
	EmptyClipboard();

	hg = GlobalAlloc(GHND | GMEM_SHARE, strlen(text)+1);
	strMem = (PTSTR)GlobalLock(hg);
	lstrcpy(strMem, text);
	GlobalUnlock(hg);

	SetClipboardData(CF_TEXT, hg);

	CloseClipboard();
#endif
}

//-----------------------------------------------------------------------------
bool C700GUI::loadToCurrentProgram( const char *path )
{
	RawBRRFile		brrfile(path,false);
	PlistBRRFile	plbrrfile(path,false);
	AudioFile		audiofile(path,false);
	SPCFile			spcfile(path,false);
	
	brrfile.Load();
	if ( brrfile.IsLoaded() ) {
		return loadToCurrentProgramFromBRR( &brrfile );
	}
	
	plbrrfile.Load();
	if ( plbrrfile.IsLoaded() ) {
		return loadToCurrentProgramFromPlistBRR( &plbrrfile );
	}
	
	audiofile.Load();
	if ( audiofile.IsLoaded() ) {
		return loadToCurrentProgramFromAudioFile( &audiofile );
	}
	
	spcfile.Load();
	if ( spcfile.IsLoaded() ) {
		return loadToCurrentProgramFromSPC( &spcfile );
	}
	
	return false;
}

//-----------------------------------------------------------------------------
bool C700GUI::loadToCurrentProgramFromKhaos()
{
    // �����̏�����
    CMersenneTwister    mersennetwister;
    mersennetwister.init_genrand((unsigned)time(NULL));
    // �T���v���������߂�
    int     blockNumMin = 1;
    int     blockNumMax = 4;     // ��
    int     blockNum = mersennetwister.genrand_N(blockNumMax) + blockNumMin;
    BRRData brrData;
    brrData.size = blockNum * 9;    // 1�u���b�N=9�o�C�g
    brrData.data = new unsigned char[brrData.size];
    for (int i=0; i<brrData.size; i++) {
        brrData.data[i] = mersennetwister.genrand_N(256);
        if ((i % 9) == 0) {
            brrData.data[i] &= 0xfe;
            if (brrData.data[i] >= 0xd0) {
                brrData.data[i] &= 0x0f;
                brrData.data[i] |= 0xc0;
            }
        }
    }
    brrData.data[(blockNum-1)*9] |= 0x01;       // END bit
    efxAcc->SetBRRData(&brrData);
    
    // ���O�����߂�
    char    instName[PROGRAMNAME_MAX_LEN];
    int     nameLength = mersennetwister.genrand_N(brrData.size-9)+9;
    if (nameLength > PROGRAMNAME_MAX_LEN-1) {
        nameLength = PROGRAMNAME_MAX_LEN-1;
    }
    for (int i=0; i<nameLength; i++) {
        instName[i] = 0x20 + mersennetwister.genrand_N(0x5f);
    }
    instName[nameLength] = 0;
    efxAcc->SetCStringProperty( kAudioUnitCustomProperty_ProgramName, instName );
    
    delete [] brrData.data;
    
    efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LoopPoint,0);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Loop,		1.0f);
    efxAcc->SetPropertyValue(kAudioUnitCustomProperty_BaseKey,	69);
    efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Rate,	32000);
    return true;
}

//-----------------------------------------------------------------------------
bool C700GUI::loadToCurrentProgramFromBRR( RawBRRFile *file )
{
	//RawBRRFile����f�[�^���擾���ăG�t�F�N�^���֔��f
	InstParams	inst = *(file->GetLoadedInst());
	
	efxAcc->SetBRRData(inst.getBRRData());
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LoopPoint,inst.lp);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Loop,		inst.isLoop() ? 1.0f:.0f);
	
	unsigned int	hasFlg = file->GetHasFlag();
	if ( hasFlg & HAS_PGNAME ) efxAcc->SetCStringProperty( kAudioUnitCustomProperty_ProgramName, inst.pgname );
	if ( hasFlg & HAS_RATE ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Rate,		inst.rate);
	else {
		if ( inst.isLoop() ) {
			double	samplerate = 32000.0;
			short	*buffer;
			int		pitch;
			int		length;
			buffer = new short[inst.brrSamples()*2];
			brrdecode(inst.brrData(), buffer, inst.lp, 2);
			length = (inst.brrSamples()-inst.brrLpSamples())*2;
			pitch = estimatebasefreq(buffer+inst.brrLpSamples(), length);
			if (pitch > 0) {
				samplerate = length/(double)pitch * 440.0*pow(2.0,-9.0/12);
			}
			delete[] buffer;
			efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Rate,	samplerate);
			inst.basekey = 60;
			hasFlg |= HAS_BASEKEY;
		}
	}	
	if ( hasFlg & HAS_BASEKEY ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_BaseKey,	inst.basekey);
	if ( hasFlg & HAS_LOWKEY ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LowKey,	inst.lowkey);
	if ( hasFlg & HAS_HIGHKEY ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_HighKey,	inst.highkey);
	if ( hasFlg & HAS_AR ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_AR,	inst.ar);
	if ( hasFlg & HAS_DR ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_DR,	inst.dr);
	if ( hasFlg & HAS_SL ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_SL,	inst.sl);
	if ( hasFlg & HAS_SR1 ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_SR1,	inst.sr1);
	if ( hasFlg & HAS_SR2 ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_SR2,	inst.sr2);
	if ( hasFlg & HAS_VOLL ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_VolL,	inst.volL);
	if ( hasFlg & HAS_VOLR ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_VolR,	inst.volR);
	if ( hasFlg & HAS_ECHO ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Echo,	inst.echo ? 1.0:0.f);
	if ( hasFlg & HAS_BANK ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Bank,	inst.bank);
	if ( hasFlg & HAS_ISEMPHASIZED ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_IsEmaphasized,inst.isEmphasized ? 1.0f:.0f);
	if ( hasFlg & HAS_SOURCEFILE ) {
		if ( strlen(inst.sourceFile) ) {
			efxAcc->SetFilePathProperty( kAudioUnitCustomProperty_SourceFileRef, inst.sourceFile );
		}
	}
    if ( hasFlg & HAS_SUSTAINMODE ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_SustainMode,inst.sustainMode ? 1.0f:.0f);
    if ( hasFlg & HAS_MONOMODE ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_MonoMode,inst.monoMode ? 1.0f:.0f);
    if ( hasFlg & HAS_PORTAMENTOON ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_PortamentoOn,inst.portamentoOn ? 1.0f:.0f);
    if ( hasFlg & HAS_PORTAMENTORATE ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_PortamentoRate,inst.portamentoRate);
    if ( hasFlg & HAS_NOTEONPRIORITY ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_NoteOnPriority,inst.noteOnPriority);
    if ( hasFlg & HAS_RELEASEPRIORITY ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_ReleasePriority,inst.releasePriority);
    if ( hasFlg & HAS_PMON ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_PitchModulationOn,inst.pmOn ? 1.0f:.0f);
    if ( hasFlg & HAS_NOISEON ) efxAcc->SetPropertyValue(kAudioUnitCustomProperty_NoiseOn,inst.noiseOn ? 1.0f:.0f);
	
	return true;
}

//-----------------------------------------------------------------------------
bool C700GUI::loadToCurrentProgramFromPlistBRR( PlistBRRFile *file )
{
    bool result = efxAcc->SetPlistBRRFileData(file);
    efxAcc->SetPropertyValue(kAudioUnitCustomProperty_SustainMode, .0f);
	return result;
}

//-----------------------------------------------------------------------------
bool C700GUI::loadToCurrentProgramFromAudioFile( AudioFile *file )
{
	AudioFile::InstData	inst;
	short		*wavedata;
	long		numSamples;
	BRRData		brr;
	int			looppoint;
	bool		loop;
	int			pad;
	
	//�g�`�t�@�C���̏����擾
	wavedata	= file->GetAudioData();
	numSamples	= file->GetLoadedSamples();
	file->GetInstData( &inst );
	
	if ( IsPreemphasisOn() ) {
		emphasis(wavedata, numSamples);
	}
	
	brr.data = new unsigned char[numSamples/16*9+18];
	if (inst.loop) {
		numSamples = inst.lp_end;
	}
	looppoint = (inst.lp + 15)/16*9;
	loop = inst.loop ? true:false;
	pad = 16-(numSamples % 16);
	brr.size = brrencode(wavedata, brr.data, numSamples, loop, (looppoint/9)*16, pad);
	looppoint += pad/16 * 9;
	
	//�g�`�f�[�^��ݒ�
	efxAcc->SetBRRData(&brr);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Rate,		inst.srcSamplerate);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_BaseKey,	inst.basekey);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LowKey,	inst.lowkey);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_HighKey,	inst.highkey);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LoopPoint,	looppoint);
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Loop,		loop ? 1.0f:.0f);
	
	//���g�`�f�[�^�̏����Z�b�g����
	efxAcc->SetFilePathProperty( kAudioUnitCustomProperty_SourceFileRef, file->GetFilePath() );
	efxAcc->SetPropertyValue(kAudioUnitCustomProperty_IsEmaphasized,	IsPreemphasisOn() ? 1.0f:.0f);
	
	//�g���q���������t�@�C�������v���O�������ɐݒ肷��
	char	pgname[256];
	getFileNameDeletingPathExt(file->GetFilePath(), pgname, 256);
	efxAcc->SetCStringProperty( kAudioUnitCustomProperty_ProgramName, pgname );
	
	delete[] brr.data;
	
	return true;
}

//-----------------------------------------------------------------------------
bool C700GUI::loadToCurrentProgramFromSPC( SPCFile *file )
{
	BRRData	brr;
	double	samplerate;
	int		looppoint;
	bool	loop;
	int		pitch;
	int		length;
	short	*buffer;
	int		cEditNum=0;
	
	for (int i=0; i<128; i++) {
		brr.data = file->GetSampleIndex(i, &brr.size);
		if ( brr.data == NULL ) continue;
		
		looppoint = file->GetLoopSizeIndex( i );
		loop = looppoint >= 0 ? true:false;
		
		samplerate = 32000;
		if (loop) {
			buffer = new short[brr.samples()*2];
			brrdecode(brr.data, buffer, looppoint, 2);
			length = ((brr.size-looppoint)*2)/9*16;
			pitch = estimatebasefreq(buffer+looppoint/9*16, length);
			if (pitch > 0) {
				samplerate = length/(double)pitch * 440.0*pow(2.0,-9.0/12);
			}
			delete[] buffer;
		}
		
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_EditingProgram, cEditNum);
		efxAcc->SetBRRData(&brr);
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Rate, samplerate);
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_BaseKey, 60);
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LowKey, 0);
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_HighKey, 127);
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_LoopPoint, looppoint);
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Loop, loop?1.0f:.0f);
		
		//�t�@�C���l�[���̏���
		char	pgname[256];
		char	filename[256];
		getFileNameDeletingPathExt(file->GetFilePath(), filename, 256);
		snprintf(pgname, 255, "%s#%02x", filename, i);
		efxAcc->SetCStringProperty(kAudioUnitCustomProperty_ProgramName, pgname);

		cEditNum++;
	}
	//efxAcc->SetParameter(this, kParam_engine, 1);
	
	return true;
}

//-----------------------------------------------------------------------------
bool C700GUI::getLoadFile( char *path, int maxLen, const char *title )
{
#if VSTGUI_NEW_CFILESELECTOR
	CFileExtension	brrType("AddmusicM(Raw) BRR Sample", "brr");
	CFileExtension	aiffType("AIFF File", "aif", "audio/aiff", 'AIFF');
	CFileExtension	aifcType("AIFC File", "aif", "audio/aiff", 'AIFC');
	CFileExtension	waveType("Wave File", "wav", "audio/wav");
	CFileExtension	sd2Type("Sound Designer II File", "sd2", 0, 'Sd2f');
	CFileExtension	cafType("CoreAudio File", "caf", 0, 'caff');
	CFileExtension	spcType("SPC File", "spc");
	CNewFileSelector* selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectFile);
	if (selector)
	{
		selector->addFileExtension(brrType);
		selector->addFileExtension(aiffType);
		selector->addFileExtension(aifcType);
		selector->addFileExtension(waveType);
		selector->addFileExtension(sd2Type);
		//selector->addFileExtension(cafType);
		selector->addFileExtension(spcType);
		if ( title ) selector->setTitle(title);
		selector->runModal();
		if ( selector->getNumSelectedFiles() > 0 ) {
			const char *url = selector->getSelectedFile(0);
			strncpy(path, url, maxLen-1);
			selector->forget();
			return true;
		}
		selector->forget();
		return false;
	}
#else
	VstFileType brrType("AddmusicM(Raw) BRR Sample", "", "brr", "brr");
	VstFileType waveType("Wave File", "WAVE", "wav", "wav",  "audio/wav", "audio/x-wav");
	VstFileType spcType("SPC File", "", "spc", "spc");
	VstFileType types[] = {brrType, waveType, spcType};

//	CFileSelector OpenFile( ((AEffGUIEditor *)getEditor())->getEffect() );
	CFileSelector OpenFile(0);
	VstFileSelect Filedata;
	memset(&Filedata, 0, sizeof(VstFileSelect));
	Filedata.command=kVstFileLoad;
	Filedata.type= kVstFileType;
	strncpy(Filedata.title, title, maxLen-1 );
	Filedata.nbFileTypes=3;
	Filedata.fileTypes=types;
	Filedata.returnPath= path;
	Filedata.initialPath = 0;
	Filedata.future[0] = 0;
	if (OpenFile.run(&Filedata) > 0) {
		return true;
	}
#endif
	return false;
}

//-----------------------------------------------------------------------------
bool C700GUI::getSaveFile( char *path, int maxLen, const char *defaultName, const char *title )
{
#if VSTGUI_NEW_CFILESELECTOR
	CFileExtension	brrType("AddmusicM BRR Sample", "brr");
	CFileExtension	xiType("FastTracker II Instruments", "xi");
	CNewFileSelector* selector = CNewFileSelector::create(getFrame(), CNewFileSelector::kSelectSaveFile);
	if (selector)
	{
		char	ext[8];
		getFileNameExt(defaultName, ext, sizeof(ext));
		if ( strncmp(ext, "brr", 3) == 0 ) selector->addFileExtension(brrType);
		if ( strncmp(ext, "xi", 2) == 0 ) selector->addFileExtension(xiType);
		if ( defaultName ) selector->setDefaultSaveName(defaultName);
		if ( title ) selector->setTitle(title);
		selector->runModal();
		if ( selector->getNumSelectedFiles() > 0 ) {
			const char *url = selector->getSelectedFile(0);
			strncpy(path, url, maxLen-1);
			selector->forget();
			return true;
		}
		selector->forget();
		return false;
	}
#else
	VstFileType brrType("AddmusicM BRR Sample", "", "brr", "brr");
	VstFileType xiType("FastTracker II Instruments", "", "xi", "xi");
	//VstFileType types[] = {brrType, xiType};

//	CFileSelector OpenFile( ((AEffGUIEditor *)getEditor())->getEffect() );
	CFileSelector OpenFile(0);
	VstFileSelect Filedata;
	memset(&Filedata, 0, sizeof(VstFileSelect));
	Filedata.command=kVstFileSave;
	Filedata.type= kVstFileType;
	strncpy(Filedata.title, title, maxLen-1 );
	
	char	ext[8];
	getFileNameExt(defaultName, ext, sizeof(ext));
	if ( strncmp(ext, "brr", 3) == 0 ) {
		Filedata.nbFileTypes=1;
		Filedata.fileTypes=&brrType;
	}
	if ( strncmp(ext, "xi", 2) == 0 ) {
		Filedata.nbFileTypes=1;
		Filedata.fileTypes=&xiType;
	}
	//Filedata.nbFileTypes=1;
	//Filedata.fileTypes=types;
	Filedata.returnPath= path;
	Filedata.initialPath = 0;
	Filedata.future[0] = 0;
	if (OpenFile.run(&Filedata) > 0) {
		return true;
	}
#endif
	return false;
}

//-----------------------------------------------------------------------------
void C700GUI::saveFromCurrentProgram(const char *path)
{
	RawBRRFile	*file;
	
	if ( efxAcc->CreateBRRFileData(&file) ) {
		file->SetFilePath( path );
		file->Write();
		delete file;
	}
}

//-----------------------------------------------------------------------------
void C700GUI::saveFromCurrentProgramToXI(const char *path)
{
	XIFile	*file;
	
	if ( efxAcc->CreateXIFileData(&file) ) {
		file->SetFilePath( path );
		file->Write();
		delete file;
	}
}

//-----------------------------------------------------------------------------
void C700GUI::autocalcCurrentProgramSampleRate()
{
	// �g�`�̃T���v�����O���[�g�����o
	int		looppoint;
	int		key;
	double	samplerate;
	BRRData	brr;
	short	*buffer;
	int		pitch, length;
	
	if ( efxAcc->GetBRRData( &brr ) == false ) return;
	
	if (brr.data == NULL) return;
	
	looppoint = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_LoopPoint);
	if (looppoint == brr.size) {
		looppoint = 0;
	}
	
	key = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_BaseKey);
	
	buffer = new short[brr.samples()];
	brrdecode(brr.data, buffer, 0, 0);
	length = (brr.size-looppoint)/9*16;
	pitch = estimatebasefreq(buffer+looppoint/9*16, length);
	if (pitch > 0) {
		samplerate = length/(double)pitch * 440.0*pow(2.0,(key-69.0)/12);
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_Rate, samplerate);
	}
	delete[] buffer;
}

//-----------------------------------------------------------------------------
void C700GUI::autocalcCurrentProgramBaseKey()
{
	// �g�`�̊�{�m�[�g�����o
	int		looppoint;
	int		key;
	double	samplerate, freq;
	BRRData	brr;
	short	*buffer;
	int		pitch, length;
	
	if (efxAcc->GetBRRData( &brr ) == false) return;
	
	if (brr.data == NULL) return;
	
	looppoint = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_LoopPoint);
	if (looppoint == brr.size) {
		looppoint = 0;
	}	
	
	samplerate = efxAcc->GetPropertyValue(kAudioUnitCustomProperty_Rate);
	
	buffer = new short[brr.samples()];
	brrdecode(brr.data, buffer, 0, 0);
	length = (brr.size-looppoint)/9*16;
	pitch = estimatebasefreq(buffer+looppoint/9*16, length);
	if (pitch > 0) {
		freq = samplerate / (length/(double)pitch);
		key = log(freq)*17.312-35.874;
		efxAcc->SetPropertyValue(kAudioUnitCustomProperty_BaseKey, key);
	}
	delete[] buffer;
}

//-----------------------------------------------------------------------------
bool C700GUI::IsPreemphasisOn()
{
	CControl	*cntl = FindControlByTag(kControlButtonPreemphasis);
	return cntl->getValue()>0.5f ? true:false;
}

