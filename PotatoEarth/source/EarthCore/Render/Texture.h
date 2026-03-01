#pragma once

#include "EarthCore/Core/PotatoEarthBase.h"
#include <string>

namespace PTEarth {

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void SetData(void* data, uint32_t size) = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual bool IsLoaded() const = 0;

        static PT_Ref<Texture> Create(const std::string& path);
        static PT_Ref<Texture> Create(uint32_t width, uint32_t height);
    };

}
