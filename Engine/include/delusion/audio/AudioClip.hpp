#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "delusion/UniqueId.hpp"

class AudioClip {
    private:
        UniqueId m_id;

        std::vector<uint8_t> m_data;

        AVFormatContext *m_formatContext {};
        AVIOContext *m_avioContext {};
        AVCodecContext *codecContext {};

        AudioClip(
            UniqueId id, const std::vector<uint8_t> &data, AVFormatContext *formatContext, AVIOContext *avioContext,
            AVCodecContext *codecContext
        )
            : m_id(id), m_data(data), m_formatContext(formatContext), m_avioContext(avioContext),
              codecContext(codecContext) {}
    public:
        AudioClip() = delete;

        ~AudioClip();

        [[nodiscard]] static std::shared_ptr<AudioClip> create(const std::filesystem::path &path);

        [[nodiscard]] static std::shared_ptr<AudioClip> create(UniqueId id, const std::filesystem::path &path);

        [[nodiscard]] UniqueId id() const {
            return m_id;
        }

        [[nodiscard]] std::vector<uint8_t> &data() {
            return m_data;
        }

        [[nodiscard]] size_t size() {
            return m_data.size();
        }

        [[nodiscard]] size_t sampleRate() const {
            return codecContext->sample_rate;
        }

        [[nodiscard]] size_t channels() const {
            return codecContext->ch_layout.nb_channels;
        }
};
