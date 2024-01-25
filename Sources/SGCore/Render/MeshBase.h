//
// Created by stuka on 19.01.2024.
//

#ifndef SUNGEARENGINE_MESHBASE_H
#define SUNGEARENGINE_MESHBASE_H

#include "SGCore/ImportedScenesArch/MeshDataRenderInfo.h"
#include "SGCore/ImportedScenesArch/IMeshData.h"
#include "SGCore/Main/CoreMain.h"
#include "SGUtils/EventListenerHolder.h"

namespace SGCore
{
    struct MeshBase
    {
        MeshBase();
        // MeshBase(const MeshBase&) = default;
        MeshBase(MeshBase&&) noexcept = default;

        MeshDataRenderInfo m_meshDataRenderInfo;
        Ref<IMeshData> m_meshData;

    protected:
        std::string m_shaderPath;

        Scope<SGUtils::EventListenerHolder<void()>> m_onRenderPipelineSet = MakeScope<SGUtils::EventListenerHolder<void()>>([this]() { onRenderPipelineSet(); });

        void onMeshCreated(const std::string& shaderPath) noexcept;
        void onRenderPipelineSet() noexcept;
    };
}

#endif //SUNGEARENGINE_MESHBASE_H
