#include <d3d12.h>
#include <wrl/client.h>

#include <cstring>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Logger.h"

namespace
{
HRESULT QueueBackBufferCapture(
    ID3D12GraphicsCommandList* commandList,
    ID3D12Resource* backBuffer,
    ID3D12Resource* readbackResource,
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& footprint,
    UINT frameNumber)
{
    D3D12_RESOURCE_BARRIER toCopySource{};
    toCopySource.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toCopySource.Transition.pResource = backBuffer;
    toCopySource.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    toCopySource.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    toCopySource.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &toCopySource);

    D3D12_TEXTURE_COPY_LOCATION source{};
    source.pResource = backBuffer;
    source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    source.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION destination{};
    destination.pResource = readbackResource;
    destination.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    destination.PlacedFootprint = footprint;
    commandList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);

    D3D12_RESOURCE_BARRIER toPresent = toCopySource;
    toPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
    toPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &toPresent);

    logtrace::Write("Capture: START frame=%u", frameNumber);
    return S_OK;
}

void MarkSubmittedCapture(
    bool& hasPendingCapture,
    UINT64& pendingFenceValue,
    UINT64 submittedFenceValue)
{
    if (hasPendingCapture)
    {
        pendingFenceValue = submittedFenceValue;
    }
}

HRESULT WriteCapturePng(
    ID3D12Resource* readbackResource,
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& footprint,
    UINT width,
    UINT height,
    UINT64 frameNumber,
    const std::filesystem::path& outputPath)
{
    void* mappedData = nullptr;
    D3D12_RANGE readRange{};
    readRange.Begin = 0;
    readRange.End = static_cast<SIZE_T>(footprint.Footprint.RowPitch) * height;
    const HRESULT hr = readbackResource->Map(0, &readRange, &mappedData);
    if (FAILED(hr))
    {
        logtrace::Write("Capture: FAILED Map (hr=0x%08lx)", hr);
        return hr;
    }

    const size_t destinationRowBytes = static_cast<size_t>(width) * 4;
    std::vector<std::uint8_t> pixels(destinationRowBytes * height);
    const auto* source = static_cast<const std::uint8_t*>(mappedData) + footprint.Offset;
    for (UINT row = 0; row < height; ++row)
    {
        memcpy(
            pixels.data() + destinationRowBytes * row,
            source + static_cast<size_t>(footprint.Footprint.RowPitch) * row,
            destinationRowBytes);
    }
    readbackResource->Unmap(0, nullptr);

    if (stbi_write_png(
            outputPath.string().c_str(),
            static_cast<int>(width),
            static_cast<int>(height),
            4,
            pixels.data(),
            static_cast<int>(destinationRowBytes)) == 0)
    {
        logtrace::Write("Capture: FAILED stbi_write_png path=%s", outputPath.string().c_str());
        return E_FAIL;
    }

    logtrace::Write("Capture: END frame=%llu", frameNumber);
    return S_OK;
}

void ProcessCompletedCapture(
    ID3D12Fence* fence,
    ID3D12Resource* readbackResource,
    const D3D12_PLACED_SUBRESOURCE_FOOTPRINT& footprint,
    UINT width,
    UINT height,
    bool& hasPendingCapture,
    UINT64 pendingFenceValue,
    UINT64 pendingFrameNumber,
    const std::filesystem::path& outputPath)
{
    if (!hasPendingCapture)
    {
        return;
    }

    if (fence->GetCompletedValue() < pendingFenceValue)
    {
        return;
    }

    WriteCapturePng(readbackResource, footprint, width, height, pendingFrameNumber, outputPath);
    hasPendingCapture = false;
}
}

/*
使用順序:
1. 初期化時に GetCopyableFootprints で行ピッチを計算し、Readbackバッファを1つ作成する。
2. フレーム開始時に ProcessCompletedCapture を呼び出し、過去フレームの完了済みキャプチャを保存する。
3. キャプチャ対象フレームで、Present直前の描画コマンドへ QueueBackBufferCapture を記録する。
4. コマンドリスト実行後にキューへSignalしたフェンス値を MarkSubmittedCapture へ渡す。
5. 同じフレームでフェンス待機、Map、PNG保存をしない。
6. 終了処理では、GPU完了後に ProcessCompletedCapture を呼び出して保留中のキャプチャを回収する。

hasPendingCapture は pending キャプチャがある時だけ true になる。
pendingFenceValue はキューへSignalしたフェンス値を保持する。
pendingFrameNumber はキャプチャ元のフレーム番号を保持する。

capturePath が未指定の場合、実行体の親ディレクトリをカレントディレクトリに設定済みであるため、通常は capture.png は (プロジェクトルート)/bin/capture.png となる。
このファイルは設計確認用のサンプルであり、単独ではビルドしない。
*/
