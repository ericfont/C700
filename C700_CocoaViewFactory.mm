//
//  C700_CocoaViewFactory.m
//  C700
//
//  Created by osoumen on 12/10/01.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <CAAUParameter.h>
#import "C700_CocoaViewFactory.h"
#import "C700Edit.h"
#include "C700Properties.h"

@implementation C700_CocoaViewFactory

// version 0
- (unsigned) interfaceVersion {
	return 0;
}

// string description of the Cocoa UI
- (NSString *) description {
	return @" Cocoa View";
}

- (void)dealloc {
	[super dealloc];
}

- (NSView *)uiViewForAudioUnit:(AudioUnit)inAU withSize:(NSSize)inPreferredSize
{
	NSRect r;
	r.origin.x = 0;
	r.origin.y = 0;
	r.size.width = 200;
	r.size.height = 200;
	
    C700_CocoaView *returnView = [[C700_CocoaView alloc] initWithFrame:r audioUnit:inAU];
	[returnView setEditorFrame];
	
    return [returnView autorelease];
}

@end

void EventListenerDispatcher(
void *						inCallbackRefCon,
void *						inObject,
const AudioUnitEvent *		inEvent,
UInt64						inEventHostTime,
AudioUnitParameterValue		inParameterValue
)
{
	C700Edit *editor = (C700Edit *)inCallbackRefCon;
	if ( inEvent->mEventType == kAudioUnitEvent_ParameterValueChange ) {
		editor->setParameter(inEvent->mArgument.mParameter.mParameterID, inParameterValue);
	}
	if ( inEvent->mEventType == kAudioUnitEvent_PropertyChange ) {
		AudioUnitPropertyID	propertyId = inEvent->mArgument.mProperty.mPropertyID;
		float		value;
		char		outDataPtr[16];
		UInt32		outDataSize=16;
		
        AudioUnitGetProperty((AudioUnit)editor->getEffect(), propertyId,
                             kAudioUnitScope_Global, 0, &outDataPtr, &outDataSize);
		
        auto it = editor->mPropertyParams.find(propertyId);
        if (it != editor->mPropertyParams.end()) {
            switch (it->second.dataType) {
                case propertyDataTypeFloat32:
                    value = *((Float32*)outDataPtr);
                    editor->setParameter( propertyId, value );
                    break;
                case propertyDataTypeDouble:
                    value = *((double*)outDataPtr);
                    editor->setParameter( propertyId, value );
                    break;
                case propertyDataTypeInt32:
                    value = *((int*)outDataPtr);
                    editor->setParameter( propertyId, value );
                    break;
                case propertyDataTypeBool:
                    value = *((bool*)outDataPtr);
                    editor->setParameter( propertyId, value );
                    break;
                case propertyDataTypeStruct:
                    editor->setParameter( propertyId, outDataPtr );
                    break;
                case propertyDataTypeString:
                case propertyDataTypeFilePath:
                case propertyDataTypeVariableData:
                case propertyDataTypePointer:
                {
                    char **ptr = (char**)outDataPtr;
                    editor->setParameter( propertyId, *ptr );
                    break;
                }
            }
        }
	}
}

@implementation C700_CocoaView

- (C700_CocoaView*)initWithFrame:(NSRect)frameRect audioUnit:(AudioUnit)inAU
{
	C700_CocoaView	*view = [super initWithFrame:frameRect];
	
	mAU = inAU;
#ifndef __LP64__
	//CFrame::setCocoaMode(true);
#endif
	editor = new C700Edit(inAU);
	
	if ( editor ) {
		editor->open(self);
	}
	
	//�^�C�}�[�̐ݒ�
	timer = [NSTimer scheduledTimerWithTimeInterval:1.0/60
											 target:self
										   selector:@selector(respondToEventTimer:)
										   userInfo:nil
											repeats:YES];	
	
	//�p�����[�^�[���X�i�[�̓o�^
	[self _addListeners];
	
	//�ݒ�l�̔��f
	[self _synchronizeUIWithParameterValues];
	
	return view;
}

- (void)_addListeners
{
	//�C�x���g���X�i�[�̍쐬
	NSAssert(	AUEventListenerCreate(	EventListenerDispatcher, editor, 
								 CFRunLoopGetCurrent(), 
								kCFRunLoopDefaultMode, 1.0/60, 1.0/60,
								 &mEventListener	) == noErr,
			 @"[CocoaView _addListeners] AUListenerCreate()");
	
	//�p�����[�^���X�i�[�̓o�^
    for (int i = 0; i < kNumberOfParameters; ++i) {
		AudioUnitParameter parameter = { mAU, i, kAudioUnitScope_Global, 0 };
        NSAssert (	AUListenerAddParameter (mEventListener, editor, &parameter) == noErr,
				  @"[CocoaView _addListeners] AUListenerAddParameter()");
    }
	
	//�G�t�F�N�^�[�A�N�Z�b�T�̍쐬
	efxAcc = new EfxAccess( mAU );
	efxAcc->SetEventListener(mEventListener);
	editor->SetEfxAccess(efxAcc);
	
	//�v���p�e�B���X�i�[�̓o�^
	for (int i=0; i<kNumberOfProperties; ++i) {
		AudioUnitProperty property = { mAU, kAudioUnitCustomProperty_Begin+i, kAudioUnitScope_Global, 0 };
		AudioUnitEvent	event;
		event.mEventType = kAudioUnitEvent_PropertyChange;
		event.mArgument.mProperty = property;
        NSAssert (	AUEventListenerAddEventType (mEventListener, editor, &event) == noErr,
				  @"[CocoaView _addListeners] AUListenerAddParameter()");
		
		//�����l�𔽉f������
		//AUEventListenerNotify(mEventListener, editor, &event);
		EventListenerDispatcher(editor, editor, &event, 0, 0);
    }
}

- (void)_removeListeners
{
	for (int i=0; i<kNumberOfProperties; ++i) {
		AudioUnitProperty property = { mAU, i+kAudioUnitCustomProperty_Begin, kAudioUnitScope_Global, 0 };
		AudioUnitEvent	event;
		event.mEventType = kAudioUnitEvent_PropertyChange;
		event.mArgument.mProperty = property;
        NSAssert (	AUEventListenerRemoveEventType (mEventListener, editor, &event) == noErr,
				  @"[CocoaView _removeListeners] AUEventListenerRemoveEventType()");
    }
	for (int i = 0; i < kNumberOfParameters; ++i) {
		AudioUnitParameter parameter = { mAU, i, kAudioUnitScope_Global, 0 };
        NSAssert (	AUListenerRemoveParameter(mEventListener, editor, &parameter) == noErr,
				  @"[CocoaView _removeListeners] AUListenerRemoveParameter()");
    }
	NSAssert (	AUListenerDispose(mEventListener) == noErr,
			  @"[CocoaView _removeListeners] AUListenerDispose()");
}

- (void)_synchronizeUIWithParameterValues
{
	Float32 value;
    
    for (int i = 0; i < kNumberOfParameters; ++i)
	{
		//�ő�l�A�ŏ��l�A�f�t�H���g�l���R���g���[���ɔ��f
		CAAUParameter tParam(mAU, i, kAudioUnitScope_Global, 0);
		editor->SetParameterInfo( i, tParam.ParamInfo().minValue, tParam.ParamInfo().maxValue, tParam.ParamInfo().defaultValue );
		
        if ((i != kParam_program) && ((i < kParam_program_2) || (i > kParam_program_16))) {
            AudioUnitParameter parameter = { mAU, i, kAudioUnitScope_Global, 0 };
            // only has global parameters
            NSAssert (	AudioUnitGetParameter(mAU, parameter.mParameterID, kAudioUnitScope_Global, 0, &value) == noErr,
                      @"[CocoaView synchronizeUIWithParameterValues] (x.1)");
            NSAssert (	AUParameterSet (mEventListener, self, &parameter, value, 0) == noErr,
                      @"[CocoaView synchronizeUIWithParameterValues] (x.2)");
            NSAssert (	AUParameterListenerNotify (mEventListener, self, &parameter) == noErr,
                      @"[CocoaView synchronizeUIWithParameterValues] (x.3)");
		}
    }
}

- (void)respondToEventTimer:(NSTimer*)inTimer
{
	if (editor) {
		editor->doIdleStuff();
	}
}

- (void)setEditorFrame
{
	CRect fsize;
	if ( editor ) {
		fsize = editor->getFrame()->getViewSize(fsize);
	}
	NSRect r;
	r.origin.x = fsize.left;
	r.origin.y = fsize.top;
	r.size.width = fsize.getWidth();
	r.size.height = fsize.getHeight();
	[self setFrame:r];	
}

- (void)dealloc
{
	delete efxAcc;
	
	//�p�����[�^���X�i�[�̍폜
	[self _removeListeners];
	
	//�^�C�}�[�̒�~
	if ( timer ) {
		[timer invalidate];
	}
	
	if (editor) {
		editor->close();
		delete editor;
		editor = NULL;
	}
	[super dealloc];
}

@end
