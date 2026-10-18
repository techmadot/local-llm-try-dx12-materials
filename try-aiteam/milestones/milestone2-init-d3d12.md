# M02: D3D12初期化、Debug Layer、エラーロギング接続

## 目的

D3D12デバイスを初期化し、Debug LayerとInfoQueue1の通知をアプリケーションログへ接続する。

## 実装範囲

- Debug Layerを初期化前に有効化する
- DXGIファクトリとD3D12デバイスを作成する
- D3D12関連のCOMオブジェクトを適切に管理する
- `src/D3D12App.h`、`src/D3D12App.cpp` などに初期化処理を分離する
- `ID3D12InfoQueue1` の通知を `trace.log` へ転送する
    - `docs/d3d12-debug-layer.md` と スキル:`configure-d3d12-debug-layer` を参照
- HRESULTと処理段階をログへ記録する
- 初期化失敗時に対話的な通知を行わず、失敗終了する

## 実装しないもの

このマイルストーンでは、以下の作業を行わない

- スワップチェーン作成
- コマンドキュー、コマンドリストを使用した描画
- PNGキャプチャ
- 三角形描画
- ImGui

## 期待する成果物

- D3D12初期化処理
- Debug Layer設定処理
- InfoQueue1通知コールバック
- 初期化・終了ログ


## 完了条件

- M01の検証条件を維持したままD3D12初期化が成功する
- Debug Layerが有効な環境でアプリケーションが正常終了する
- InfoQueue1の通知が `trace.log` に記録される
- 初期化失敗時に終了コードが0以外になる
- GPUリソースとコールバックが適切な順序で解放される
- Debug Layerに未解決のエラーや警告がない

## 参照資料

- `docs/d3d12-debug-layer.md`
- `.opencode/skills/configure-d3d12-debug-layer/SKILL.md`
- `.opencode/skills/research-microsoft-api/SKILL.md`
- `docs/api-research.md`
