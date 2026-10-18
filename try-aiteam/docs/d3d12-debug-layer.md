# Direct3D 12 デバッグレイヤー

## 概要

Direct3D 12 のデバッグレイヤーを有効化し、API の呼び出しエラーや警告を `trace.log` へ記録する仕組みです。

## ファイル

| ファイル | 説明 |
|---------|------|
| `src/D3D12Device.h` | `D3D12Device` クラスの宣言 |
| `src/D3D12Device.cpp` | デバイス生成・デバッグレイヤー設定・メッセージキャプチャの実装 |

## 実装内容

### デバッグレイヤーの有効化

`D3D12Device::Initialize()` で以下の順序で初期化を行います。

1. `D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))` で `ID3D12Debug` を取得
2. `debugController->EnableDebugLayer()` でデバッグレイヤーを有効化
3. `D3D12CreateDevice()` で `ID3D12Device` を作成
4. `m_device.As(&m_infoQueue)` で `ID3D12InfoQueue` を取得
5. `SetBreakOnCategory()` で ERROR/WARNING レベルのメッセージでブレークしないよう設定

### デバッグメッセージの出力

`D3D12Device::Shutdown()` または `D3D12Device::FlushDebugMessages()` で、キューに蓄積されたすべてのデバッグメッセージを取得し、`logtrace::Write()` で出力します。

- `D3D12_MESSAGE_SEVERITY_CORRUPTION`: `D3D12 ERROR: ` プレフィックス
- `D3D12_MESSAGE_SEVERITY_ERROR`: `D3D12 ERROR: ` プレフィックス
- `D3D12_MESSAGE_SEVERITY_WARNING`: `D3D12 WARNING: ` プレフィックス
- `D3D12_MESSAGE_SEVERITY_INFO`: `D3D12 INFO: ` プレフィックス

### フィルタリング

INFO レベルのメッセージはブレーク対象から除外します。ERROR と WARNING はブレーク対象にしますが、本アプリではログへ出力するだけにします（ブレークはしません）。

## 使用例

```cpp
D3D12Device d3d12Device;
const HRESULT hr = d3d12Device.Initialize();
// ... use device ...
d3d12Device.Shutdown();
```

## 制約

- デバッグレイヤーは `D3D12CreateDevice()` より前に有効化する必要がある
- `ID3D12InfoQueue` はデバッグレイヤーが有効な場合にのみ取得可能
- デバッグレイヤー有効時はパフォーマンスが低下する

## 参照

- [ID3D12Debug::EnableDebugLayer](https://learn.microsoft.com/windows/win32/api/d3d12sdklayers/nf-d3d12sdklayers-id3d12debug-enabledebuglayer)
- [ID3D12InfoQueue](https://learn.microsoft.com/windows/win32/api/d3d12sdklayers/nn-d3d12sdklayers-id3d12infoqueue)
- [Understanding the D3D12 Debug Layer](https://learn.microsoft.com/windows/win32/direct3d12/understanding-the-d3d12-debug-layer)
