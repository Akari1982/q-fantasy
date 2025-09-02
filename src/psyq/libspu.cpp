#include "libspu.h"
#include "psxspu/spu.h"
#include "psxspu/audio_stream.h"

#include "ofSoundBuffer.h"

#include <mutex>

std::mutex spuMutex;

u32 g_reverb_on;
//u8* g_reverb_workarea;
s32 g_reverb_mode;
s16 g_reverb_depth_left;
s16 g_reverb_depth_right;
s32 g_reverb_delay;
s32 g_reverb_feedback;

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
    // init SPU emulator
    SPU::Initialize();

    // real func calls
    SPU::WriteRegister( 0x180, 0 ); // Mainvolume left
    SPU::WriteRegister( 0x182, 0 ); // Mainvolume right
    SPU::WriteRegister( 0x1aa, 0 ); // SPU Control Register
    SPU::Execute( nullptr, 0x1 * 0x300, 0 );
    SPU::WriteRegister( 0x180, 0 ); // Mainvolume left
    SPU::WriteRegister( 0x182, 0 ); // Mainvolume right

    SPU::WriteRegister( 0x1ac, 0x0004 ); // Sound RAM Data Transfer Control (should be 0004h)
    SPU::WriteRegister( 0x184, 0x0 ); // Reverb Output Volume Left
    SPU::WriteRegister( 0x186, 0x0 ); // Reverb Output Volume Right
    SPU::WriteRegister( 0x18c, 0xffff ); // Key OFF lower
    SPU::WriteRegister( 0x18e, 0xffff ); // Key OFF upper
    SPU::WriteRegister( 0x198, 0x0 ); // Reverb mode aka Echo On lower
    SPU::WriteRegister( 0x19a, 0x0 ); // Reverb mode aka Echo On upper

    SPU::WriteRegister( 0x190, 0 ); // Pitch Modulation Enable Flags lower
    SPU::WriteRegister( 0x192, 0 ); // Pitch Modulation Enable Flags upper
    SPU::WriteRegister( 0x194, 0 ); // Noise mode enable lower
    SPU::WriteRegister( 0x196, 0 ); // Noise mode enable upper
    SPU::WriteRegister( 0x1b0, 0 ); // CD Audio Input Volume Left
    SPU::WriteRegister( 0x1b2, 0 ); // CD Audio Input Volume Right
    SPU::WriteRegister( 0x1b4, 0 ); // External Audio Input Volume Left
    SPU::WriteRegister( 0x1b6, 0 ); // External Audio Input Volume Right

    for( int i = 0; i < 0x18; ++i )
    {
        SPU::WriteRegister( i * 0x10 + 0x0, 0 ); // Volume Left
        SPU::WriteRegister( i * 0x10 + 0x2, 0 ); // Volume Right
        SPU::WriteRegister( i * 0x10 + 0x4, 0x3fff ); // Reverb Output Volume Left
        SPU::WriteRegister( i * 0x10 + 0x6, 0x0200 ); // Reverb Output Volume Right
        SPU::WriteRegister( i * 0x10 + 0x8, 0 ); // Key ON lower
        SPU::WriteRegister( i * 0x10 + 0xa, 0 ); // Key ON upper
    }

    SPU::WriteRegister( 0x188, 0xffff ); // Key ON lower
    SPU::WriteRegister( 0x18a, 0x00ff ); // Key ON upper
    SPU::Execute( nullptr, 0x4 * 0x300, 0 );
    SPU::WriteRegister( 0x18c, 0xffff ); // Key OFF upper
    SPU::WriteRegister( 0x18e, 0x00ff ); // Key OFF lower
    SPU::Execute( nullptr, 0x4 * 0x300, 0 );

    SPU::WriteRegister( 0x1aa, 0xc000 ); // enable and unmute SPU

    g_reverb_on = SPU_OFF;
//    [0x8004a698] = w(0);
    g_reverb_mode = 0;
    g_reverb_depth_left = 0;
    g_reverb_depth_right = 0;
    g_reverb_delay = 0;
    g_reverb_feedback = 0;
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



s32 PsyqSpuSetReverb( s32 on_off )
{
    if( on_off == 0 )
    {
        g_reverb_on = 0;
        SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) & 0xff7f ); // Reverb Master Disable
    }
    else if( on_off == 1 )
    {
//        if( w[0x8004a698] != on_off )
//        {
//            if( func37d90( g_reverb_workarea ) != 0 ) // reverb work area already reserved
//            {
//                g_reverb_on = 0;
//                [spu + 0x1aa] = h(hu[spu + 0x1aa] & 0xff7f); // Reverb Master Disable
//
//                return g_reverb_on;
//            }
//        }

        g_reverb_on = on_off;
        SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) | 0x0080 ); // Reverb Master Enable
    }

    return g_reverb_on;
}



s32 PsyqSpuSetReverbModeParam( SpuReverbAttr* attr )
{
    u32 mask = attr->mask;

    bool rev_enabled = false;
    bool loaded_mode = false;
    bool loaded_delay = false;
    bool loaded_feedback = false;
    bool clear_wa = false;
//    [SP + 0x10] = w(0);

    if( (mask == 0) || (mask & SPU_REV_MODE) )
    {
        s32 mode = attr->mode;

        if( mode & SPU_REV_MODE_CLEAR_WA )
        {
            mode &= ~SPU_REV_MODE_CLEAR_WA;
            clear_wa = true;
        }

        if( mode >= SPU_REV_MODE_MAX ) return -1;

        loaded_mode = true;

//        if( func37d90( w[0x8004ab5c + mode * 4] ) != 0 ) return -1; // reverb work area already reserved

        g_reverb_mode = mode;

//        g_reverb_workarea = w[0x8004ab5c + mode * 4];
//
//        u32 src = 0x8004abac + mode * 0x44;
//        dst = SP + 0x10;
//        for( int i = 0x43; i != -1; --i )
//        {
//            [dst] = b(bu[src]);
//            src += 0x1;
//            dst += 0x1;
//        }

        if( mode == SPU_REV_MODE_ECHO )
        {
            g_reverb_feedback = 0x7f;
            g_reverb_delay = 0x7f;
        }
        else if( mode == SPU_REV_MODE_DELAY )
        {
            g_reverb_feedback = 0;
            g_reverb_delay = 0x7f;
        }
        else
        {
            g_reverb_feedback = 0;
            g_reverb_delay = 0;
        }
    }

//    if( (mask == 0) || (mask & SPU_REV_DELAYTIME) )
//    {
//        if( (g_reverb_mode == SPU_REV_MODE_ECHO) || (g_reverb_mode == SPU_REV_MODE_DELAY) )
//        {
//            loaded_delay = true;
//
//            if( loaded_mode == false )
//            {
//                src = 0x8004abac + g_reverb_mode * 0x44;
//                dst = SP + 0x10;
//                for( int i = 0x43; i != -1; --i )
//                {
//                    [dst] = b(bu[src]);
//                    src += 0x1;
//                    dst += 0x1;
//                }
//                [SP + 0x10] = w(0x0c011c00);
//            }
//
//            g_reverb_delay = attr->delay;
//
//            A2 = 0x81020409;
//            V1 = g_reverb_delay << 0xd;
//            80038034	mult   v1, a2
//            80038038	mfhi   v0
//            A1 = g_reverb_delay << 0xc;
//            80038044	mult   a1, a2
//            V0 = V0 + V1;
//            V0 = V0 >> 0x6;
//            V1 = V1 >> 0x1f;
//            V0 = V0 - V1;
//            [SP + 0x28] = h(V0 - hu[SP + 0x14]);
//            80038074	mfhi   v1
//            V1 = V1 + A1;
//            V1 = V1 >> 06;
//            A1 = A1 >> 1f;
//            V1 = V1 - A1;
//            [SP + 0x2a] = h(V1 - hu[SP + 0x16]);
//            [SP + 0x34] = h(V1 + hu[SP + 0x36]);
//            [SP + 0x2c] = h(V1 + hu[SP + 0x2e]);
//            [SP + 0x48] = h(V1 + hu[SP + 0x4c]);
//            [SP + 0x4a] = h(V1 + hu[SP + 0x4e]);
//        }
//        else
//        {
//            g_reverb_delay = 0;
//        }
//    }
//
//    if( (mask == 0) || (mask & SPU_REV_FEEDBACK) )
//    {
//        if( (g_reverb_mode == SPU_REV_MODE_ECHO) || (g_reverb_mode == SPU_REV_MODE_DELAY) )
//        {
//            loaded_feedback = true;
//
//            if( loaded_mode == false )
//            {
//                if( loaded_delay == false )
//                {
//                    src = 0x8004abac + g_reverb_mode * 0x44;
//                    dst = SP + 0x10;
//                    for( int i = 0x43; i != -1; --i )
//                    {
//                        [dst] = b(bu[src]);
//                        src += 0x1;
//                        dst += 0x1;
//                    }
//                    [SP + 0x10] = w(0x80);
//                }
//                else
//                {
//                    [SP + 0x10] = w(w[SP + 0x10] | 0x80);
//                }
//            }
//
//            g_reverb_feedback = attr->feedback;
//
//            // division by by 1.985
//            const int64_t A0 = 0x81020409LL;
//            int64_t V1 = (int64_t)g_reverb_feedback * 0x8100;
//            int64_t mul = V1 * A0;
//            int64_t V0 = mul >> 32;
//            V0 += V1;
//            V0 >>= 0x6;
//            int64_t signCorr = V1 >> 31;
//            V0 -= signCorr;
//            [SP + 0x22] = h(V0);
//        }
//        else
//        {
//            g_reverb_feedback = 0;
//        }
//    }

    if( loaded_mode )
    {
        rev_enabled = (SPU::ReadRegister( 0x1aa ) >> 0x7) & 0x1;

        if( rev_enabled != 0 )
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) & 0xff7f ); // Reverb Master Disable
        }

        SPU::WriteRegister( 0x184, 0 );
        SPU::WriteRegister( 0x186, 0 );
        g_reverb_depth_left = 0;
        g_reverb_depth_right = 0;
    }
    else
    {
        if( (mask == 0) || (mask & SPU_REV_DEPTHL) )
        {
            SPU::WriteRegister( 0x184, attr->depth.left );
            g_reverb_depth_left = attr->depth.left;
        }

        if( (mask == 0) || (mask & SPU_REV_DEPTHR) )
        {
            SPU::WriteRegister( 0x186, attr->depth.right );
            g_reverb_depth_right = attr->depth.right;
        }
    }

    if( loaded_mode || loaded_delay || loaded_feedback )
    {
//        func3832c( SP + 0x10 ); // set spu reg
    }

    if( clear_wa == true )
    {
//        func388e8( g_reverb_mode ); // transfer reverb data to spu ram
    }

    if( loaded_mode )
    {
//        func36d98( 0xd1, g_reverb_workarea, 0 ); // 1a2 Sound RAM Reverb Work Area Start Address

        if( rev_enabled != 0 )
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) | 0x0080 ); // Reverb Master Enable
        }
    }

    return 0;
}



//void func3832c( A0 )
//{
//    spu = w[0x8004aaf4]; // 0x1f801c00
//
//    u32 mask = w[A0 + 0x0];
//
//    if( (mask == 0) || (mask & 0x00000001) ) [spu + 0x1c0] = h(hu[A0 + 0x04]);
//    if( (mask == 0) || (mask & 0x00000002) ) [spu + 0x1c2] = h(hu[A0 + 0x06]);
//    if( (mask == 0) || (mask & 0x00000004) ) [spu + 0x1c4] = h(hu[A0 + 0x08]);
//    if( (mask == 0) || (mask & 0x00000008) ) [spu + 0x1c6] = h(hu[A0 + 0x0a]);
//    if( (mask == 0) || (mask & 0x00000010) ) [spu + 0x1c8] = h(hu[A0 + 0x0c]);
//    if( (mask == 0) || (mask & 0x00000020) ) [spu + 0x1ca] = h(hu[A0 + 0x0e]);
//    if( (mask == 0) || (mask & 0x00000040) ) [spu + 0x1cc] = h(hu[A0 + 0x10]);
//    if( (mask == 0) || (mask & 0x00000080) ) [spu + 0x1ce] = h(hu[A0 + 0x12]);
//    if( (mask == 0) || (mask & 0x00000100) ) [spu + 0x1d0] = h(hu[A0 + 0x14]);
//    if( (mask == 0) || (mask & 0x00000200) ) [spu + 0x1d2] = h(hu[A0 + 0x16]);
//    if( (mask == 0) || (mask & 0x00000400) ) [spu + 0x1d4] = h(hu[A0 + 0x18]);
//    if( (mask == 0) || (mask & 0x00000800) ) [spu + 0x1d6] = h(hu[A0 + 0x1a]);
//    if( (mask == 0) || (mask & 0x00001000) ) [spu + 0x1d8] = h(hu[A0 + 0x1c]);
//    if( (mask == 0) || (mask & 0x00002000) ) [spu + 0x1da] = h(hu[A0 + 0x1e]);
//    if( (mask == 0) || (mask & 0x00004000) ) [spu + 0x1dc] = h(hu[A0 + 0x20]);
//    if( (mask == 0) || (mask & 0x00008000) ) [spu + 0x1de] = h(hu[A0 + 0x22]);
//    if( (mask == 0) || (mask & 0x00010000) ) [spu + 0x1e0] = h(hu[A0 + 0x24]);
//    if( (mask == 0) || (mask & 0x00020000) ) [spu + 0x1e2] = h(hu[A0 + 0x26]);
//    if( (mask == 0) || (mask & 0x00040000) ) [spu + 0x1e4] = h(hu[A0 + 0x28]);
//    if( (mask == 0) || (mask & 0x00080000) ) [spu + 0x1e6] = h(hu[A0 + 0x2a]);
//    if( (mask == 0) || (mask & 0x00100000) ) [spu + 0x1e8] = h(hu[A0 + 0x2c]);
//    if( (mask == 0) || (mask & 0x00200000) ) [spu + 0x1ea] = h(hu[A0 + 0x2e]);
//    if( (mask == 0) || (mask & 0x00400000) ) [spu + 0x1ec] = h(hu[A0 + 0x30]);
//    if( (mask == 0) || (mask & 0x00800000) ) [spu + 0x1ee] = h(hu[A0 + 0x32]);
//    if( (mask == 0) || (mask & 0x01000000) ) [spu + 0x1f0] = h(hu[A0 + 0x34]);
//    if( (mask == 0) || (mask & 0x02000000) ) [spu + 0x1f2] = h(hu[A0 + 0x36]);
//    if( (mask == 0) || (mask & 0x04000000) ) [spu + 0x1f4] = h(hu[A0 + 0x38]);
//    if( (mask == 0) || (mask & 0x08000000) ) [spu + 0x1f6] = h(hu[A0 + 0x3a]);
//    if( (mask == 0) || (mask & 0x10000000) ) [spu + 0x1f8] = h(hu[A0 + 0x3c]);
//    if( (mask == 0) || (mask & 0x20000000) ) [spu + 0x1fa] = h(hu[A0 + 0x3e]);
//    if( (mask == 0) || (mask & 0x40000000) ) [spu + 0x1fc] = h(hu[A0 + 0x40]);
//    if( (mask == 0) || (mask & 0x80000000) ) [spu + 0x1fe] = h(hu[A0 + 0x42]);
//}



void PsyqSpuGetReverbModeParam( SpuReverbAttr* attr )
{
    attr->mode = g_reverb_mode;
    attr->depth.left = g_reverb_depth_left;
    attr->depth.right = g_reverb_depth_right;
    attr->delay = g_reverb_delay;
    attr->feedback = g_reverb_feedback;
}



void PsyqSpuSetReverbDepth( SpuReverbAttr* attr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    if( (attr->mask < 1) || (attr->mask & SPU_REV_DEPTHL) )
    {
        SPU::WriteRegister( 0x184, attr->depth.left );
        g_reverb_depth_left = attr->depth.left;
    }

    if( (attr->mask < 1) || (attr->mask & SPU_REV_DEPTHR) )
    {
        SPU::WriteRegister( 0x186, attr->depth.right );
        g_reverb_depth_right = attr->depth.right;
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
