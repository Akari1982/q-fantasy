#include "libspu.h"
#include "psxspu/spu.h"

SpuPlayer* spu_player;

machine_config dummy;
spu_device emulatedSpuDevice( dummy, "SPU", nullptr, 33800000 );



void SpuPlayer::setup()
{
    int bufferSize = 512;
    sampleRate = 44100;
    phase = 0;
    phaseAdder = 0.0f;
    phaseAdderTarget = 0.0f;
    volume = 0.1f;
    bNoise = true;

    lAudio.assign( bufferSize, 0.0 );
    rAudio.assign( bufferSize, 0.0 );

    soundStream.printDeviceList();

    ofSoundStreamSettings settings;

    settings.setOutListener( this );
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = bufferSize;
    soundStream.setup( settings );
}



void SpuPlayer::audioOut( ofSoundBuffer & buffer )
{
    pan = 0.5f;
    float leftScale = 1 - pan;
    float rightScale = pan;

    // sin (n) seems to have trouble when n is very large, so we
    // keep phase in the range of 0-TWO_PI like this:
    while( phase > TWO_PI )
    {
        phase -= TWO_PI;
    }

    if( bNoise == true )
    {
        for( size_t i = 0; i < buffer.getNumFrames(); i++ )
        {
            lAudio[i] = buffer[i*buffer.getNumChannels()    ] = ofRandom( 0, 1 ) * volume * leftScale;
            rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = ofRandom( 0, 1 ) * volume * rightScale;
        }
    }
    else
    {
        int16_t temp[44100][2];

        int numSamples = std::min<int>( 44100, buffer.getNumFrames() );

        //spuMutex.lock();
        emulatedSpuDevice.generate(temp, numSamples * 4); 
        //spuMutex.unlock();

        for( size_t i = 0; i < buffer.getNumFrames(); i++ )
        {
            lAudio[i] = buffer[i*buffer.getNumChannels()    ] = temp[i][0] / (float)0x7fff;
            rAudio[i] = buffer[i*buffer.getNumChannels() + 1] = temp[i][1] / (float)0x7fff;
        }
    }
}



void SpuInit()
{
    emulatedSpuDevice.device_start();
    emulatedSpuDevice.device_reset();

    spu_player = new SpuPlayer();
    spu_player->setup();
}
