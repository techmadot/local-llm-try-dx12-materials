---
name: integrate-error-logging
description: Integrate non-interactive error reporting for external libraries and runtime notifications.
---

# エラー通知のログ接続スキル

外部ライブラリやランタイムが発生させる ASSERT、デバッグレポート、異常終了、未処理例外を、ユーザー入力待ちにせずアプリケーションのログへ接続するためのスキルです。

アプリケーションコードから `ASSERT`、`DebugReportW`、`_CrtDbgReportW`、`abort()`、`MessageBox` を直接呼び出してはなりません。
通知元が提供する設定、コールバック、フック、またはプロセス全体のハンドラーを登録して使用します。

## 使用する場面

次のいずれかに該当する場合に使用します。

- 外部ライブラリのエラー通知を `trace.log` へ接続する
- D3D12 Debug Layer の通知を `ID3D12InfoQueue1` 経由で `trace.log` へ接続する
- CRT の ASSERT やデバッグレポートでダイアログが表示される問題を修正する
- クラッシュ時にログが残らない問題を調査する
- 実行体がエージェントを巻き込んで停止する問題を防ぐ

## 作業手順

1. エラー通知の発生元を特定する
  - アプリケーションの直接呼び出しか、外部ライブラリ・CRT・Windows APIからの通知かを確認する
  - D3D12 Debug Layer の通知であれば、`.opencode/skills/configure-d3d12-debug-layer/SKILL.md` へ処理を引き渡す
  - 直接呼び出しの場合は、通知関数を追加せず、エラーを戻り値と `logtrace::Write` で扱う
2. 既存のログ経路を確認する
  - 通常経路は `logtrace::Write` を使用する
  - `logtrace::Initialize()` 前に発生する可能性がある場合は、最小限のフォールバック経路を設計する
3. 通知元の設定またはフックを初期化時に登録する
  - CRTレポートはウィンドウ表示モードを無効にする
  - 外部ライブラリは公式のコールバックまたは通知設定を使用する
  - 未処理例外はプロセス全体のハンドラーで受け取る
4. 受け取りハンドラーを実装する
  - メッセージ、種別、ファイル名、行番号、例外コードなどを抽出する
  - `logtrace::Write` またはフォールバック経路へ転送する
  - ハンドラー内から別の通知関数を呼び出さない
5. 対話的な停止を抑止する
  - メッセージボックスを使用しない
  - CRTの `_CRTDBG_MODE_WNDW` を使用しない
  - Windowsのエラーダイアログと異常終了時の対話的レポートを抑止する
6. 最小限の検証を行う
  - 正常終了時に `=== APP END ===` が記録されることを確認する
  - エラー通知時に `trace.log` へ記録されることを確認する
  - 異常終了時にダイアログが表示されず、`run-summary.log` が `FAIL` になることを確認する
  - `run-with-timeout.ps1` を使用し、エージェントの実行を直接ブロックしないことを確認する

## 編集上の制約

- このスキルの1回の作業では、1つの通知経路だけを扱う
- 通知経路が不明な場合は、推測でフックを追加せず、発生元と公式APIを調査して停止する
- 既存のログ形式と終了マーカーを変更しない
- エラー通知を抑止する場合も、エラー情報を破棄しない
- 通知ハンドラーで再帰的なログ出力や例外処理を発生させない
- `abort()` や `TerminateProcess()` を新たに追加しない

## 参照用サンプル

実装の骨格は次のファイルを参照します。

```text
docs/samples/error-message-logging/ErrorHandlingSample.cpp
```

このサンプルは設計確認用であり、CMakeのビルド対象に自動追加しません。
プロジェクトの実際の `Logger` API、初期化順序、エラー終了方針に合わせて必要な部分だけを移植します。

## 関連資料

- `docs/error-message-logging.md`
- `docs/d3d12-debug-layer.md`
- `docs/logging-system.md`
- `.opencode/skills/configure-d3d12-debug-layer/SKILL.md`
- `rules/coding-rule.md`
- `.opencode/skills/execute-app/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
