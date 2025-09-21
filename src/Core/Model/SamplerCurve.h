#pragma once
#include "FCurve.h"
#include "openGL/Texture/Texture1D.h"
#include "openGL/Texture/TextureLoader.h"

namespace curves {
    template <typename K, typename P>
    class SamplerCurve : public Curve<curve_traits<K, P>> {
    public:
        Texture1D toTexture(const int resolution, TextureWrap wrap = TextureWrap::MIRRORED_REPEAT) {
            Texture1D texture;

            const P step = this->duration() / static_cast<P>(resolution - 1);
            size_t hint = 0;
            
            std::vector<K> sampler(resolution);

            for (int i = 0; i < resolution; ++i) {
                P t = step * i;
                sampler[i] = this->at(t, hint);
            }

            ColorBufferInternalFormat iFormat;

            if constexpr (std::is_same_v<K, float>) {
                iFormat = ColorBufferInternalFormat::RED_32F;
            } else if constexpr (std::is_same_v<K, glm::vec2>) {
                iFormat = ColorBufferInternalFormat::RG_32F;
            } else if constexpr (std::is_same_v<K, glm::vec3>) {
                iFormat = ColorBufferInternalFormat::RGB_32F;
            } else if constexpr (std::is_same_v<K, glm::vec4>) {
                iFormat = ColorBufferInternalFormat::RGBA_32F;
            } else {
                static_assert(false, "Invalid Type");
            }

            texture.allocate(iFormat, internal_format_to_format(iFormat),
                internal_format_to_type(iFormat), resolution, sampler.data(),
                TextureMinFilter::LINEAR, TextureMagFilter::LINEAR, wrap, false
            );
            
            TextureLoader::writePNG(resolution, 1, channel_count(iFormat), TextureLoader::floatToUint8(sampler).data(), "gradienttex.png");
            return std::move(texture);
        }
    };
}

using SamplerCurve3 = curves::SamplerCurve<glm::vec3, double>;