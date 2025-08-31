// SPDX-FileCopyrightText: 2019-2025 Connor McLaughlin <stenzek@gmail.com>
// SPDX-License-Identifier: CC-BY-NC-ND-4.0

#include "audio_stream.h"
#include "../libspu.h"

//#include "common/assert.h"
//#include "common/error.h"
//#include "common/log.h"



namespace {

class OfAudioStream final : public AudioStream
{
public:
    OfAudioStream(u32 sample_rate, const AudioStreamParameters& parameters);
    ~OfAudioStream();

  void SetPaused(bool paused) override;

  bool OpenDevice();
  void CloseDevice();

protected:
    SpuPlayer* m_stream = nullptr;
};
} // namespace



std::unique_ptr<AudioStream> AudioStream::CreateOfAudioStream( u32 sample_rate, const AudioStreamParameters& parameters )
{
    std::unique_ptr<OfAudioStream> stream = std::make_unique<OfAudioStream>( sample_rate, parameters );

    if( !stream->OpenDevice() )
    {
        stream.reset();
    }

    return stream;
}



OfAudioStream::OfAudioStream( u32 sample_rate, const AudioStreamParameters& parameters )
  : AudioStream( sample_rate, parameters )
{
}



OfAudioStream::~OfAudioStream()
{
    OfAudioStream::CloseDevice();
}



bool OfAudioStream::OpenDevice()
{
    m_stream = new SpuPlayer( this );
    m_stream->setup();

    BaseInitialize();

    return true;
}



void OfAudioStream::SetPaused( bool paused )
{
//    if (m_paused == paused)
//        return;
//
//    paused ? SDL_PauseAudioStreamDevice(m_stream) : SDL_ResumeAudioStreamDevice(m_stream);
//    m_paused = paused;
}



void OfAudioStream::CloseDevice()
{
    if( m_stream )
    {
        m_stream->quit();
        delete m_stream;
        m_stream = nullptr;
    }
}
