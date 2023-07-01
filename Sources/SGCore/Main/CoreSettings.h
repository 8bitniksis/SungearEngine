//
// Created by stuka on 30.06.2023.
//

#pragma once

#ifndef SUNGEARENGINE_CORESETTINGS_H
#define SUNGEARENGINE_CORESETTINGS_H

#include "assimp/postprocess.h"

namespace Core::Main::CoreSettings
{
    namespace ModelsImport
    {
        static inline const int IMPORTER_FLAGS = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_FixInfacingNormals;
    }
}

#endif //SUNGEARENGINE_CORESETTINGS_H
