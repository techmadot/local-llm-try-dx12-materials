---
name: configure-d3d12-debug-layer
description: Configure the D3D12 Debug Layer and connect ID3D12InfoQueue1 messages to the application logger.
---

# D3D12 Debug Layer 組み込みスキル

D3D12 Debug Layer を有効化し、`ID3D12InfoQueue1` のメッセージコールバックをアプリケーションのログへ接続するためのスキルです。

## 使用する場面

- D3D12デバイス初期化へDebug Layerを追加する場合
- D3D12のエラーや警告を `bin/trace.log` へ記録する場合
- D3D12のデバッグメッセージでメッセージボックスやエージェント停止が発生する場合
- InfoQueueのコールバック登録・解除を実装する場合

## 作業手順

1. D3D12デバイス初期化の所有箇所を特定する
2. `D3D12CreateDevice` より前に `D3D12GetDebugInterface` を呼び出す
3. Debug Layerを取得できた場合だけ `ID3D12Debug::EnableDebugLayer` を呼び出す
4. D3D12デバイス作成後、`ID3D12InfoQueue1` を取得する
5. `RegisterMessageCallback` でコールバックを登録する
   - コールバック型は `D3D12MessageFunc`
   - 標準設定では `D3D12_MESSAGE_CALLBACK_FLAG_NONE` を使用する
   - `callbackCookie` とコールバックコンテキストの寿命を管理する
6. コールバックからカテゴリ、重大度、ID、説明文字列を `logtrace::Write` へ転送する
   - `D3D12_MESSAGE_SEVERITY_ERROR` の場合は `D3D12Debug: FAILED` を記録する
7. D3D12デバイスを解放する前に `UnregisterMessageCallback` を呼び出す
8. `run-with-timeout.ps1` でログ、終了コード、メッセージボックスの有無を確認する

## 実装上の制約

- `D3D12CreateDevice` 後にDebug Layerを有効化しない
- アプリケーションコードから `ASSERT`、`DebugReportW`、`_CrtDbgReportW`、`abort()`、`MessageBox` を呼び出さない
- コールバック内からD3D12 APIを再入呼び出ししない
- コールバック内でメッセージボックス、待機、例外送出を行わない
- コールバックのコンテキストを解除前に破棄しない
- `stderr` や `OutputDebugStringW` をアプリケーションログの代替にしない
- Debug Layerが使用できない場合のHRESULTを握りつぶさない
- GPU-Based Validationを標準で有効化せず、必要な場合は別の明示設定として扱う

## 検証

次のログを確認します。

- Debug Layer有効化の成否
- `ID3D12InfoQueue1` 取得とコールバック登録の成否
- `D3D12Debug: FAILED` を含むエラー通知
- `=== APP END ===`
- `run-summary.log` の `Result: PASS` または期待された `FAIL`

実行は直接アプリケーションを起動せず、必ず次のスキルに従います。

```text
.opencode/skills/execute-app/SKILL.md
```

## 参照サンプル

```text
docs/samples/d3d12-debug-layer/D3D12DebugLayerSample.cpp
```

サンプルは設計確認用であり、CMakeのビルド対象に自動追加しません。
既存のD3D12初期化、Logger API、リソース所有権に合わせて必要な部分だけを移植します。

## 関連資料

- `.opencode/skills/research-microsoft-api/SKILL.md`
- `docs/d3d12-debug-layer.md`
- `docs/error-message-logging.md`
- `docs/logging-system.md`
- `.opencode/skills/execute-app/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
