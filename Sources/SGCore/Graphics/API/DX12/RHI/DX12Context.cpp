//
// Created by 8bitniksis on 18.08.2026.
//

#include "DX12Context.h"

#if defined(_WIN32)

#include <vector>

#include "SGCore/Logger/Logger.h"
#include "SGCore/Utils/Utils.h"

bool SGCore::DX12Context::create(bool enableDebugLayer) noexcept
{
    std::uint32_t factoryFlags = 0;

    if(enableDebugLayer)
    {
        // must be enabled before the device is created, otherwise the device is created without it
        DX12Ptr<ID3D12Debug> debug;
        if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
        {
            debug->EnableDebugLayer();
            factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            m_debugLayerEnabled = true;
        }
        else
        {
            // the Graphics Tools optional feature is not installed — not fatal, just unvalidated
            SG_LOG_W("DX12: debug layer requested but unavailable (install the 'Graphics Tools' optional feature).");
        }
    }

    if(!SG_DX_CHECK(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory)))) return false;
    if(!pickAdapter()) return false;

    if(!SG_DX_CHECK(D3D12CreateDevice(m_adapter.Get(), m_featureLevel, IID_PPV_ARGS(&m_device)))) return false;

    D3D12_COMMAND_QUEUE_DESC queueDesc { };
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    if(!SG_DX_CHECK(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_directQueue)))) return false;
    setObjectName(m_directQueue.Get(), "direct_queue");

    if(m_debugLayerEnabled && SUCCEEDED(m_device.As(&m_infoQueue)))
    {
        // break on the two classes that always mean a bug in our code, as the validation layer's
        // error severity does on Vulkan
        m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        m_infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    }

    return true;
}

bool SGCore::DX12Context::pickAdapter() noexcept
{
    // high-performance preference asks DXGI for the discrete GPU on hybrid machines; WARP is only
    // taken when nothing else can reach the required feature level, so a software fallback is
    // visible in the log instead of silently costing frame time
    for(std::uint32_t index = 0;; ++index)
    {
        DX12Ptr<IDXGIAdapter1> candidate;
        if(m_factory->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                 IID_PPV_ARGS(&candidate)) == DXGI_ERROR_NOT_FOUND)
        {
            break;
        }

        DXGI_ADAPTER_DESC1 desc { };
        if(FAILED(candidate->GetDesc1(&desc))) continue;
        if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

        if(FAILED(D3D12CreateDevice(candidate.Get(), m_featureLevel, __uuidof(ID3D12Device), nullptr))) continue;
        if(FAILED(candidate.As(&m_adapter))) continue;

        m_adapterName = Utils::toUTF8(std::wstring(desc.Description));
        return true;
    }

    SG_LOG_E("DX12: no adapter supports feature level 12.0. The backend can not be used on this machine.");
    return false;
}

void SGCore::DX12Context::setObjectName(ID3D12Object* object, const std::string& name) const noexcept
{
    if(!object || !m_debugLayerEnabled) return;
    std::wstring wide;
    Utils::fromUTF8(name, wide);
    object->SetName(wide.c_str());
}

void SGCore::DX12Context::drainDebugMessages() noexcept
{
    if(!m_infoQueue) return;

    const std::uint64_t count = m_infoQueue->GetNumStoredMessages();
    std::vector<std::uint8_t> storage;

    for(std::uint64_t i = 0; i < count; ++i)
    {
        SIZE_T length = 0;
        if(FAILED(m_infoQueue->GetMessage(i, nullptr, &length)) || length == 0) continue;

        storage.resize(length);
        auto* message = reinterpret_cast<D3D12_MESSAGE*>(storage.data());
        if(FAILED(m_infoQueue->GetMessage(i, message, &length))) continue;

        const std::string text(message->pDescription, message->DescriptionByteLength);
        switch(message->Severity)
        {
            case D3D12_MESSAGE_SEVERITY_CORRUPTION:
            case D3D12_MESSAGE_SEVERITY_ERROR:
                SG_LOG_E("[DX12 debug layer] {}", text);
                break;
            case D3D12_MESSAGE_SEVERITY_WARNING:
                SG_LOG_W("[DX12 debug layer] {}", text);
                break;
            default:
                SG_LOG_I("[DX12 debug layer] {}", text);
                break;
        }
    }

    m_infoQueue->ClearStoredMessages();
}

void SGCore::DX12Context::destroy() noexcept
{
    // COM releases in reverse order on its own; the queue must go before the device
    m_infoQueue.Reset();
    m_directQueue.Reset();
    m_device.Reset();
    m_adapter.Reset();
    m_factory.Reset();
    m_adapterName.clear();
    m_debugLayerEnabled = false;
}

#endif
