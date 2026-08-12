//
// Created by stuka on 09.05.2023.
//

#pragma once

#include "IAsset.h"
#include "SGCore/Utils/FileUtils.h"

namespace SGCore
{
    struct SGCORE_EXPORT TextFileAsset : public IAsset
    {
        SG_SERDE_AS_FRIEND()

        SG_IMPLEMENT_TYPE_ID(SGCore::TextFileAsset)

        [[nodiscard]] std::string getData() const noexcept;
    
    protected:
        void doLoad(const InterpolatedPath& path) override;

        void doLoadFromBinaryFile(AssetManager* parentAssetManager) noexcept override;

        void doReloadFromDisk(AssetsLoadPolicy loadPolicy, Ref<Threading::Thread> lazyLoadInThread) noexcept override;
        
    private:
        std::streamsize m_dataOffsetInPackage = 0;
        std::streamsize m_dataSizeInPackage = 0;

        std::string m_data;
    };
}
