#include "libspu.h"
#include "psxspu/spu.h"
#include "psxspu/audio_stream.h"

#include "ofSoundBuffer.h"

#include <mutex>

std::mutex spuMutex;

u32 g_reverb_on;
u32 g_reverb_workarea_cur;
s32 g_reverb_mode;
s16 g_reverb_depth_left;
s16 g_reverb_depth_right;
s32 g_reverb_delay;
s32 g_reverb_feedback;

u32 g_spu_shift;

u32 g_transfer_start_addr = 0; // in real SPU we set special register and after that DMA transfer occured

u32 g_reverb_workarea[0xa] =
{
    0x0000fffe, 0x0000fb28, 0x0000fc18, 0x0000f6f8, 0x0000f204, 0x0000ea44, 0x0000e128, 0x0000cff8, 0x0000cff8, 0x0000f880
};

SpuReverbReg g_reverb_data[0xa] =
{
    { 0x00000000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
    { 0x00000000, 0x007d, 0x005b, 0x6d80, 0x54b8, 0xbed0, 0x0000, 0x0000, 0xba80, 0x5800, 0x5300, 0x04d6, 0x0333, 0x03f0, 0x0227, 0x0374, 0x01ef, 0x0334, 0x01b5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x01b4, 0x0136, 0x00b8, 0x005c, 0x8000, 0x8000 },
    { 0x00000000, 0x0033, 0x0025, 0x70f0, 0x4fa8, 0xbce0, 0x4410, 0xc0f0, 0x9c00, 0x5280, 0x4ec0, 0x03e4, 0x031b, 0x03a4, 0x02af, 0x0372, 0x0266, 0x031c, 0x025d, 0x025c, 0x018e, 0x022f, 0x0135, 0x01d2, 0x00b7, 0x018f, 0x00b5, 0x00b4, 0x0080, 0x004c, 0x0026, 0x8000, 0x8000 },
    { 0x00000000, 0x00b1, 0x007f, 0x70f0, 0x4fa8, 0xbce0, 0x4510, 0xbef0, 0xb4c0, 0x5280, 0x4ec0, 0x0904, 0x076b, 0x0824, 0x065f, 0x07a2, 0x0616, 0x076c, 0x05ed, 0x05ec, 0x042e, 0x050f, 0x0305, 0x0462, 0x02b7, 0x042f, 0x0265, 0x0264, 0x01b2, 0x0100, 0x0080, 0x8000, 0x8000 },
    { 0x00000000, 0x00e3, 0x00a9, 0x6f60, 0x4fa8, 0xbce0, 0x4510, 0xbef0, 0xa680, 0x5680, 0x52c0, 0x0dfb, 0x0b58, 0x0d09, 0x0a3c, 0x0bd9, 0x0973, 0x0b59, 0x08da, 0x08d9, 0x05e9, 0x07ec, 0x04b0, 0x06ef, 0x03d2, 0x05ea, 0x031d, 0x031c, 0x0238, 0x0154, 0x00aa, 0x8000, 0x8000 },
    { 0x00000000, 0x01a5, 0x0139, 0x6000, 0x5000, 0x4c00, 0xb800, 0xbc00, 0xc000, 0x6000, 0x5c00, 0x15ba, 0x11bb, 0x14c2, 0x10bd, 0x11bc, 0x0dc1, 0x11c0, 0x0dc3, 0x0dc0, 0x09c1, 0x0bc4, 0x07c1, 0x0a00, 0x06cd, 0x09c2, 0x05c1, 0x05c0, 0x041a, 0x0274, 0x013a, 0x8000, 0x8000 },
    { 0x00000000, 0x033d, 0x0231, 0x7e00, 0x5000, 0xb400, 0xb000, 0x4c00, 0xb000, 0x6000, 0x5400, 0x1ed6, 0x1a31, 0x1d14, 0x183b, 0x1bc2, 0x16b2, 0x1a32, 0x15ef, 0x15ee, 0x1055, 0x1334, 0x0f2d, 0x11f6, 0x0c5d, 0x1056, 0x0ae1, 0x0ae0, 0x07a2, 0x0464, 0x0232, 0x8000, 0x8000 },
    { 0x00000000, 0x0001, 0x0001, 0x7fff, 0x7fff, 0x0000, 0x0000, 0x0000, 0x8100, 0x0000, 0x0000, 0x1fff, 0x0fff, 0x1005, 0x0005, 0x0000, 0x0000, 0x1005, 0x0005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1004, 0x1002, 0x0004, 0x0002, 0x8000, 0x8000 },
    { 0x00000000, 0x0001, 0x0001, 0x7fff, 0x7fff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1fff, 0x0fff, 0x1005, 0x0005, 0x0000, 0x0000, 0x1005, 0x0005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1004, 0x1002, 0x0004, 0x0002, 0x8000, 0x8000 },
    { 0x00000000, 0x0017, 0x0013, 0x70f0, 0x4fa8, 0xbce0, 0x4510, 0xbef0, 0x8500, 0x5f80, 0x54c0, 0x0371, 0x02af, 0x02e5, 0x01df, 0x02b0, 0x01d7, 0x0358, 0x026a, 0x01d6, 0x011e, 0x012d, 0x00b1, 0x011f, 0x0059, 0x01a0, 0x00e3, 0x0058, 0x0040, 0x0028, 0x0014, 0x8000, 0x8000 }
};



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

    g_spu_shift = 0x3;

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

    SPU::WriteRegister( voiceNum * 0x10 + 0x6, startAddr >> g_spu_shift );
}



void PsyqSpuSetVoiceLoopStartAddr( s32 voiceNum, u32 loopStartAddr )
{
    std::lock_guard<std::mutex> lock( spuMutex );

    SPU::WriteRegister( voiceNum * 0x10 + 0xe, loopStartAddr >> g_spu_shift );
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
//            if( func37d90( g_reverb_workarea_cur ) != 0 ) // reverb work area already reserved
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



s32 SpuReverbClearWorkarea( s32 mode )
{
    if( mode >= SPU_REV_MODE_MAX ) return -1;

    u32 dst, size;

    if( mode == 0 )
    {
        size = 0x10 << g_spu_shift;
        dst = 0xfff0 << g_spu_shift;
    }
    else
    {
        size = (0x10000 - g_reverb_workarea[mode]) << g_spu_shift;
        dst = g_reverb_workarea[mode] << g_spu_shift;
    }

    auto& s_ram = SPU::GetWritableRAM();
    std::memset( &s_ram[dst], 0, size );

    return 0;
}



void SpuSetReverbReg(SpuReverbReg* attr)
{
    u32 mask = attr->mask;
    if( (mask == 0) || (mask & 0x00000001) ) SPU::WriteRegister( 0x1c0, attr->dAPF1 );
    if( (mask == 0) || (mask & 0x00000002) ) SPU::WriteRegister( 0x1c2, attr->dAPF2 );
    if( (mask == 0) || (mask & 0x00000004) ) SPU::WriteRegister( 0x1c4, attr->vIIR );
    if( (mask == 0) || (mask & 0x00000008) ) SPU::WriteRegister( 0x1c6, attr->vCOMB1 );
    if( (mask == 0) || (mask & 0x00000010) ) SPU::WriteRegister( 0x1c8, attr->vCOMB2 );
    if( (mask == 0) || (mask & 0x00000020) ) SPU::WriteRegister( 0x1ca, attr->vCOMB3 );
    if( (mask == 0) || (mask & 0x00000040) ) SPU::WriteRegister( 0x1cc, attr->vCOMB4 );
    if( (mask == 0) || (mask & 0x00000080) ) SPU::WriteRegister( 0x1ce, attr->vWALL );
    if( (mask == 0) || (mask & 0x00000100) ) SPU::WriteRegister( 0x1d0, attr->vAPF1 );
    if( (mask == 0) || (mask & 0x00000200) ) SPU::WriteRegister( 0x1d2, attr->vAPF2 );
    if( (mask == 0) || (mask & 0x00000400) ) SPU::WriteRegister( 0x1d4, attr->mLSAME );
    if( (mask == 0) || (mask & 0x00000800) ) SPU::WriteRegister( 0x1d6, attr->mRSAME );
    if( (mask == 0) || (mask & 0x00001000) ) SPU::WriteRegister( 0x1d8, attr->mLCOMB1 );
    if( (mask == 0) || (mask & 0x00002000) ) SPU::WriteRegister( 0x1da, attr->mRCOMB1 );
    if( (mask == 0) || (mask & 0x00004000) ) SPU::WriteRegister( 0x1dc, attr->mLCOMB2 );
    if( (mask == 0) || (mask & 0x00008000) ) SPU::WriteRegister( 0x1de, attr->mRCOMB2 );
    if( (mask == 0) || (mask & 0x00010000) ) SPU::WriteRegister( 0x1e0, attr->dLSAME );
    if( (mask == 0) || (mask & 0x00020000) ) SPU::WriteRegister( 0x1e2, attr->dRSAME );
    if( (mask == 0) || (mask & 0x00040000) ) SPU::WriteRegister( 0x1e4, attr->mLDIFF );
    if( (mask == 0) || (mask & 0x00080000) ) SPU::WriteRegister( 0x1e6, attr->mRDIFF );
    if( (mask == 0) || (mask & 0x00100000) ) SPU::WriteRegister( 0x1e8, attr->mLCOMB3 );
    if( (mask == 0) || (mask & 0x00200000) ) SPU::WriteRegister( 0x1ea, attr->mRCOMB3 );
    if( (mask == 0) || (mask & 0x00400000) ) SPU::WriteRegister( 0x1ec, attr->mLCOMB4 );
    if( (mask == 0) || (mask & 0x00800000) ) SPU::WriteRegister( 0x1ee, attr->mRCOMB4 );
    if( (mask == 0) || (mask & 0x01000000) ) SPU::WriteRegister( 0x1f0, attr->dLDIFF );
    if( (mask == 0) || (mask & 0x02000000) ) SPU::WriteRegister( 0x1f2, attr->dRDIFF );
    if( (mask == 0) || (mask & 0x04000000) ) SPU::WriteRegister( 0x1f4, attr->mLAPF1 );
    if( (mask == 0) || (mask & 0x08000000) ) SPU::WriteRegister( 0x1f6, attr->mRAPF1 );
    if( (mask == 0) || (mask & 0x10000000) ) SPU::WriteRegister( 0x1f8, attr->mLAPF2 );
    if( (mask == 0) || (mask & 0x20000000) ) SPU::WriteRegister( 0x1fa, attr->mRAPF2 );
    if( (mask == 0) || (mask & 0x40000000) ) SPU::WriteRegister( 0x1fc, attr->vLIN );
    if( (mask == 0) || (mask & 0x80000000) ) SPU::WriteRegister( 0x1fe, attr->vRIN );
}



s32 PsyqSpuSetReverbModeParam( SpuReverbAttr* attr )
{
    u32 mask = attr->mask;

    bool rev_enabled = false;
    bool loaded_mode = false;
    bool loaded_delay = false;
    bool loaded_feedback = false;
    bool clear_wa = false;

    SpuReverbReg rev_reg;

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

        g_reverb_mode = mode;
        g_reverb_workarea_cur = g_reverb_workarea[mode];
        rev_reg = g_reverb_data[g_reverb_mode];

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
//                rev_reg = g_reverb_data[g_reverb_mode];
//                rev_reg.mask = 0x0c011c00;
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
//                    rev_reg = g_reverb_data[g_reverb_mode];
//                    rev_reg.mask = 0x00000080;
//                }
//                else
//                {
//                    rev_reg.mask |= 0x00000080;
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
        SpuSetReverbReg( &rev_reg );
    }

    if( clear_wa == true )
    {
        SpuReverbClearWorkarea( g_reverb_mode );
    }

    if( loaded_mode )
    {
        SPU::WriteRegister( 0xd1 * 0x2, g_reverb_workarea_cur ); // Reverb Work Area Start Address

        if( rev_enabled != 0 )
        {
            SPU::WriteRegister( 0x1aa, SPU::ReadRegister( 0x1aa ) | 0x0080 ); // Reverb Master Enable
        }
    }

    return 0;
}



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
