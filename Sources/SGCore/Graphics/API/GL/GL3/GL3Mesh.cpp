//
// Created by stuka on 30.06.2023.
//

#include "GL3Mesh.h"

#include "SGCore/Main/Core.h"
#include "SGCore/Graphics/API/IVertexBufferLayout.h"

void Core::Graphics::API::GL::GL3::GL3Mesh::prepare()
{
    //m_material = std::make_shared<Memory::Assets::Material>();

    m_vertexArray = std::shared_ptr<API::IVertexArray>(Core::Main::Core::getRenderer().createVertexArray());
    m_vertexArray->create()->bind();

    // ---------------- preparing positions -------
    m_positionsBuffer = std::shared_ptr<Core::Graphics::API::IVertexBuffer>(Core::Main::Core::getRenderer().createVertexBuffer());
    m_positionsBuffer->setUsage(SGGUsage::SGG_DYNAMIC)->create()->bind()->putData(m_positions);

    std::shared_ptr<API::IVertexBufferLayout> bufferLayout = std::shared_ptr<Core::Graphics::API::IVertexBufferLayout>(Core::Main::Core::getRenderer().createVertexBufferLayout());
    bufferLayout
            ->addAttribute(std::shared_ptr<Core::Graphics::API::IVertexAttribute>(bufferLayout->createVertexAttribute(0, "positionsAttribute", SGGDataType::SGG_FLOAT3)))
            ->prepare()->enableAttributes();
    // --------------------------------------------

    // ----- preparing uv -------------------------
    m_uvBuffer = std::shared_ptr<Core::Graphics::API::IVertexBuffer>(Core::Main::Core::getRenderer().createVertexBuffer());
    m_uvBuffer->setUsage(SGGUsage::SGG_DYNAMIC)->create()->bind()->putData(m_uv);

    bufferLayout->reset();
    bufferLayout
            ->addAttribute(std::shared_ptr<Core::Graphics::API::IVertexAttribute>(bufferLayout->createVertexAttribute(1, "UVAttribute", SGGDataType::SGG_FLOAT3)))
            ->prepare()->enableAttributes();
    // --------------------------------------------

    // ---------- preparing normals ---------------
    m_normalsBuffer = std::shared_ptr<Core::Graphics::API::IVertexBuffer>(Core::Main::Core::getRenderer().createVertexBuffer());
    m_normalsBuffer->setUsage(SGGUsage::SGG_DYNAMIC)->create()->bind()->putData(m_normals);

    bufferLayout->reset();
    bufferLayout
            ->addAttribute(std::shared_ptr<Core::Graphics::API::IVertexAttribute>(bufferLayout->createVertexAttribute(2, "normalsAttribute", SGGDataType::SGG_FLOAT3)))
            ->prepare()->enableAttributes();
    // --------------------------------------------

    // ------ preparing indices -------------------
    m_indicesBuffer = std::shared_ptr<Core::Graphics::API::IIndexBuffer>(Core::Main::Core::getRenderer().createIndexBuffer());
    m_indicesBuffer->setUsage(SGGUsage::SGG_DYNAMIC)->create()->bind()->putData(m_indices);
    // --------------------------------------------
}
