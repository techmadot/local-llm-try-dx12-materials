---
name: verify-app-execution
description: Verify the result of an application execution.
---

## アプリケーションの実行確認方法

このスキルは、`execute-app` スキルによるアプリケーション実行が完了した後に使用します。
このスキルではアプリケーションを起動しません。

実行結果として、次のファイルを確認します。

```text
bin/run-summary.log
bin/trace.log
```

## 確認項目

`bin/run-summary.log` に次の内容があることを確認します。

- `Result: PASS`
- `NaturalExit: True`
- `Killed: False`
- `ExitCode: 0`

`bin/trace.log` に次の記録があることを確認します。

- `=== APP START ===`
- `Initialize: END (hr=0x...)`
- `Message loop: exited, frameCount=N`
- `Cleanup: START`
- `Cleanup: END`
- `=== APP END ===`

次のいずれかに該当する場合は、実行確認を失敗とします。

- `Result: FAIL` が記録されている
- タイムアウトで強制終了されている
- プロセスの終了コードが `0` 以外である
- `trace.log` が存在しない、または空である
- `trace.log` に `FAILED` が記録されている
- `trace.log` に `=== APP END ===` がない

PNGキャプチャを要求した実行では、次も確認します。

- 既定の保存先を使用した場合、`bin/capture.png` が存在する
- `--capture-path` を指定した場合、指定した保存先にPNGが存在する
- PNGを画像ビューアで開ける
- `Capture: START frame=N` と `Capture: END frame=N` が `trace.log` に記録されている

確認結果は、アプリケーションの再実行ではなく、確認したログとサマリーの内容に基づいて報告します。
