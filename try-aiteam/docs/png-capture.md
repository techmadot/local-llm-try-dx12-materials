# バックバッファのPNGキャプチャ

本プロジェクトでは、D3D12のバックバッファをReadbackヒープへコピーし、`stb_image_write` でPNGファイルとして保存する。
ImGuiの有無に関係なく、Present直前のバックバッファをキャプチャする。

## 保存先とログ

引数未指定時の既定の保存先は、`trace.log` と同じ `(プロジェクトルート)/bin/capture.png` とする。

```text
bin/capture.png
```

起動時引数で保存先が指定された場合は、そのパスを使用する。
引数名は `--capture-path <path>` とし、相対パスは実行体のある `(プロジェクトルート)/bin` ディレクトリを基準に解決する。
絶対パスはそのまま使用する。

```text
--capture-path custom-capture.png
```

`--capture-frame <N>` でキャプチャを実行するフレーム番号を指定する。
`--capture-frame` が指定された場合のみ、指定されたフレームでキャプチャを実行する。
未指定時はキャプチャを要求しない。

```text
--capture-frame 1
```

`--loop-frame` は `--capture-frame` よりも大きい必要がある。

キャプチャの前後に次のログを記録する。

```text
Capture: START frame=N
Capture: END frame=N
```

失敗時は、HRESULTと処理段階を含む `Capture: FAILED ...` を記録する。

## コピー手順

初期化時にReadbackバッファを1つ作成し、キャプチャ実行時にそれを再利用する。

1. バックバッファを `D3D12_RESOURCE_STATE_PRESENT` から `D3D12_RESOURCE_STATE_COPY_SOURCE` へ遷移する
2. `GetCopyableFootprints` でReadback用の行ピッチと必要サイズを計算する（初期化時）
3. 初期化時にReadbackヒープ上にバッファを1つ作成する
4. `CopyTextureRegion` でバックバッファをReadbackバッファへコピーする
5. バックバッファを `D3D12_RESOURCE_STATE_PRESENT` へ戻す
6. コマンドリストを実行する
7. コマンドキューへSignalしたフェンス値を、キャプチャに関連付ける
8. 後続フレームでフェンス完了を確認する
9. 完了したキャプチャだけReadbackバッファをMapする
10. 行ピッチを考慮してCPU側の画像バッファへコピーする
11. UnmapしてReadbackリソースを再利用可能な状態に戻す
12. `stbi_write_png` でPNGへエンコードする

Readback完了を待つ前にMapやPNGエンコードを開始してはならない。

## 単一pendingキャプチャの管理

キャプチャは一度に1つだけを処理する。
各キャプチャは次の情報をpending状態として保持する。

- 初期化時に作成したReadbackリソース
- `D3D12_PLACED_SUBRESOURCE_FOOTPRINT`
- コピーを含むコマンドリストを実行したフェンス値
- キャプチャ元のフレーム番号

フレーム開始時など、後続フレームの安全な時点で、フェンス完了値がpendingのフェンス値以上かを確認する。
完了済みのキャプチャだけMapしてPNGへ保存し、保存後にpending状態を解除するReadbackリソースは再利用する。

コピーを記録したフレームでは、キャプチャのために追加のフェンス待機を行わない。通常のフレーム同期はキャプチャの有無にかかわらず実行する。
終了処理では `D3D12App::WaitForGpu()` でGPU完了を待機した後、`ProcessCompleted()` で残っているpendingキャプチャを回収する。その後にキャプチャとD3D12リソースを解放する。

実装の骨格は、次のサンプルを参照する。

```text
docs/samples/png-capture/AsyncBackBufferCaptureSample.cpp
```

## バックバッファ形式

PNG保存時のチャンネル変換を不要にするため、スワップチェーン作成時にバックバッファ形式を `DXGI_FORMAT_R8G8B8A8_UNORM` へ固定する。
キャプチャ処理で別の形式を受け入れるための分岐は追加しない。

スワップチェーンの設定とキャプチャ処理で、同じ形式を使用する。

```cpp
swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
```

Readbackの行ピッチは画像幅の4バイト単位のサイズと一致するとは限らない。
`D3D12_PLACED_SUBRESOURCE_FOOTPRINT::Footprint.RowPitch` を使用して、行ごとに画像バッファへコピーする。

## stb_image_write

PNG保存には `stb_image_write.h` の `stbi_write_png` を使用する。
ヘッダーファイルは `third_party/stb/stb_image_write.h` に同梱する。
`STB_IMAGE_WRITE_IMPLEMENTATION` はプロジェクト内の1つの翻訳単位でだけ定義する。
ImGuiと同様に、使用するstbのバージョンを固定し、`third_party/imgui/` とは別の依存物として管理する。

PNGエンコードの基本処理は次のとおりとする。

1. Readbackした行を `RowPitch` からRGBA8の連続したCPUバッファへコピーする
2. `stbi_write_png(path, width, height, 4, pixels, width * 4)` を呼び出す
3. 戻り値が0の場合は `Capture: FAILED` を記録する

## キャプチャの同期

`--capture-frame <N>` で指定されたフレームでのみキャプチャを実行する。
通常のフレーム処理ではキャプチャ処理をスキップする。

対象フレームでは `RequestCapture()` を呼び出し、`IsCaptureRequested()` が真の場合だけ `QueueCapture()` を呼び出す。`QueueCapture()` が成功した場合に要求を解除する。
毎フレーム `ProcessCompleted` を呼び出し、完了済みのpendingキャプチャがあればPNG保存を実行する。キャプチャ実行後は、コマンドキューへSignalしたフェンス値を `MarkSubmitted` へ渡す。

リサイズ中、スワップチェーン再作成中、デバイスロスト検出後はキャプチャを実行しない。

## 検証

- 既定値として `bin/capture.png` が生成される
- `--capture-path` で指定した保存先へ生成される
- `--capture-frame` で指定したフレームでのみキャプチャが行われる
- PNGを画像ビューアで開ける
- 画像サイズがクライアント領域のサイズと一致する
- Present直前の描画内容が画像に含まれる
- 行ピッチによる画像のずれや色チャンネルの入れ替わりがない
- `Capture: START frame=N` と `Capture: END frame=N` がログに記録される
- キャプチャ失敗時に `Capture: FAILED` とHRESULTが記録される
- キャプチャ後も通常のPresentと終了処理が継続する
- `--capture-frame` が `--loop-frame` 以上の場合、エラーで終了する

## 関連資料

- `docs/samples/png-capture/AsyncBackBufferCaptureSample.cpp`
- `docs/imgui_integration.md`
- `docs/logging-system.md`
- `.opencode/skills/capture-backbuffer-png/SKILL.md`
