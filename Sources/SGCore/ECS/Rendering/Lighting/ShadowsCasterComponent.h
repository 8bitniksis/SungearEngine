//
// Created by stuka on 27.07.2023.
//

#ifndef SUNGEARENGINE_SHADOWSCASTERCOMPONENT_H
#define SUNGEARENGINE_SHADOWSCASTERCOMPONENT_H

//#include "SGCore/ECS/Rendering/ShadowsCasterSystem.h"

#include "SGCore/Graphics/API/IShader.h"
#include "SGCore/Graphics/API/IFrameBuffer.h"

#include "SGCore/ECS/Rendering/IRenderingComponent.h"
#include "SGCore/Graphics/API/IRenderer.h"
#include "SGCore/Main/CoreMain.h"

#include "SGCore/Graphics/Defines.h"
#include "SGCore/Memory/Assets/ShaderAsset.h"

namespace Core::Graphics
{
    class IRenderer;
}

namespace Core::Main
{
    class CoreMain;
}

namespace Core::ECS
{
    class ShadowsCasterComponent : public IRenderingComponent
    {
        friend class ShadowsCasterSystem;

    public:

        // frame buffer with depth attachment
        std::shared_ptr<Core::Graphics::IFrameBuffer> m_frameBuffer =
                std::shared_ptr<Core::Graphics::IFrameBuffer>(Main::CoreMain::getRenderer().createFrameBuffer())
                ->create()
                ->setSize(1024 * 2, 1024 * 2)
                ->addAttachment(SGFrameBufferAttachmentType::SGG_DEPTH_ATTACHMENT,
                                SGGColorFormat::SGG_DEPTH_COMPONENT,
                                SGGColorInternalFormat::SGG_DEPTH_COMPONENT32F,
                                0,
                                0)
                                ->unbind();

    private:
        void init() noexcept final { }
    };
}

#endif //SUNGEARENGINE_SHADOWSCASTERCOMPONENT_H
