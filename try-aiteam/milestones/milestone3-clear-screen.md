# M03: スワップチェーン、描画ループ、画面クリア

## 目的

ウィンドウへ表示するスワップチェーンとフレーム処理を実装し、バックバッファを指定色でクリアする。

## 実装範囲

- DXGIスワップチェーンを1280x720、`DXGI_FORMAT_R8G8B8A8_UNORM` で作成する
- バックバッファとRTVディスクリプタヒープを作成する
- コマンドキュー、コマンドアロケータ、コマンドリストを作成する
- バックバッファのリソースステートを適切に遷移する
- フレームごとのコマンド記録、実行、Presentを実装する
- FenceによるGPU同期とフレームリソースの再利用制御を実装する
    - 描画フレームに関する情報を `FrameContext`構造体で管理する
    - 詳細は `docs/d3d12-frame-syncing.md` ドキュメントを参照
- `--loop-frame` に従って指定フレーム後に終了する
- `TriangleRenderer` などの責務を必要に応じて分離する

## 実装しないもの

- 三角形の頂点バッファ・シェーダー描画
- PNGキャプチャ
- ImGui

## 期待する成果物

- スワップチェインの初期化・終了
- 描画フレームを管理する `FrameContext`の作成
    - コマンドアロケーター、待機フェンス値
- 描画コマンドの実行とPresent処理    
- 描画フレームとコマンドの同期処理
- バックバッファをクリアして、画面に反映

## 完了条件

- M02までの検証条件を維持する
- 指定したフレーム数だけ描画して終了する
- `Present` とGPU同期が成功する
- Debug Layerにリソースステート、同期、解放順序のエラーがない
- 終了時にGPU処理を待機してからリソースを解放する

## 参照資料

- `docs/d3d12-debug-layer.md`
- `docs/d3d12-frame-syncing.md`
- `.opencode/skills/research-microsoft-api/SKILL.md`
- `.opencode/skills/cmake-build/SKILL.md`
- `.opencode/skills/execute-app/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
