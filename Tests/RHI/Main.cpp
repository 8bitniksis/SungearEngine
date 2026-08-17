//
// Created by 8bitniksis on 17.08.2026.
//
// Vertical slice of the RHI on the GL46 backend: a triangle drawn through IDevice /
// ICommandList into a legacy IFrameBuffer, its color fed through the SGSLEVulkanizer legacy
// uniform block by reflection, read back and checked. No reference images — exact pixel
// values are asserted.

#include <cstdio>
#include <cstring>
#include <csignal>
#include <stacktrace>
#include <vector>
#include <glm/vec4.hpp>

#include "SGCore/Graphics/API/GAPISelector.h"
#include "SGCore/Graphics/API/IFrameBuffer.h"
#include "SGCore/Graphics/API/IRenderer.h"
#include "SGCore/Graphics/RHI/IDevice.h"
#include "SGCore/Main/CoreMain.h"
#include "SGCore/Main/Window.h"
#include "SGCore/Utils/SGSL/SGSLEVulkanizer.h"

namespace
{
    int g_failures = 0;
    int g_exitCode = 0;
    bool g_done = false;

    void check(bool condition, const char* what) noexcept
    {
        if(!condition)
        {
            ++g_failures;
            std::printf("  FAIL: %s\n", what);
        }
    }

    // vertex: position (2 floats) + color (3 floats)
    struct Vertex
    {
        float m_x, m_y;
        float m_r, m_g, m_b;
    };

    // legacy-style GLSL: a loose uniform, no bindings — exactly what engine shaders look like
    const char* vertex_source =
        "layout(location = 0) in vec2 positionAttribute;\n"
        "layout(location = 1) in vec3 colorAttribute;\n"
        "uniform vec2 u_offset;\n"
        "out vec3 vs_color;\n"
        "void main() { vs_color = colorAttribute; gl_Position = vec4(positionAttribute + u_offset, 0.0, 1.0); }\n";

    const char* fragment_source =
        "in vec3 vs_color;\n"
        "uniform vec4 u_tint;\n"
        "void main() { gl_FragColor = vec4(vs_color, 1.0) * u_tint; }\n";

    void runSlice()
    {
        auto* device = SGCore::CoreMain::getRenderer()->getDevice();
        check(device != nullptr, "renderer exposes an RHI device");
        if(!device) { g_exitCode = 2; return; }

        const auto& properties = device->getProperties();
        std::printf("device: api=%s originBottomLeft=%d framesInFlight=%u\n",
                    std::string(gapiTypeToString(properties.m_apiType)).c_str(),
                    properties.m_originBottomLeft, properties.m_framesInFlight);

        // ---- shader: vulkanize (OpenGL dialect) then create program
        std::vector<SGCore::SGSLEVulkanizer::Stage> stages = {
            { SGCore::SST_VERTEX, vertex_source },
            { SGCore::SST_FRAGMENT, fragment_source },
        };
        SGCore::SGSLEVulkanizer::Config vulkanizeConfig;
        // OPENGL dialect for the GL backends, VULKAN (explicit set/binding, SPIR-V compiled by the
        // device) for the explicit ones
        vulkanizeConfig.m_target = isOpenGLAPI(properties.m_apiType) ? SGCore::SGSLEVulkanizer::Target::OPENGL
                                                                     : SGCore::SGSLEVulkanizer::Target::VULKAN;
        const auto report = SGCore::SGSLEVulkanizer::vulkanize(stages, vulkanizeConfig);
        check(report.m_looseUniformsMoved == 2, "2 loose uniforms moved into per-stage blocks");

        SGCore::ShaderProgramDesc programDesc;
        programDesc.m_debugName = "rhi_triangle";
        for(const auto& stage : stages) programDesc.m_stages.push_back({ stage.m_type, stage.m_code, { } });
        auto program = device->createShaderProgram(programDesc);
        check(program && program->isValid(), "shader program compiles and links");
        if(!program || !program->isValid())
        {
            std::printf("%s\n", program ? program->getLog().c_str() : "(no program)");
            g_exitCode = 2;
            return;
        }

        const auto& reflection = program->getReflection();
        const auto* vsBlock = reflection.findBinding("SGLegacyUniforms_vertex");
        const auto* fsBlock = reflection.findBinding("SGLegacyUniforms_fragment");
        check(vsBlock && fsBlock, "reflection reports both legacy blocks");
        const auto* offsetMember = reflection.findMember("SGLegacyUniforms_vertex", "u_offset");
        const auto* tintMember = reflection.findMember("SGLegacyUniforms_fragment", "u_tint");
        check(offsetMember && tintMember, "reflection reports block members");
        check(reflection.m_vertexInputs.size() == 2, "reflection reports 2 vertex inputs");
        if(!vsBlock || !fsBlock || !offsetMember || !tintMember) { g_exitCode = 2; return; }
        std::printf("reflection: vs block binding=%u size=%u, fs block binding=%u size=%u, u_offset@%u, u_tint@%u\n",
                    vsBlock->m_binding, vsBlock->m_blockSize, fsBlock->m_binding, fsBlock->m_blockSize,
                    offsetMember->m_offset, tintMember->m_offset);
        check(vsBlock->m_binding != fsBlock->m_binding, "per-stage blocks have distinct bindings");

        // ---- uniform data by reflection
        SGCore::GPUBufferDesc uboDesc;
        uboDesc.m_usage = SGCore::GPUBufferUsage::SGG_UNIFORM_BUFFER;
        uboDesc.m_access = SGCore::GPUMemoryAccess::SGG_HOST_VISIBLE;

        uboDesc.m_size = vsBlock->m_blockSize;
        uboDesc.m_debugName = "vs_legacy";
        auto vsUbo = device->createBuffer(uboDesc);
        const float offset[2] = { 0.0f, 0.0f };
        vsUbo->write(offset, sizeof(offset), offsetMember->m_offset);

        uboDesc.m_size = fsBlock->m_blockSize;
        uboDesc.m_debugName = "fs_legacy";
        auto fsUbo = device->createBuffer(uboDesc);
        const float tint[4] = { 1.0f, 0.5f, 1.0f, 1.0f };
        fsUbo->write(tint, sizeof(tint), tintMember->m_offset);

        auto descriptorSet = device->createDescriptorSet();
        descriptorSet->setUniformBuffer(vsBlock->m_binding, vsUbo);
        descriptorSet->setUniformBuffer(fsBlock->m_binding, fsUbo);

        // ---- geometry: one triangle covering the center, colored green
        const Vertex vertices[3] = {
            { -0.8f, -0.8f, 0.0f, 1.0f, 0.0f },
            {  0.8f, -0.8f, 0.0f, 1.0f, 0.0f },
            {  0.0f,  0.8f, 0.0f, 1.0f, 0.0f },
        };
        SGCore::GPUBufferDesc vboDesc;
        vboDesc.m_size = sizeof(vertices);
        vboDesc.m_usage = SGCore::GPUBufferUsage::SGG_VERTEX_BUFFER;
        vboDesc.m_access = SGCore::GPUMemoryAccess::SGG_DEVICE_LOCAL;
        vboDesc.m_debugName = "triangle_vbo";
        auto vbo = device->createBuffer(vboDesc);

        // ---- pipeline
        SGCore::PipelineStateDesc pipelineDesc;
        pipelineDesc.m_program = program;
        pipelineDesc.m_debugName = "rhi_triangle";
        pipelineDesc.m_renderState.m_useDepthTest = false;
        pipelineDesc.m_renderState.m_useStencilTest = false;
        pipelineDesc.m_blendingState.m_useBlending = false;
        pipelineDesc.m_meshRenderState.m_useFacesCulling = false;
        pipelineDesc.m_meshRenderState.m_useIndices = false;
        pipelineDesc.m_vertexInput.m_slots = { { 0, sizeof(Vertex), false } };
        pipelineDesc.m_vertexInput.m_attributes = {
            { 0, 0, SGGDataType::SGG_FLOAT, 2, offsetof(Vertex, m_x), false },
            { 1, 0, SGGDataType::SGG_FLOAT, 3, offsetof(Vertex, m_r), false },
        };
        auto pipeline = device->getOrCreatePipeline(pipelineDesc);
        check(pipeline != nullptr, "pipeline created");
        check(device->getOrCreatePipeline(pipelineDesc) == pipeline, "pipeline cache returns the same object for an equal desc");

        // ---- render target: legacy framebuffer 64x64 RGBA8
        constexpr int size = 64;
        SGCore::Ref<SGCore::IFrameBuffer> frameBuffer(SGCore::CoreMain::getRenderer()->createFrameBuffer());
        frameBuffer->setSize(size, size);
        frameBuffer->create();
        frameBuffer->bind();
        frameBuffer->addAttachment(SGFrameBufferAttachmentType::SGG_COLOR_ATTACHMENT0,
                                   SGGColorFormat::SGG_RGBA, SGGColorInternalFormat::SGG_RGBA8, SGGDataType::SGG_UNSIGNED_BYTE, 0, 0);
        frameBuffer->unbind();

        // ---- record + execute
        auto commandList = device->createCommandList();
        commandList->begin();

        SGCore::RenderPassBeginDesc pass;
        pass.m_frameBuffer = frameBuffer.get();
        pass.m_colorAttachments = { SGFrameBufferAttachmentType::SGG_COLOR_ATTACHMENT0 };
        pass.m_colorLoadOp = SGCore::LoadOp::SGG_CLEAR;
        pass.m_clearColor = { 0.0f, 0.0f, 1.0f, 1.0f };
        pass.m_width = size;
        pass.m_height = size;
        commandList->beginRenderPass(pass);

        commandList->uploadData(vbo, vertices, sizeof(vertices));
        commandList->bindPipeline(pipeline);
        commandList->bindDescriptorSet(0, descriptorSet);
        commandList->bindVertexBuffer(0, vbo);
        commandList->setViewport({ 0.0f, 0.0f, float(size), float(size) });
        commandList->draw(3);

        commandList->endRenderPass();
        commandList->end();
        device->submit(commandList);
        device->waitIdle();

        // ---- verify: center pixel = green * tint (0, 0.5, 0), corner = clear color (blue)
        SGCore::AttachmentReadback readback;
        check(frameBuffer->readAttachmentPixels(SGFrameBufferAttachmentType::SGG_COLOR_ATTACHMENT0, readback), "readback works");
        if(readback.m_data.empty()) { g_exitCode = 2; return; }
        check(readback.m_format == SGGColorFormat::SGG_RGBA && readback.m_dataType == SGGDataType::SGG_UNSIGNED_BYTE, "readback in RGBA8 as the attachment was declared");

        auto pixel = [&](int x, int y) {
            const std::size_t index = (static_cast<std::size_t>(y) * size + x) * 4;
            return glm::ivec4(readback.m_data[index], readback.m_data[index + 1], readback.m_data[index + 2], readback.m_data[index + 3]);
        };

        const auto center = pixel(size / 2, size / 2 - 4);
        const auto corner = pixel(1, size - 2);
        std::printf("center=(%d,%d,%d,%d) corner=(%d,%d,%d,%d)\n", center.r, center.g, center.b, center.a, corner.r, corner.g, corner.b, corner.a);
        check(center.r == 0 && center.g >= 126 && center.g <= 129 && center.b == 0 && center.a == 255, "center pixel is green * tint (0, ~128, 0)");
        check(corner.r == 0 && corner.g == 0 && corner.b == 255 && corner.a == 255, "corner pixel is the clear color");

        // ---- screen blit (first migrated pass): the attachment onto the window at 1:1, read the window back
        auto* renderer = SGCore::CoreMain::getRenderer().get();
        renderer->bindScreenFrameBuffer();
        renderer->renderTextureOnScreen(frameBuffer->getAttachment(SGFrameBufferAttachmentType::SGG_COLOR_ATTACHMENT0).get(), false, 0, 0, size, size);
        device->waitIdle();

        SGCore::AttachmentReadback screen;
        const bool screenRead = renderer->readScreenPixels(screen);
        check(screenRead, "screen readback works");
        if(screenRead && screen.m_width >= size && screen.m_height >= size)
        {
            auto screenPixel = [&](int x, int y) {
                const std::size_t index = (static_cast<std::size_t>(y) * screen.m_width + x) * 4;
                return glm::ivec4(screen.m_data[index], screen.m_data[index + 1], screen.m_data[index + 2], screen.m_data[index + 3]);
            };
            const auto screenCenter = screenPixel(size / 2, size / 2 - 4);
            const auto screenCorner = screenPixel(1, size - 2);
            std::printf("screen center=(%d,%d,%d,%d) corner=(%d,%d,%d,%d)\n",
                        screenCenter.r, screenCenter.g, screenCenter.b, screenCenter.a,
                        screenCorner.r, screenCorner.g, screenCorner.b, screenCorner.a);
            check(screenCenter == center, "screen blit reproduces the attachment center pixel");
            check(screenCorner == corner, "screen blit reproduces the attachment corner pixel");
        }
    }

    void onInit()
    {
        runSlice();
        std::printf("\nRHI slice: %s (%d failure(s))\n", g_failures == 0 && g_exitCode == 0 ? "PASS" : "FAIL", g_failures);
        if(g_failures != 0 && g_exitCode == 0) g_exitCode = 1;
        g_done = true;
        SGCore::CoreMain::getWindow().setShouldClose(true);
    }

    void onUpdate(double, double)
    {
        if(g_done) SGCore::CoreMain::getWindow().setShouldClose(true);
    }
}

namespace
{
    // an abort() at exit (VMA/driver asserts in Debug) is otherwise invisible in CI logs
    void onAbort(int)
    {
        std::fprintf(stderr, "abort() called, stack:\n%s\n", std::to_string(std::stacktrace::current()).c_str());
        std::fflush(stderr);
    }
}

int main(int argc, char** argv)
{
    std::signal(SIGABRT, onAbort);
    // unbuffered so crash-handler output survives when stdout is a file/pipe
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);
    for(int i = 1; i < argc; ++i)
    {
        if(std::strcmp(argv[i], "--gapi") == 0 && i + 1 < argc)
        {
            if(const auto type = SGCore::gapiTypeFromString(argv[++i])) SGCore::GAPISelector::setPreference({ *type });
        }
    }

    SGCore::CoreMain::onInit.connect<&onInit>();
    SGCore::CoreMain::getRenderTimer().onUpdate.connect<&onUpdate>();

    SGCore::CoreMain::init();
    SGCore::CoreMain::startCycle();

    return g_exitCode;
}
