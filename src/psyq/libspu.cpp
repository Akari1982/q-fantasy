#include "libspu.h"
#include "psxspu/spu.h"

SpuPlayer* spu_player;

machine_config dummy;
spu_device emulatedSpuDevice( dummy, "SPU", nullptr, 33800000 );
std::mutex spuMutex;



void SpuPlayer::setup()
{
    soundStream.printDeviceList();

    ofSoundStreamSettings settings;

    settings.setOutListener( this );

    settings.setApi(ofSoundDevice::MS_DS);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = 512;
    soundStream.setup( settings );
}



void SpuPlayer::quit()
{
    soundStream.stop();
    soundStream.close();
}



void SpuPlayer::audioOut( ofSoundBuffer & buffer )
{
    int16_t temp[44100][2];

    int numSamples = std::min<int>( 44100, buffer.getNumFrames() );

    spuMutex.lock();
    emulatedSpuDevice.generate( temp, numSamples * 4 );
    spuMutex.unlock();

    for( size_t i = 0; i < buffer.getNumFrames(); i++ )
    {
        buffer[i*buffer.getNumChannels() + 0] = temp[i][0] / (float)0x7fff;
        buffer[i*buffer.getNumChannels() + 1] = temp[i][1] / (float)0x7fff;
    }
}



void PsyqSpuInit()
{
    emulatedSpuDevice.device_start();
    emulatedSpuDevice.device_reset();

    spu_player = new SpuPlayer();
    spu_player->setup();
}



void PsyqSpuQuit()
{
    spu_player->quit();
    delete spu_player;

    emulatedSpuDevice.device_stop();
}



void PsyqSpuSetTransferStartAddr( u32 addr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    emulatedSpuDevice.write( 0x1a6 / 0x2, addr >> 0x3 );
}



void PsyqSpuWrite( u8* addr, u32 size )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    emulatedSpuDevice.dma_write( (u32*)addr, 0, size / 4 );
}



void PsyqSpuRead( u8* addr, u32 size )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    emulatedSpuDevice.dma_read( (u32*)addr, 0, size / 4 );
}



void PsyqSpuSetVoicePitch( s32 voiceNum, u16 pitch )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    emulatedSpuDevice.write( voiceNum * 0x8 + 0x2, pitch );
}



void PsyqSpuSetVoiceVolume( s32 voiceNum, s16 volumeL, s16 volumeR )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    emulatedSpuDevice.write( voiceNum * 0x8 + 0x0, volumeL & 0x7fff );
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x1, volumeR & 0x7fff );
}



void PsyqSpuSetVoiceStartAddr( s32 voiceNum, u32 startAddr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    emulatedSpuDevice.write( voiceNum * 0x8 + 0x3, startAddr >> 0x3 );
}



void PsyqSpuSetVoiceLoopStartAddr( s32 voiceNum, u32 loopStartAddr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    emulatedSpuDevice.write( voiceNum * 0x8 + 0x7, loopStartAddr >> 0x3 );
}



void PsyqSpuSetVoiceSRAttr( s32 voiceNum, u16 SR, s32 SRmode )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 mode_f = 0x100;
         if( SRmode == 0x1 ) mode_f = 0x0;
    else if( SRmode == 0x5 ) mode_f = 0x200;
    else if( SRmode == 0x7 ) mode_f = 0x300;

    u16 value = emulatedSpuDevice.read( voiceNum * 0x8 + 0x5 );
    value &= 0x003f;
    value |= (SR | mode_f) << 0x6;
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x5, value );
}



void PsyqSpuSetVoiceRRAttr( s32 voiceNum, u16 RR, s32 RRmode )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = emulatedSpuDevice.read( voiceNum * 0x8 + 0x5 );
    value &= 0xffc0;
    value |= RR;
    value |= (RRmode == 0x7) << 0x5;
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x5, value );
}



void PsyqSpuSetVoiceARAttr( s32 voiceNum, u16 AR, s32 Armode )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = emulatedSpuDevice.read( voiceNum * 0x8 + 0x4 );
    value &= 0x00ff;
    value |= (AR | ((Armode == 0x5) << 0x7)) << 0x8;
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x4, value );
}



void PsyqSpuSetVoiceDR( s32 voiceNum, u16 DR )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = emulatedSpuDevice.read( voiceNum * 0x8 + 0x4 );
    value &= 0xff0f;
    value |= DR << 0x4;
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x4, value );
}



void PsyqSpuSetVoiceSL( s32 voiceNum, u16 SL )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = emulatedSpuDevice.read( voiceNum * 0x8 + 0x4 );
    value &= 0xfff0;
    value |= SL;
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x4, value );
}



void PsyqSpuSetVoiceVolumeAttr( s32 voiceNum, s16 volumeL, s16 volumeR, s16 volModeL, s16 volModeR )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 mode = 0;
    switch( volModeL )
    {
        case 0x1: mode = 0x8000; break;
        case 0x2: mode = 0x9000; break;
        case 0x3: mode = 0xa000; break;
        case 0x4: mode = 0xb000; break;
        case 0x5: mode = 0xc000; break;
        case 0x6: mode = 0xd000; break;
        case 0x7: mode = 0xe000; break;
    }
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x0, mode | (volumeL & 0x7fff) );

    mode = 0;
    switch( volModeR )
    {
        case 0x1: mode = 0x8000; break;
        case 0x2: mode = 0x9000; break;
        case 0x3: mode = 0xa000; break;
        case 0x4: mode = 0xb000; break;
        case 0x5: mode = 0xc000; break;
        case 0x6: mode = 0xd000; break;
        case 0x7: mode = 0xe000; break;
    }
    emulatedSpuDevice.write( voiceNum * 0x8 + 0x1, mode | (volumeR & 0x7fff) );
}



void PsyqSpuSetKey( s32 on_off, u32 voice_bit )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    if( on_off == SPU_OFF )
    {
        emulatedSpuDevice.write( 0x18c / 0x2, voice_bit & 0xffff );
        emulatedSpuDevice.write( 0x18e / 0x2, voice_bit >> 0x10 );
    }
    else if( on_off == SPU_ON )
    {
        emulatedSpuDevice.write( 0x188 / 0x2, voice_bit & 0xffff );
        emulatedSpuDevice.write( 0x18a / 0x2, voice_bit >> 0x10 );
    }
}



void PsyqSpuSetReverbDepth( SpuReverbAttr* attr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    if( (attr->mask < 1) || (attr->mask & SPU_REV_DEPTHL) )
    {
        emulatedSpuDevice.write( 0x184 / 0x2, attr->depth.left );
    }

    if( (attr->mask < 1) || (attr->mask & SPU_REV_DEPTHR) )
    {
        emulatedSpuDevice.write( 0x186 / 0x2, attr->depth.right );
    }
}



void PsyqSpuSetReverbVoice( s32 on_off, u32 voice_bit )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    if( on_off == SPU_OFF )
    {
        emulatedSpuDevice.write( 0xcc, emulatedSpuDevice.read( 0xcc ) & ~(voice_bit & 0xffff) );
        emulatedSpuDevice.write( 0xcd, emulatedSpuDevice.read( 0xcd ) & ~((voice_bit >> 0x10) & 0xff) );
    }
    else if( on_off == SPU_ON )
    {
        emulatedSpuDevice.write( 0xcc, emulatedSpuDevice.read( 0xcc ) | (voice_bit & 0xffff) );
        emulatedSpuDevice.write( 0xcd, emulatedSpuDevice.read( 0xcd ) | ((voice_bit >> 0x10) & 0xff) );
    }
}
