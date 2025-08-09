#pragma once

#include "typedef.h"

#include "ofMain.h"



class SpuPlayer : public ofBaseApp
{
public:
    void setup();
    void audioOut( ofSoundBuffer& buffer );

    ofSoundStream soundStream;

    float pan;
    int sampleRate;
    bool bNoise;
    float volume;

    vector <float> lAudio;
    vector <float> rAudio;

    //------------------- for the simple sine wave synthesis
    float targetFrequency;
    float phase;
    float phaseAdder;
    float phaseAdderTarget;
};



void PsyqSpuInit();
void PsyqSpuSetTransferStartAddr( u32 addr );
void PsyqSpuWrite( u8* addr, u32 size );
