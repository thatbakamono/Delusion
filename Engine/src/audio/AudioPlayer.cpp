#include "delusion/audio/AudioPlayer.hpp"

#include "delusion/Macros.hpp"



int AudioPlayer::callback(
    const void *, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags,
    void *userData
) {
    // TODO: Support for mono, 5.1, 7.1, etc.
    // TODO: Support for other sample formats besides paFloat32.

    auto buffer = reinterpret_cast<AudioBuffer *>(userData);

    std::lock_guard<std::mutex> lock(buffer->mutex);

    auto samples = reinterpret_cast<float *>(output);

    auto availableFrames = buffer->samples.size() / 2;
    auto framesToCopy = std::min(availableFrames, static_cast<size_t>(frameCount));

    // Send samples from the buffer to the output
    for (size_t i = 0; i < framesToCopy; i++) {
        samples[i * 2] = buffer->samples.front();

        buffer->samples.pop();

        samples[i * 2 + 1] = buffer->samples.front();

        buffer->samples.pop();

        buffer->currentSampleIndex += 2;
    }

    // If there's not enough samples in the buffer, fill the rest with zeroes
    for (size_t i = framesToCopy; i < frameCount; i++) {
        samples[i * 2] = 0.0f;
        samples[i * 2 + 1] = 0.0f;
    }

    if (buffer->finishedDecoding && buffer->currentSampleIndex >= buffer->decodedSamples) {
        return paComplete;
    }

    return paContinue;
}

float AudioPlayer::readSample(
    AVFrame *frame, AVSampleFormat sampleFormat, int sampleIndex, int channelIndex, int isPlanar
) {
    if (isPlanar == 1) {
        float sample;

        switch (sampleFormat) {
            case AV_SAMPLE_FMT_U8P:
                sample =
                    static_cast<float>(reinterpret_cast<uint8_t *>(frame->extended_data[channelIndex])[sampleIndex]) /
                    static_cast<float>(std::numeric_limits<uint8_t>::max());

                break;
            case AV_SAMPLE_FMT_S16P:
                sample =
                    static_cast<float>(reinterpret_cast<int16_t *>(frame->extended_data[channelIndex])[sampleIndex]) /
                    static_cast<float>(std::numeric_limits<int16_t>::max());

                break;
            case AV_SAMPLE_FMT_S32P:
                sample =
                    static_cast<float>(reinterpret_cast<int32_t *>(frame->extended_data[channelIndex])[sampleIndex]) /
                    static_cast<float>(std::numeric_limits<int32_t>::max());

                break;
            case AV_SAMPLE_FMT_S64P:
                sample =
                    static_cast<float>(reinterpret_cast<int64_t *>(frame->extended_data[channelIndex])[sampleIndex]) /
                    static_cast<float>(std::numeric_limits<int64_t>::max());

                break;
            case AV_SAMPLE_FMT_FLTP:
                sample = reinterpret_cast<float *>(frame->extended_data[channelIndex])[sampleIndex];

                break;
            case AV_SAMPLE_FMT_DBLP:
                sample =
                    static_cast<float>(reinterpret_cast<double *>(frame->extended_data[channelIndex])[sampleIndex]);

                break;
            case AV_SAMPLE_FMT_U8:
            case AV_SAMPLE_FMT_S16:
            case AV_SAMPLE_FMT_S32:
            case AV_SAMPLE_FMT_S64:
            case AV_SAMPLE_FMT_FLT:
            case AV_SAMPLE_FMT_DBL:
                UNREACHABLE();
            default:
                UNIMPLEMENTED();
        }

        return sample;
    } else if (isPlanar == 0) {
        float sample;

        switch (sampleFormat) {
            case AV_SAMPLE_FMT_U8:
                sample = static_cast<float>(reinterpret_cast<uint8_t *>(frame->extended_data[0]
                         )[sampleIndex * 2 + channelIndex]) /
                         static_cast<float>(std::numeric_limits<uint8_t>::max());

                break;
            case AV_SAMPLE_FMT_S16:
                sample = static_cast<float>(reinterpret_cast<int16_t *>(frame->extended_data[0]
                         )[sampleIndex * 2 + channelIndex]) /
                         static_cast<float>(std::numeric_limits<int16_t>::max());

                break;
            case AV_SAMPLE_FMT_S32:
                sample = static_cast<float>(reinterpret_cast<int32_t *>(frame->extended_data[0]
                         )[sampleIndex * 2 + channelIndex]) /
                         static_cast<float>(std::numeric_limits<int32_t>::max());

                break;
            case AV_SAMPLE_FMT_S64:
                sample = static_cast<float>(reinterpret_cast<int64_t *>(frame->extended_data[0]
                         )[sampleIndex * 2 + channelIndex]) /
                         static_cast<float>(std::numeric_limits<int64_t>::max());

                break;
            case AV_SAMPLE_FMT_FLT:
                sample = reinterpret_cast<float *>(frame->extended_data[0])[sampleIndex * 2 + channelIndex];

                break;
            case AV_SAMPLE_FMT_DBL:
                sample = static_cast<float>(reinterpret_cast<double *>(frame->extended_data[0]
                )[sampleIndex * 2 + channelIndex]);

                break;
            case AV_SAMPLE_FMT_U8P:
            case AV_SAMPLE_FMT_S16P:
            case AV_SAMPLE_FMT_S32P:
            case AV_SAMPLE_FMT_S64P:
            case AV_SAMPLE_FMT_FLTP:
            case AV_SAMPLE_FMT_DBLP:
                UNREACHABLE();
            default:
                UNIMPLEMENTED();
        }

        return sample;
    } else {
        UNREACHABLE();
    }
}

void AudioPlayer::readSamples(AudioBuffer *buffer, AVCodecContext *codecContext, AVFrame *frame) {
    auto isPlanar = av_sample_fmt_is_planar(codecContext->sample_fmt);

    for (int sampleIndex = 0; sampleIndex < frame->nb_samples; sampleIndex++) {
        for (int channelIndex = 0; channelIndex < codecContext->ch_layout.nb_channels; channelIndex++) {

            auto sample = readSample(frame, codecContext->sample_fmt, sampleIndex, channelIndex, isPlanar);

            buffer->samples.push(sample);
            buffer->decodedSamples += 1;
        }
    }
}

void AudioPlayer::worker() {
    auto controlMutex = m_controlMutex;
    auto controlCommands = m_controlCommands;

    auto mutex = m_mutex;
    auto conditionVariable = m_conditionVariable;

    AVFormatContext *formatContext {};
    AVIOContext *avioContext {};
    AVCodecContext *codecContext {};

    int audioStreamIndex = -1;

    AVFrame *frame {};
    AVPacket *packet {};

    AudioBuffer *buffer {};

    size_t threshold {};

    bool waitBeforeProceeding = false;

    while (true) {
        {
            std::unique_lock<std::mutex> lock(*mutex);

            conditionVariable->wait(lock, [&controlCommands]() { return !controlCommands->empty(); });

            lock.unlock();
        }

        while (true) {
            {
                std::lock_guard<std::mutex> controlLock(*controlMutex);

                if (!controlCommands->empty()) {
                    {
                        auto &command = controlCommands->front();

                        if (command == ControlCommand::Play) {
                            formatContext = avformat_alloc_context();

                            if (formatContext == nullptr) {
                                throw std::exception();
                            }

                            auto size = m_audioClip->data().size();
                            auto *data = static_cast<unsigned char *>(av_malloc(size));

                            std::memcpy(data, m_audioClip->data().data(), size);

                            avioContext = avio_alloc_context(data, size, 0, nullptr, nullptr, nullptr, nullptr);

                            formatContext->pb = avioContext;
                            formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

                            if (avformat_open_input(&formatContext, nullptr, nullptr, nullptr) < 0) {
                                throw std::exception();
                            }

                            if (avformat_find_stream_info(formatContext, nullptr) < 0) {
                                throw std::exception();
                            }

                            for (int i = 0; i < formatContext->nb_streams; ++i) {
                                if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                                    audioStreamIndex = i;

                                    break;
                                }
                            }

                            if (audioStreamIndex == -1) {
                                throw std::exception();
                            }

                            const AVCodec *codec =
                                avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);

                            if (codec == nullptr) {
                                throw std::exception();
                            }

                            codecContext = avcodec_alloc_context3(codec);

                            if (codecContext == nullptr) {
                                throw std::exception();
                            }

                            if (avcodec_parameters_to_context(
                                    codecContext, formatContext->streams[audioStreamIndex]->codecpar
                                ) < 0) {
                                throw std::exception();
                            }

                            if (avcodec_open2(codecContext, codec, nullptr) < 0) {
                                throw std::exception();
                            }

                            frame = av_frame_alloc();

                            if (frame == nullptr) {
                                throw std::exception();
                            }

                            packet = av_packet_alloc();

                            if (packet == nullptr) {
                                throw std::exception();
                            }

                            buffer = m_buffer;

                            threshold = static_cast<size_t>(codecContext->sample_rate) * 2 * 25;

                            waitBeforeProceeding = false;

                            controlCommands->pop();
                        }

                        if (command == ControlCommand::Pause) {
                            controlCommands->pop();

                            break;
                        }

                        if (command == ControlCommand::Stop) {
                            av_packet_free(&packet);
                            av_frame_free(&frame);

                            avcodec_free_context(&codecContext);
                            avformat_close_input(&formatContext);
                            av_freep(&avioContext->buffer);
                            avio_context_free(&avioContext);

                            delete buffer;

                            buffer = {};

                            threshold = 0;

                            waitBeforeProceeding = false;

                            controlCommands->pop();

                            break;
                        }
                    }
                }
            }

            if (waitBeforeProceeding) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2500));

                waitBeforeProceeding = false;
            }

            {
                std::lock_guard<std::mutex> bufferLock(buffer->mutex);

                if (buffer->samples.size() >= threshold) {
                    waitBeforeProceeding = true;

                    continue;
                }
            }

            int readStatus = av_read_frame(formatContext, packet);

            if (readStatus == AVERROR(EOF)) {
                av_packet_free(&packet);
                av_frame_free(&frame);

                waitBeforeProceeding = false;

                buffer->finishedDecoding = true;

                break;
            }

            if (readStatus < 0) {
                throw std::exception();
            }

            if (packet->stream_index != audioStreamIndex) {
                av_packet_unref(packet);

                continue;
            }

            if (avcodec_send_packet(codecContext, packet) != 0) {
                throw std::exception();
            }

            while (true) {
                int receiveStatus = avcodec_receive_frame(codecContext, frame);

                if (receiveStatus == AVERROR(EAGAIN) || receiveStatus == AVERROR(EOF)) {
                    break;
                }

                if (receiveStatus < 0) {
                    throw std::exception();
                }

                {
                    std::lock_guard<std::mutex> bufferLock(buffer->mutex);

                    readSamples(buffer, codecContext, frame);
                }
            }
        }
    }
}

AudioPlayer::AudioPlayer() {
    auto deviceCount = Pa_GetDeviceCount();

    if (deviceCount == 0) {
        throw std::runtime_error("No audio devices found");
    }

    auto defaultOutputDeviceIndex = Pa_GetDefaultOutputDevice();

    if (defaultOutputDeviceIndex == paNoDevice) {
        throw std::runtime_error("No default audio device found");
    }

    std::thread([this]() { worker(); }).detach();
}

AudioPlayer::~AudioPlayer() {
    if (m_stream != nullptr) {
        std::lock_guard<std::mutex> lock(*m_mutex);

        {
            std::lock_guard<std::mutex> controlLock(*m_controlMutex);

            if (Pa_CloseStream(m_stream) != paNoError) {
                throw std::runtime_error("Failed to close stream");
            }

            m_stream = nullptr;
            m_buffer = nullptr;

            m_audioClip.reset();

            m_controlCommands->push(ControlCommand::Stop);
        }

        m_conditionVariable->notify_all();
    }
}

void AudioPlayer::play(std::shared_ptr<AudioClip> audioClip) {
    // TODO: Support for mono, 5.1, 7.1, etc.
    // TODO: Support for other sample formats besides paFloat32.
    // TODO: Resample if the device doesn't support audio file's sample rate, sample format or channel layout.

    if (m_stream != nullptr) {
        throw std::runtime_error("Already playing");
    }

    std::lock_guard<std::mutex> lock(*m_mutex);

    {
        std::lock_guard<std::mutex> controlLock(*m_controlMutex);

        m_audioClip = std::move(audioClip);

        const auto capacity = static_cast<size_t>(m_audioClip->sampleRate()) * 2 * 30;

        m_buffer = new AudioBuffer(capacity);

        if (Pa_OpenDefaultStream(
                &m_stream, 0, m_audioClip->channels(), paFloat32, static_cast<double>(m_audioClip->sampleRate()), 0,
                callback, m_buffer
            ) != paNoError) {
            throw std::exception();
        }

        if (Pa_StartStream(m_stream) != paNoError) {
            throw std::exception();
        }

        m_controlCommands->push(ControlCommand::Play);
    }

    m_conditionVariable->notify_all();
}

void AudioPlayer::resume() {
    std::lock_guard<std::mutex> lock(*m_mutex);

    if (m_stream == nullptr) {
        throw std::runtime_error("Nothing was playing");
    }

    if (Pa_StartStream(m_stream) != paNoError) {
        throw std::runtime_error("Failed to resume");
    }

    m_conditionVariable->notify_all();
}

void AudioPlayer::pause() {
    std::lock_guard<std::mutex> lock(*m_mutex);

    if (m_stream == nullptr) {
        throw std::runtime_error("Nothing is playing");
    }

    {
        std::lock_guard<std::mutex> controlLock(*m_controlMutex);

        if (Pa_StopStream(m_stream) != paNoError) {
            throw std::runtime_error("Failed to pause");
        }

        m_controlCommands->push(ControlCommand::Pause);
    }

    m_conditionVariable->notify_all();
}

void AudioPlayer::stop() {
    if (m_stream == nullptr) {
        throw std::runtime_error("Nothing is playing");
    }

    std::lock_guard<std::mutex> lock(*m_mutex);

    {
        std::lock_guard<std::mutex> controlLock(*m_controlMutex);

        if (Pa_CloseStream(m_stream) != paNoError) {
            throw std::runtime_error("Failed to close stream");
        }

        m_stream = nullptr;
        m_buffer = nullptr;

        m_audioClip.reset();

        m_controlCommands->push(ControlCommand::Stop);
    }

    m_conditionVariable->notify_all();
}

bool AudioPlayer::isPlaying() const {
    if (m_stream == nullptr) {
        return false;
    }

    return Pa_IsStreamActive(m_stream) == 1;
}

bool AudioPlayer::isPaused() const {
    if (m_stream == nullptr) {
        return false;
    }

    return Pa_IsStreamActive(m_stream) == 0;
}

bool AudioPlayer::isStopped() const {
    return m_stream == nullptr;
}
