//
// Created by stuka on 07.05.2023.
//

#pragma once

#ifndef NATIVECORE_MODELASSET_H
#define NATIVECORE_MODELASSET_H

#include "IAsset.h"

#include "SGCore/ImportedScenesArch/Node.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "SGCore/ECS/Scene.h"
#include "SGCore/ECS/Rendering/Mesh.h"

namespace SGCore
{
    class ModelAsset : public IAsset, public std::enable_shared_from_this<ModelAsset>
    {
    public:
        std::vector<std::shared_ptr<Node>> m_nodes;

        std::shared_ptr<IAsset> load(const std::string&) override;

    private:
        // local import flags
        // TODO: maybe reimport after change flags
        int m_importerFlags = 0;

        // model name
        std::string m_name;

        std::shared_ptr<Node> processNode(const aiNode*, const aiScene*);
        std::shared_ptr<IMeshData> processMesh(const aiMesh*, const aiScene*);
        void loadTextures(aiMaterial* aiMat, std::shared_ptr<IMaterial>& sgMaterial, const aiTextureType& aiTexType, const SGTextureType& sgMaterialTextureType);
    };
}

#endif //NATIVECORE_MODELASSET_H
