#pragma once

#include <mutex>
#include <queue>

#include <portaudio.h>

#include "delusion/audio/AudioBuffer.hpp"
#include "delusion/audio/AudioClip.hpp"

class AudioPlayer {
    private:
        enum class ControlCommand {
            Play,
            Pause,
            Stop,
        };

        std::shared_ptr<AudioClip> m_audioClip {};

        PaStream *m_stream {};
        AudioBuffer *m_buffer {};

        std::shared_ptr<std::mutex> m_controlMutex = std::make_shared<std::mutex>();
        std::shared_ptr<std::queue<ControlCommand>> m_controlCommands = std::make_shared<std::queue<ControlCommand>>();

        std::shared_ptr<std::mutex> m_mutex = std::make_shared<std::mutex>();
        std::shared_ptr<std::condition_variable> m_conditionVariable = std::make_shared<std::condition_variable>();

        static int callback(
            const void *, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *,
            PaStreamCallbackFlags, void *userData
        );

        [[nodiscard]] static float readSample(
            AVFrame *frame, AVSampleFormat sampleFormat, int sampleIndex, int channelIndex, int isPlanar
        );

        static void readSamples(AudioBuffer *buffer, AVCodecContext *codecContext, AVFrame *frame);

        void worker();
    public:
        AudioPlayer();

        ~AudioPlayer();

        AudioPlayer(const AudioPlayer &) = delete;

        AudioPlayer(AudioPlayer &&) noexcept = delete;

        AudioPlayer &operator=(const AudioPlayer &) = delete;

        AudioPlayer &operator=(AudioPlayer &&) noexcept = delete;

        void play(std::shared_ptr<AudioClip> audioClip);

        void resume();

        void pause();

        void stop();

        [[nodiscard]] bool isPlaying() const;

        [[nodiscard]] bool isPaused() const;

        [[nodiscard]] bool isStopped() const;
};
