#pragma once

#include "EarthCore/Render/Texture.h"

namespace PTEarth {

    class OpenGLTexture : public Texture {
    public:
        OpenGLTexture(const std::string& path);
        OpenGLTexture(uint32_t width, uint32_t height);
        virtual ~OpenGLTexture();

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        uint32_t GetRendererID() const override { return m_RendererID; }

        void SetData(void* data, uint32_t size) override;

        void Bind(uint32_t slot = 0) const override;

        bool IsLoaded() const override { return m_IsLoaded; }

    private:
        std::string m_Path;
        bool m_IsLoaded = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        uint32_t m_InternalFormat, m_DataFormat;
    };

}
