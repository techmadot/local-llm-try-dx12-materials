---
name: capture-backbuffer-png
description: Capture a D3D12 back buffer through a readback resource and encode it as PNG.
---

# バックバッファPNGキャプチャスキル

D3D12のバックバッファをReadbackヒープへコピーし、`stb_image_write` でPNGとして保存するためのスキルです。
ImGuiの組み込み有無に依存せず、Present直前のバックバッファを対象にします。

## 手順

1. 初期化時に `GetCopyableFootprints` で行ピッチとReadbackサイズを計算する
2. 初期化時にReadbackヒープのバッファを1つ作成する
3. `--capture-frame <N>` で指定されたフレームで処理を開始する
4. `Capture: START frame=N` をログに記録する
5. バックバッファを `PRESENT` から `COPY_SOURCE` へ遷移する
6. 初期化済みのReadbackバッファへ `CopyTextureRegion` でコピーする
7. バックバッファを `PRESENT` へ戻す
8. コマンドリストを実行し、Signalしたフェンス値をpendingキャプチャへ関連付ける
9. 後続フレームで、完了済みフェンス値を持つpendingキャプチャだけを回収する
10. ReadbackバッファをMapし、`RowPitch` を考慮してCPU画像バッファへコピーする
11. `stbi_write_png` で既定の `bin/capture.png` へエンコードする
12. `Capture: END frame=N` をログに記録し、Readbackリソースを再利用する

起動時に `--capture-path <path>` が指定された場合は、その保存先を使用します。
相対パスは `(プロジェクトルート)/bin` を基準にします。
引数未指定時は、必ず `(プロジェクトルート)/bin/capture.png` を使用します。

`--capture-frame <N>` でキャプチャを実行するフレーム番号を指定します。
`--capture-frame` が指定された場合のみ、指定されたフレームでキャプチャを実行します。
未指定時はキャプチャを要求しません。

## 実装上の制約

- Readback完了前にMapやPNGエンコードを開始しない
- コピーを記録したフレームで、キャプチャのためにフェンス待機しない
- pendingキャプチャは単一。Readbackリソース、フットプリント、フェンス値、フレーム番号を保持する
- 完了していないReadbackリソースを再利用しない
- 終了時はGPU完了後にpendingキャプチャを回収する
- 初期化時に1つのReadbackリソースを作成し、再利用する
- Present中、リサイズ中、スワップチェーン再作成中にキャプチャしない
- キャプチャのためだけに、コピー記録フレーム内で追加のGPU待機を行わない
- 通常のフレーム同期に必要な `WaitForPreviousFrame` は、キャプチャの有無にかかわらず実行する
- `RowPitch` を画像幅のバイト数として仮定しない
- スワップチェーンのバックバッファ形式はセットアップ時に `DXGI_FORMAT_R8G8B8A8_UNORM` へ固定する
- `stb_image_write.h` の `STB_IMAGE_WRITE_IMPLEMENTATION` を複数の翻訳単位で定義しない
- BGRA/RGBAのチャンネル順を変換なしで混同しない
- Readback行をRGBA8の連続バッファへ変換してから `stbi_write_png` を呼び出す
- キャプチャ失敗時はHRESULTと処理段階を `Capture: FAILED ...` としてログに記録する
- メッセージボックスを使用しない

## キャプチャ要求の制御

対象フレームでは `ScreenCapture::RequestCapture()` を呼び出し、同じフレームのコマンド記録中に
`ScreenCapture::IsCaptureRequested()` が真であることを確認して `QueueCapture()` を呼び出します。
`QueueCapture()` が成功した場合にだけ要求を解除し、失敗時は `Capture: FAILED QueueCapture (hr=...)` を記録します。

終了時は `D3D12App::WaitForGpu()` でGPU完了を待機してから `ProcessCompleted()` を呼び出し、
pendingキャプチャを回収します。その後に `ScreenCapture::Shutdown()` とD3D12リソース解放を行います。

## 描画との順序

ImGuiを組み込んでいる場合も、組み込んでいない場合も、描画順序は次のとおりにする。

1. アプリケーションの描画
2. ImGuiを使用する場合はImGuiのDrawData描画
3. バックバッファのReadbackコピー
4. Present

## 検証

- 未指定時に `bin/capture.png` が生成される
- `--capture-path` で指定した保存先へPNGが生成される
- `--capture-frame` で指定したフレームでのみキャプチャが行われる
- PNGを画像ビューアで開ける
- 画像サイズがクライアント領域と一致する
- Present直前の描画内容が画像に含まれる
- 行ピッチと色チャンネルが正しい
- `Capture: START frame=N` と `Capture: END frame=N` がログに記録される
- 失敗時に `Capture: FAILED` とHRESULTが記録される
- キャプチャ後も通常のフレーム処理と終了処理が継続する

## 参照サンプル

単一pendingキャプチャでReadbackを回収する実装例は、次のファイルを参照します。

```text
docs/samples/png-capture/AsyncBackBufferCaptureSample.cpp
```

このサンプルは、初期化済みのReadbackリソースの再利用、pendingキャプチャの管理、送信フェンス値の関連付け、後続フレームでの回収、終了時の回収を示します。
設計確認用であり、CMakeのビルド対象に自動追加しません。

## 参照資料

- `.opencode/skills/research-microsoft-api/SKILL.md`
- `docs/png-capture.md`
- `docs/samples/png-capture/AsyncBackBufferCaptureSample.cpp`
- `docs/imgui_integration.md`
- `.opencode/skills/integrate-imgui/SKILL.md`
- `.opencode/skills/execute-app/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
