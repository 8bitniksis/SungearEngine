//
// Created by ilya on 13.04.24.
//

#pragma once

#include "IAsset.h"
#include "SGCore/Memory/AssetsPackage.h"

SG_PREDECLARE_SERDE()

namespace SGCore
{
    struct SGCORE_EXPORT ByteFileAsset : public IAsset
    {
        SG_IMPLEMENT_TYPE_ID(SGCore::ByteFileAsset)
        SG_SERDE_AS_FRIEND()

        ~ByteFileAsset() override;
        
        [[nodiscard]] char* getDataBuffer() const noexcept;
        [[nodiscard]] size_t getDataBufferSize() const noexcept;
    
    protected:
        void doLoad(const InterpolatedPath& path) override;
        void doReloadFromDisk(AssetsLoadPolicy loadPolicy, Ref<Threading::Thread> lazyLoadInThread) noexcept override;

        void doLoadFromBinaryFile(AssetManager* parentAssetManager) noexcept override;
        
        char* m_dataBuffer = nullptr;
        size_t m_dataBufferSize = 0;

        AssetsPackage::DataMarkup m_dataMarkupInPackage;
    };
}
