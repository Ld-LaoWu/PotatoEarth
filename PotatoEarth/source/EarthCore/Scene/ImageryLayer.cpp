#include "potatopch.h"
#include "ImageryLayer.h"

namespace PTEarth {

    ImageryLayer::ImageryLayer(const std::string& name, ImageryProviderType type)
        : m_Name(name), m_ProviderType(type) {
    }

    ImageryLayer::~ImageryLayer() {
    }

    void ImageryLayer::Initialize() {
        PT_EARTH_INFO("ImageryLayer '{0}' initialized", m_Name);
    }

    void ImageryLayer::Shutdown() {
    }

    PT_Ref<Texture> ImageryLayer::GetTileTexture(const TileID& id) {
        return nullptr;
    }

    bool ImageryLayer::IsTileAvailable(const TileID& id) const {
        return false;
    }

    void ImageryLayer::ProcessLoadingQueue() {
    }

}