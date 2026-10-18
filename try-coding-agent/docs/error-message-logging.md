# エラーメッセージ・ロギング

## 概要

CRTのASSERT、デバッグレポート、未処理例外、Windowsのエラーダイアログなどを、対話的な通知なしに `trace.log` へ接続する仕組みです。

## ファイル

| ファイル | 説明 |
|---------|------|
| `src/ErrorHandling.h` | `InitializeNonInteractiveErrorHandling()` の宣言 |
| `src/ErrorHandling.cpp` | 実装 |

## 実装内容

`InitializeNonInteractiveErrorHandling()` は以下の設定を行います。

| 設定 | API | 効果 |
|-----|-----|------|
| システムダイアログ抑制 | `SetErrorMode(SEM_FAILCRITICALERRORS \| SEM_NOGPFAULTERRORBOX \| SEM_NOOPENFILEERRORBOX)` | クリティカルエラー、Windows Error Reporting、ファイルオープンエラーのダイアログを非表示 |
| abort() 動作変更 | `_set_abort_behavior(0, _CALL_REPORTFAULT)` | crash dump 報告ダイアログを無効化 |
| CRT リポートフック | `_CrtSetReportHookW2(_CRT_RPTHOOK_INSTALL, CrtReportHook)` | ASSERT/エラー/警告をダイアログではなくフック経由で取得 |
| 未処理例外フィルタ | `SetUnhandledExceptionFilter(UnhandledExceptionHandler)` | 未処理例外をフック経由で取得 |

## フック関数

### `CrtReportHook(int reportType, wchar_t* message, int* returnValue)`

CRTの `_CrtDbgReportW` から呼び出されます。

- `reportType`: `_CRT_WARN`, `_CRT_ERROR`, `_CRT_ASSERT`
- `message`: フォーマット済みのメッセージ
- `returnValue`: フックが設定する戻り値

戻り値に `1` (TRUE) を返すと、それ以上のデバッグ出力が行われなくなります。

### `UnhandledExceptionHandler(EXCEPTION_POINTERS* exceptionPointers)`

Windowsの未処理例外ハンドラ。

- `EXCEPTION_EXECUTE_HANDLER` を返すことでプロセスを終了させます。

## 使用上の注意

- アプリケーションコードから `ASSERT`, `DebugReportW`, `_CrtDbgReportW`, `abort()`, `MessageBox` を直接呼び出してはなりません。
- エラーは戻り値 (`HRESULT`, `BOOL` など) と `logtrace::Write()` で扱います。
- フック関数から別の通知関数を呼び出してはなりません (再帰ログに注意)。

## 参照

- `.opencode/skills/integrate-error-logging/SKILL.md`
- `.opencode/samples/error-message-logging/ErrorHandlingSample.cpp`
- `rules/coding-rule.md`
