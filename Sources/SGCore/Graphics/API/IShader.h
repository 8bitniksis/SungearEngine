//
// Created by stuka on 13.05.2023.
//
#ifndef NATIVECORE_ISHADER_H
#define NATIVECORE_ISHADER_H

#include <list>
#include <glm/matrix.hpp>

#include "SGCore/Memory/Assets/FileAsset.h"
#include "SGCore/Memory/Assets/IAssetObserver.h"

#include "IUniformBuffer.h"
#include "ITexture2D.h"
#include "ShaderDefine.h"

#include "SGCore/Memory/Assets/Materials/IMaterial.h"

namespace Core::Memory::Assets
{
    class MaterialTexture;
}

namespace Core::Graphics
{
    class IUniformBuffer;

    // todo: add various types of defines like material textures block define e.t.c.
    class IShader : public Memory::Assets::IAssetObserver
    {
    public:
        std::string m_version;

        virtual ~IShader() = default;

        virtual void destroy() = 0;

        virtual void bind() = 0;

        virtual void compile(std::shared_ptr<Memory::Assets::FileAsset> asset) = 0;

        [[nodiscard]] virtual std::int32_t getShaderUniformLocation(const std::string& uniformName) const = 0;

        void addShaderDefines(const SGShaderDefineType& shaderDefineType, const std::vector<ShaderDefine>& shaderDefines);
        void removeShaderDefine(const SGShaderDefineType& shaderDefineType, const ShaderDefine& shaderDefine);
        void removeShaderDefine(const SGShaderDefineType& shaderDefineType, const std::string& shaderDefineName);

        void replaceDefines(const SGShaderDefineType& shaderDefineType, const std::list<ShaderDefine>& otherDefines) noexcept;
        void replaceDefines(const SGShaderDefineType& shaderDefineType, std::shared_ptr<IShader> otherShader) noexcept;

        void clearDefinesOfType(const SGShaderDefineType& shaderDefineType) noexcept;

        /**
         * Calls recompile of shader program.
         */
        void onAssetModified() override;

        /**
         * Calls recompile of shader program.
         */
        void onAssetPathChanged() override;

        #pragma region Uniforms use

        virtual void useMaterialTexture(const Memory::Assets::MaterialTexture&) { };
        virtual void useUniformBuffer(const std::shared_ptr<IUniformBuffer>&) { };
        virtual void useTexture(const std::string& uniformName, const std::uint8_t& texBlock) { };

        virtual void useMatrix(const std::string& uniformName, const glm::mat4& matrix) { };

        virtual void useVectorf(const std::string& uniformName, const float& x, const float& y) { };
        virtual void useVectorf(const std::string& uniformName,
                                const float& x, const float& y, const float& z) { };
        virtual void useVectorf(const std::string& uniformName,
                                const float& x, const float& y, const float& z, const float& w) { };

        virtual void useVectorf(const std::string& uniformName, const glm::vec2& vec) { };
        virtual void useVectorf(const std::string& uniformName, const glm::vec3& vec) { };
        virtual void useVectorf(const std::string& uniformName, const glm::vec4& vec) { };

        virtual void useFloat(const std::string& uniformName, const float& f) { };

        #pragma endregion

        #pragma region Operators
        IShader& operator=(const IShader&) noexcept;
        #pragma endregion

        std::weak_ptr<Memory::Assets::FileAsset> m_fileAsset;

    protected:
        std::unordered_map<SGShaderDefineType, std::list<ShaderDefine>> m_defines;

    };
    //class IUniformType
}

#endif //NATIVECORE_ISHADER_H
