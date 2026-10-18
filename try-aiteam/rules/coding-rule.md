# コーディングルール

## ソースコード

- ソースコード(.h/.cpp)は **BOMありの UTF-8 形式**で保存する
- ソースコードの改行コードは CRLFとする
- ソースコード内のコメントは**日本語**で記述する
- HLSLのソースコードは、日本語のコメントを使用しない。

ソースコードの変換には、スキル: `convert-utf8-crlf` が使用可能である。

## 使用言語

- アプリケーションの作成には、 C++ (C++20) を使用する
- シェーダーの記述には、 HLSL を使用する


## ログへの記録

実行中の動作をあとで確認するため、各タイミングで所定のトレースメッセージを出力する。

| タイミング | メッセージ | 意味 |
|-----------|-----------|------|
| 起動時 | `=== APP START ===` | アプリケーション開始 |
| 引数パース後 | `Args: argc=N` | 引数個数 |
| 初期化開始 | `Initialize: START` | 初期化ブロック進入 |
 記録に使用する関数については、 `docs/logging-system.md` を参照。
 ASSERT、CRTデバッグレポート、`DebugReportW`、`abort()`、未処理例外など、対話的なエラー通知を伴う処理については、 `docs/error-message-logging.md` を参照。
| 初期化終了 | `Initialize: END (hr=0x...)` | 初期化完了（HRESULT 付） |
| 各フレーム開始 | `RenderFrame: frame N` | レンダリング開始 |
| BeginFrame 失敗 | `RenderFrame: BeginFrame FAILED (0x...)` | コマンドリスト開始失敗 |
| Present 失敗 | `RenderFrame: Present FAILED (0x...)` | スワップチェーンプレゼン失敗 |
- エラー通知のためにメッセージボックスなどのユーザー入力待ちを発生させないこと。
| キャプチャ開始 | `Capture: START frame=N` | 画面キャプチャ開始 |
| キャプチャ終了 | `Capture: END frame=N` | 画面キャプチャ完了 |
| ループ終了 | `Message loop: exited, frameCount=N` | メッセージループ離脱 |
| クリーンアップ | `Cleanup: START` / `Cleanup: END` | リソース解放 |
| 終了 | `=== APP END ===` | アプリケーション終了 |

記録に使用する関数については、 `docs/logging-system.md` を参照。

## 実装時の注意事項

ユーザー入力を待つ通知手段を使ってはならない。
具体的には、メッセージボックスの利用を禁止する。
エラー情報は、**全てログに記録** すること。

