# 本アプリケーションにおけるD3D12の描画フレーム同期

## 概要

Direct3D 12の描画フレームの同期について記載します。
Direct3D 12の描画フレームの同期は難しく、本ドキュメントに従って実装してください。

## 実装内容

- バックバッファと同数の`FrameContext`構造体配列を用意する
- `FrameContext`構造体には、コマンドアロケーターと待機用のフェンス値を保持する
- スワップチェインから得られる現在のバックバッファインデックス値を使って、`FrameContext`構造体にアクセスする
- 現在のコマンドリスト・コマンドアロケーターをリセットして使用して良いかは描画フレームの先頭でチェックする
- 待機用のフェンス値は、アプリ全体で単調増加する `m_lastSignaledFenceValue` を参照して設定

## コードのサンプル

```cpp
class D3D12App
{
    UINT64 m_lastSignaledFenceValue = 0; // 同期用フェンス値(単調増加カウンタ)
    struct FrameContext
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        UINT64 submittedFenceValue = 0;	// このコンテキストで最後に投入したフェンス値.
    };
    WaitForFence(UINT64 value);
    WaitForGpuIdle();
    UINT64 SignalFence();
};


void WaitForFence(UINT64 value)
{
    if (m_fence->GetCompletedValue() >= value)
    {
        return; // 同期不要
    }
    m_fence->SetEventOnCompletion(value, m_fenceEvent);
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
}

...


void RenderFrame()
{
   auto& frameCtx = m_frameContexts[m_swapChain->GetCurrentBackBufferIndex()];
   WaitForFence(frame.submittedFenceValue);

   ... コマンドアロケーターリセットなど ...

   ... 描画処理など ...
   
   frameCtx.submittedFenceValue = SignalFence();
   m_swapChain->Present(1,0);
}

UINT64 SignalFence()
{
    ++m_lastSignaledFenceValue;
    m_commandQueue->Signal(m_fence.Get(), m_lastSignaledFenceValue);
    return m_lastSignaledFenceValue;
}

void WaitForGpuIdle()
{
    WaitForFence(SignalFence());
}
```

## 制約

- フェンスの待機において、値`0`は待機の意味を成さない
    - しかし初期フレームにおいては、コマンドアロケーターは未使用なので待機の必要が無い
    - 定常状態においては`1`以上の値が入るため、問題ない
