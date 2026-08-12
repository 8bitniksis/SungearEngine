//
// Created by stuka on 13.12.2024.
//

#pragma once

#include <sgcore_export.h>

#include "SGCore/Utils/SGSL/SGSLESubShaderType.h"
#include "SGCore/Serde/Defines.h"

SG_PREDECLARE_SERDE()

namespace SGCore
{
    struct SGCORE_EXPORT SGSLESubShader
    {
        SG_SERDE_AS_FRIEND();

        friend struct SGSLETranslator;
        friend struct ShaderAnalyzedFile;

        [[nodiscard]] auto getType() const noexcept
        {
            return m_type;
        }

        [[nodiscard]] const auto& getCode() const noexcept
        {
            return m_code;
        }

    private:
        SGSLESubShaderType m_type = SGSLESubShaderType::SST_NONE;
        std::string m_code;

        size_t m_codeOffsetInPackage = 0;
        size_t m_codeSizeInPackage = 0;
    };
}
