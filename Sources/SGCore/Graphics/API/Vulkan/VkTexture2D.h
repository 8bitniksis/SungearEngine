//
// Created by stuka on 07.07.2023.
//

#ifndef SUNGEARENGINE_VKTEXTURE2D_H
#define SUNGEARENGINE_VKTEXTURE2D_H

#include "SGCore/Graphics/API/ITexture2D.h"

namespace Core::Graphics
{
    // TODO: impl VkTexture2D
    class VkTexture2D : public ITexture2D
    {
    public:
        ~VkTexture2D() noexcept;

        void create(std::weak_ptr<Memory::Assets::Texture2DAsset>) noexcept final;
        void destroy() noexcept final;

        void onAssetModified() noexcept final;
        void onAssetPathChanged() noexcept final;
        void onAssetDeleted() noexcept final;
        void onAssetRestored() noexcept final;

        void bind(const std::uint8_t& textureUnit) noexcept final;

        VkTexture2D& operator=(const std::shared_ptr<ITexture2D>& other) final;
    };
}

#endif //SUNGEARENGINE_VKTEXTURE2D_H