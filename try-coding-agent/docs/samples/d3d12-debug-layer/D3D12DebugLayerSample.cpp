#include <d3d12.h>
#include <wrl/client.h>

#include "Logger.h"

namespace
{
void __stdcall D3D12MessageCallback(
    D3D12_MESSAGE_CATEGORY category,
    D3D12_MESSAGE_SEVERITY severity,
    D3D12_MESSAGE_ID id,
    LPCSTR description,
    void* context) noexcept
{
    (void)context;
    const char* message = description != nullptr ? description : "(null)";

    if (severity == D3D12_MESSAGE_SEVERITY_ERROR)
    {
        logtrace::Write("[D3D12 ERROR] %s", message);
        return;
    }

    if (severity == D3D12_MESSAGE_SEVERITY_WARNING)
    {
        logtrace::Write("[D3D12 WARNING] %s", message);
        return;
    }
}
}

HRESULT EnableD3D12DebugLayer()
{
    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
    const HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    if (FAILED(hr))
    {
        logtrace::Write("D3D12Debug: Debug Layer unavailable (hr=0x%08lx)", hr);
        return S_FALSE;
    }

    debugController->EnableDebugLayer();
    logtrace::Write("D3D12Debug: Debug Layer enabled");
    return S_OK;
}

HRESULT RegisterD3D12InfoQueueCallback(
    ID3D12Device* device,
    Microsoft::WRL::ComPtr<ID3D12InfoQueue1>& infoQueue,
    void* context,
    DWORD& callbackCookie)
{
    if (device == nullptr)
    {
        logtrace::Write("D3D12Debug: FAILED device is null");
        return E_INVALIDARG;
    }

    HRESULT hr = device->QueryInterface(IID_PPV_ARGS(&infoQueue));
    if (FAILED(hr))
    {
        logtrace::Write("D3D12Debug: ID3D12InfoQueue1 unavailable (hr=0x%08lx)", hr);
        return hr;
    }

    hr = infoQueue->RegisterMessageCallback(
        D3D12MessageCallback,
        D3D12_MESSAGE_CALLBACK_FLAG_NONE,
        context,
        &callbackCookie);
    if (FAILED(hr))
    {
        logtrace::Write("D3D12Debug: FAILED RegisterMessageCallback (hr=0x%08lx)", hr);
        infoQueue.Reset();
        return hr;
    }

    logtrace::Write("D3D12Debug: InfoQueue1 callback registered cookie=%lu", callbackCookie);
    return S_OK;
}

void UnregisterD3D12InfoQueueCallback(
    Microsoft::WRL::ComPtr<ID3D12InfoQueue1>& infoQueue,
    DWORD callbackCookie)
{
    if (infoQueue == nullptr)
    {
        return;
    }

    const HRESULT hr = infoQueue->UnregisterMessageCallback(callbackCookie);
    if (FAILED(hr))
    {
        logtrace::Write("D3D12Debug: FAILED UnregisterMessageCallback (hr=0x%08lx)", hr);
        return;
    }

    logtrace::Write("D3D12Debug: InfoQueue1 callback unregistered cookie=%lu", callbackCookie);
}

/*
使用順序:
1. D3D12CreateDevice より前に EnableD3D12DebugLayer を呼び出す。
2. D3D12CreateDevice の後に RegisterD3D12InfoQueueCallback を呼び出す。
3. デバイスを解放する前に UnregisterD3D12InfoQueueCallback を呼び出す。

このファイルは設計確認用のサンプルであり、単独ではビルドしない。
*/
