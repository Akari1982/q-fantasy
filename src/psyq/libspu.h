#pragma once

#include "typedef.h"

#include "ofBaseApp.h"
#include "ofSoundStream.h"

#define SPU_OFF 0x0
#define SPU_ON  0x1

#define SPU_VOICE_VOLL          (0x01 <<  0)
#define SPU_VOICE_VOLR          (0x01 <<  1)
#define SPU_VOICE_VOLMODEL      (0x01 <<  2)
#define SPU_VOICE_VOLMODER      (0x01 <<  3)
#define SPU_VOICE_PITCH         (0x01 <<  4)
#define SPU_VOICE_NOTE          (0x01 <<  5)
#define SPU_VOICE_SAMPLE_NOTE   (0x01 <<  6)
#define SPU_VOICE_WDSA          (0x01 <<  7)
#define SPU_VOICE_ADSR_AMODE    (0x01 <<  8)
#define SPU_VOICE_ADSR_SMODE    (0x01 <<  9)
#define SPU_VOICE_ADSR_RMODE    (0x01 << 10)
#define SPU_VOICE_ADSR_AR       (0x01 << 11)
#define SPU_VOICE_ADSR_DR       (0x01 << 12)
#define SPU_VOICE_ADSR_SR       (0x01 << 13)
#define SPU_VOICE_ADSR_RR       (0x01 << 14)
#define SPU_VOICE_ADSR_SL       (0x01 << 15)
#define SPU_VOICE_LSAX          (0x01 << 16)
#define SPU_VOICE_ADSR_ADSR1    (0x01 << 17)
#define SPU_VOICE_ADSR_ADSR2    (0x01 << 18)

#define SPU_REV_MODE            (0x01 <<  0)
#define SPU_REV_DEPTHL          (0x01 <<  1)
#define SPU_REV_DEPTHR          (0x01 <<  2)
#define SPU_REV_DELAYTIME       (0x01 <<  3)
#define SPU_REV_FEEDBACK        (0x01 <<  4)

struct SpuVolume
{
    s16 left;
    s16 right;
};

struct SpuReverbAttr
{
    u32 mask;
    s32 mode;
    SpuVolume depth;
    s32 delay;
    s32 feedback;
};

class AudioStream;

class SpuPlayer : public ofBaseApp
{
public:
    SpuPlayer( AudioStream* stream );
    void setup();
    void quit();
    void audioOut( ofSoundBuffer& buffer );

    ofSoundStream soundStream;

    AudioStream* dataStream;
};



void PsyqSpuInit();
void PsyqSpuQuit();

void PsyqSpuSetTransferStartAddr( u32 addr );
void PsyqSpuWrite( u8* addr, u32 size );

void PsyqSpuSetVoicePitch( s32 voiceNum, u16 pitch );
void PsyqSpuSetVoiceVolume( s32 voiceNum, s16 volumeL, s16 volumeR );
void PsyqSpuSetVoiceStartAddr( s32 voiceNum, u32 startAddr );
void PsyqSpuSetVoiceLoopStartAddr( s32 voiceNum, u32 loopStartAddr );
void PsyqSpuSetVoiceSRAttr( s32 voiceNum, u16 SR, s32 SRmode );
void PsyqSpuSetVoiceRRAttr( s32 voiceNum, u16 RR, s32 RRmode );
void PsyqSpuSetVoiceARAttr( s32 voiceNum, u16 AR, s32 Armode );
void PsyqSpuSetVoiceDR( s32 voiceNum, u16 DR );
void PsyqSpuSetVoiceSL( s32 voiceNum, u16 SL );
void PsyqSpuSetVoiceVolumeAttr( s32 voiceNum, s16 volumeL, s16 volumeR, s16 volModeL, s16 volModeR );

void PsyqSpuSetKey( s32 on_off, u32 voice_bit );
void PsyqSpuSetReverbDepth( SpuReverbAttr* attr );
void PsyqSpuSetReverbVoice( s32 on_off, u32 voice_bit );
