# M04: バックバッファのPNGキャプチャ

## 目的

Present直前のバックバッファをReadbackリソースへコピーし、PNGファイルとして保存する。
このキャプチャファイルはこの先のマイルストーンで実行結果確認に使用する。

## 実装範囲

- `--capture-frame` と `--capture-path` を実装する
- `GetCopyableFootprints` でReadbackのサイズと行ピッチを計算する
- Readbackヒープ上のリソースを作成する
- バックバッファをReadbackへコピーする
- Fence完了後にMapし、`RowPitch` を考慮してCPUバッファへコピーする
- `stbi_write_png` でPNGを生成する
- pendingキャプチャを1件管理し、GPU完了後に回収する
- キャプチャ開始、完了、失敗をログへ記録する
- 終了時に保留中のキャプチャを回収してからリソースを解放する

## 実装しないもの

- 三角形描画
- ImGui描画
- 複数キャプチャの同時保留
- バックバッファ形式の自動変換

## 期待する成果物

- PNGキャプチャ処理
- `third_party/stb/stb_image_write.h`
- キャプチャ関連の引数処理とログ

## 完了条件

- M03までの検証条件を維持する
- `--capture-frame 1 --loop-frame 3` でPNGが生成される
- PNGのサイズが1280x720である
- `Capture: START frame=N` と `Capture: END frame=N` がログに記録される
- `RowPitch` による画像のずれがない
- キャプチャ失敗時に `Capture: FAILED` とHRESULTが記録される
- キャプチャ後もPresentと通常終了が継続する
- read ツールを使用し、キャプチャ画像を読み取る
    - M03で実装した背景色クリアの状態を観測出来ていること

## 参照資料

- `rules/application-arguments-rule.md`
- `docs/png-capture.md`
- `docs/samples/png-capture/AsyncBackBufferCaptureSample.cpp`
- `.opencode/skills/capture-backbuffer-png/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
