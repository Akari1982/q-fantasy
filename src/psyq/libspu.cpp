#include "libspu.h"
#include "psxspu/spu.h"
#include "psxspu/audio_stream.h"

#include "ofSoundBuffer.h"

#include <mutex>

std::mutex spuMutex;

u32 g_transfer_start_addr = 0; // in real SPU we set special register and after that DMA transfer occured



SpuPlayer::SpuPlayer( AudioStream* stream )
{
    dataStream = stream;
}


void SpuPlayer::setup()
{
    soundStream.printDeviceList();

    ofSoundStreamSettings settings;

    settings.setOutListener( this );

    settings.setApi( ofSoundDevice::MS_DS );
    settings.sampleRate = 44100;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = 1024;
    settings.numBuffers = 4;
    soundStream.setup( settings );
}



void SpuPlayer::quit()
{
    soundStream.stop();
    soundStream.close();
}



void SpuPlayer::audioOut( ofSoundBuffer & buffer )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    s16 temp[44100][2];

    int numSamples = std::min<int>( 44100, buffer.getNumFrames() );

    SPU::Execute( nullptr, numSamples * 0x300, 0 );
    dataStream->ReadFrames(reinterpret_cast<s16*>(temp), numSamples);

    for( size_t i = 0; i < buffer.getNumFrames(); i++ )
    {
        buffer[i*buffer.getNumChannels() + 0] = temp[i][0] / (float)0x7fff;
        buffer[i*buffer.getNumChannels() + 1] = temp[i][1] / (float)0x7fff;
    }
}



void PsyqSpuInit()
{
    SPU::Initialize();
}



void PsyqSpuQuit()
{
    SPU::Shutdown();
}



void PsyqSpuSetTransferStartAddr( u32 addr )
{
    g_transfer_start_addr = addr;
}



void PsyqSpuWrite( u8* addr, u32 size )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    auto& s_ram = SPU::GetWritableRAM();
    std::memcpy( &s_ram[g_transfer_start_addr], addr, size );

    //s_ram = SPU::GetWritableRAM();
    //FILE* file = fopen( "SPU.RAM", "wb" );
    //fwrite( &s_ram[0], 1, 0x80000, file );
    //fclose( file );
}



void PsyqSpuSetVoicePitch( s32 voiceNum, u16 pitch )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    SPU::WriteRegister( voiceNum * 0x10 + 0x4, pitch );
}



void PsyqSpuSetVoiceVolume( s32 voiceNum, s16 volumeL, s16 volumeR )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    SPU::WriteRegister( voiceNum * 0x10 + 0x0, volumeL & 0x7fff );
    SPU::WriteRegister( voiceNum * 0x10 + 0x2, volumeR & 0x7fff );
}



void PsyqSpuSetVoiceStartAddr( s32 voiceNum, u32 startAddr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    SPU::WriteRegister( voiceNum * 0x10 + 0x6, startAddr >> 0x3 );
}



void PsyqSpuSetVoiceLoopStartAddr( s32 voiceNum, u32 loopStartAddr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    SPU::WriteRegister( voiceNum * 0x10 + 0xe, loopStartAddr >> 0x3 );
}



void PsyqSpuSetVoiceSRAttr( s32 voiceNum, u16 SR, s32 SRmode )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 mode_f = 0x100;
         if( SRmode == 0x1 ) mode_f = 0x0;
    else if( SRmode == 0x5 ) mode_f = 0x200;
    else if( SRmode == 0x7 ) mode_f = 0x300;

    u16 value = SPU::ReadRegister( voiceNum * 0x10 + 0xa );
    value &= 0x003f;
    value |= (SR | mode_f) << 0x6;
    SPU::WriteRegister( voiceNum * 0x10 + 0xa, value );
}



void PsyqSpuSetVoiceRRAttr( s32 voiceNum, u16 RR, s32 RRmode )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = SPU::ReadRegister( voiceNum * 0x10 + 0xa );
    value &= 0xffc0;
    value |= RR;
    value |= (RRmode == 0x7) << 0x5;
    SPU::WriteRegister( voiceNum * 0x10 + 0xa, value );
}



void PsyqSpuSetVoiceARAttr( s32 voiceNum, u16 AR, s32 Armode )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = SPU::ReadRegister( voiceNum * 0x10 + 0x8 );
    value &= 0x00ff;
    value |= (AR | ((Armode == 0x5) << 0x7)) << 0x8;
    SPU::WriteRegister( voiceNum * 0x10 + 0x8, value );
}



void PsyqSpuSetVoiceDR( s32 voiceNum, u16 DR )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = SPU::ReadRegister( voiceNum * 0x10 + 0x8 );
    value &= 0xff0f;
    value |= DR << 0x4;
    SPU::WriteRegister( voiceNum * 0x10 + 0x8, value );
}



void PsyqSpuSetVoiceSL( s32 voiceNum, u16 SL )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    u16 value = SPU::ReadRegister( voiceNum * 0x10 + 0x8 );
    value &= 0xfff0;
    value |= SL;
    SPU::WriteRegister( voiceNum * 0x10 + 0x8, value );
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
    SPU::WriteRegister( voiceNum * 0x10 + 0x0, mode | (volumeL & 0x7fff) );

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
    SPU::WriteRegister( voiceNum * 0x10 + 0x2, mode | (volumeR & 0x7fff) );
}



void PsyqSpuSetKey( s32 on_off, u32 voice_bit )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    if( on_off == SPU_OFF )
    {
        SPU::WriteRegister( 0x18c, voice_bit & 0xffff );
        SPU::WriteRegister( 0x18e, voice_bit >> 0x10 );
    }
    else if( on_off == SPU_ON )
    {
        SPU::WriteRegister( 0x188, voice_bit & 0xffff );
        SPU::WriteRegister( 0x18a, voice_bit >> 0x10 );
    }
}



void PsyqSpuSetReverbDepth( SpuReverbAttr* attr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    if( (attr->mask < 1) || (attr->mask & SPU_REV_DEPTHL) )
    {
        SPU::WriteRegister( 0x184, attr->depth.left );
    }

    if( (attr->mask < 1) || (attr->mask & SPU_REV_DEPTHR) )
    {
        SPU::WriteRegister( 0x186, attr->depth.right );
    }
}



void PsyqSpuSetReverbVoice( s32 on_off, u32 voice_bit )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    if( on_off == SPU_OFF )
    {
        SPU::WriteRegister( 0xcc * 2, SPU::ReadRegister( 0xcc * 2 ) & ~(voice_bit & 0xffff) );
        SPU::WriteRegister( 0xcd * 2, SPU::ReadRegister( 0xcd * 2 ) & ~((voice_bit >> 0x10) & 0xff) );
    }
    else if( on_off == SPU_ON )
    {
        SPU::WriteRegister( 0xcc * 2, SPU::ReadRegister( 0xcc * 2 ) | (voice_bit & 0xffff) );
        SPU::WriteRegister( 0xcd * 2, SPU::ReadRegister( 0xcd * 2 ) | ((voice_bit >> 0x10) & 0xff) );
    }
}



void PsyqSpuSetCommonAttr( SpuCommonAttr* attr )
{
    u32 mask = attr->mask;

    if( (mask == 0) || (mask & SPU_COMMON_MVOLL) )
    {
        s16 mvol_left = 0;
        u16 mvol_flag = 0;

        if( (mask == 0) || ((mask & SPU_COMMON_MVOLL) && (mask & SPU_COMMON_MVOLMODEL)) )
        {
            switch( attr->mvolmode.left )
            {
                case 0x1: mvol_flag = 0x8000; break;
                case 0x2: mvol_flag = 0x9000; break;
                case 0x3: mvol_flag = 0xa000; break;
                case 0x4: mvol_flag = 0xb000; break;
                case 0x5: mvol_flag = 0xc000; break;
                case 0x6: mvol_flag = 0xd000; break;
                case 0x7: mvol_flag = 0xe000; break;
                default:
                {
                    mvol_left = attr->mvol.left;
                    mvol_flag = 0;
                }
            }
        }
        else if( (mask != 0) && (mask & SPU_COMMON_MVOLL) && ((mask & SPU_COMMON_MVOLMODEL) == 0) )
        {
            mvol_left = attr->mvol.left;
            mvol_flag = 0;
        }

        if( mvol_flag != 0 )
        {
            if( attr->mvol.left >= 0x80 )
            {
                mvol_left = 0x7f;
            }
            else
            {
                mvol_left = ( attr->mvol.left < 0 ) ? 0 : attr->mvol.left;
            }
        }

        SPU::WriteRegister( 0x180, mvol_flag | (mvol_left & 0x7fff) );
    }

    if( (mask == 0) || (mask & SPU_COMMON_MVOLR) )
    {
        s16 mvol_right = 0;
        u16 mvol_flag = 0;

        if( (mask == 0) || ((mask & SPU_COMMON_MVOLR) && (mask & SPU_COMMON_MVOLMODER)) )
        {
            switch( attr->mvolmode.right )
            {
                case 0x1: mvol_flag = 0x8000; break;
                case 0x2: mvol_flag = 0x9000; break;
                case 0x3: mvol_flag = 0xa000; break;
                case 0x4: mvol_flag = 0xb000; break;
                case 0x5: mvol_flag = 0xc000; break;
                case 0x6: mvol_flag = 0xd000; break;
                case 0x7: mvol_flag = 0xe000; break;
                default:
                {
                    mvol_right = attr->mvol.right;
                    mvol_flag = 0;
                }
            }
        }
        else if( (mask != 0) && (mask & SPU_COMMON_MVOLR) && ((mask & SPU_COMMON_MVOLMODER) == 0) )
        {
            mvol_right = attr->mvol.right;
            mvol_flag = 0;
        }

        if( mvol_flag != 0 )
        {
            if( attr->mvol.right >= 0x80 )
            {
                mvol_right = 0x7f;
            }
            else
            {
                mvol_right = (attr->mvol.right < 0 ) ? 0 : attr->mvol.right;
            }
        }
        SPU::WriteRegister( 0x182, mvol_flag | (mvol_right & 0x7fff) );
    }

    if( (mask == 0) || (mask & SPU_COMMON_CDVOLL) ) SPU::WriteRegister( 0x1b0, attr->cd.volume.left );
    if( (mask == 0) || (mask & SPU_COMMON_CDVOLR) ) SPU::WriteRegister( 0x1b2, attr->cd.volume.right );
    if( (mask == 0) || (mask & SPU_COMMON_EXTVOLL) ) SPU::WriteRegister( 0x1b4, attr->ext.volume.left );
    if( (mask == 0) || (mask & SPU_COMMON_EXTVOLR) ) SPU::WriteRegister( 0x1b6, attr->ext.volume.right );

    if( (mask == 0) || (mask & SPU_COMMON_CDREV) )
    {
        if( attr->cd.reverb == 0 )
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) & 0xfffb );
        }
        else
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) | 0x0004 );
        }
    }

    if( (mask == 0) || (mask & SPU_COMMON_CDMIX) )
    {
        if( attr->cd.mix == 0 )
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) & 0xfffe );
        }
        else
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) | 0x0001 );
        }
    }

    if( (mask == 0) || (mask & SPU_COMMON_EXTREV) )
    {
        if( attr->ext.reverb == 0 )
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) & 0xfff7 );
        }
        else
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) | 0x0008 );
        }
    }

    if( (mask == 0) || (mask & SPU_COMMON_EXTMIX) )
    {
        if( attr->ext.mix == 0 )
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) & 0xfffd );
        }
        else
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) | 0x0002 );
        }
    }
}
