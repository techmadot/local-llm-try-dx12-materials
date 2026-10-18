---
name: execute-app
description: Execute the built application.
usage: ./.opencode/skills/execute-app/scripts/run-with-timeout.ps1 [-TimeoutSeconds N] [-ExePath PATH] [-AppArgs "ARGS"]
---

## アプリケーションの実行方法

プロジェクトのルートディレクトリをカレントディレクトリにして、タイムアウト付きの補助スクリプトから実行体を起動します。
補助スクリプトが実行体を監視するため、アプリケーションが応答しなくなった場合でもエージェントの実行を継続できます。

```powershell
.\.opencode\skills\execute-app\scripts\run-with-timeout.ps1
```

実行体は `bin/D3D12Triangle-Hello.exe` に配置されている必要があります。補助スクリプトはこのパスを既定値として使用します。
シェーダーは事前に CMake でビルドし、次のファイルを生成しておきます。

```text
bin/shaders/TriangleVS.cso
bin/shaders/TrianglePS.cso
```

アプリケーションは起動時にカレントディレクトリを実行体の場所へ変更します。
そのため、ログとシェーダーは `bin` を基準にした相対パスで扱います。

実行結果は `bin/run-summary.log` に保存されます。
タイムアウト、プロセスの異常終了、`trace.log` の異常、`=== APP END ===` の欠落がある場合、スクリプトはエラー終了します。

## スクリプトの引数

補助スクリプトが受け付けるPowerShellパラメーターは次の3つです。

| パラメーター | 型 | 既定値 | 説明 |
|---|---|---|---|
| `-TimeoutSeconds` | `int` | `20` | 実行体を待機する秒数。超過するとプロセスを強制終了し、失敗として扱う |
| `-ExePath` | `string` | `bin/D3D12Triangle-Hello.exe` | 起動する実行体のパス。相対パスはプロジェクトルートをカレントディレクトリにして指定する |
| `-AppArgs` | `string` | `--loop-frame 3` | 実行体へ渡すアプリケーション引数。複数の引数を1つの文字列として指定する |

`-AppArgs` は補助スクリプト自身のオプションではなく、実行体へそのまま渡す引数列です。PowerShellでは、引数列全体を二重引用符で囲みます。
空文字列または省略時だけ `--loop-frame 3` が使用されます。アプリケーション引数を指定した場合、既定値は自動追加されません。

`-ExePath` を省略した場合は、必ず `bin/D3D12Triangle-Hello.exe` を起動します。指定した実行体が存在しない場合、起動前に失敗します。
実行体の作業ディレクトリは、指定した実行体が存在するディレクトリです。通常は `bin` になるため、アプリケーションの相対パスは `bin` を基準に解決します。

スクリプト自身へ渡すオプションと、実行体へ渡すオプションを混在させないでください。

```powershell
.\.opencode\skills\execute-app\scripts\run-with-timeout.ps1 -TimeoutSeconds 30 -AppArgs "--loop-frame 10"
```

PNGキャプチャを要求する場合は、アプリケーション引数として保存先を指定します。

```powershell
.\.opencode\skills\execute-app\scripts\run-with-timeout.ps1 -AppArgs "--loop-frame 3 --capture-path capture.png"
```

実行後は、次の値を `bin/run-summary.log` で確認します。

- `ExePath`: 実際に起動した実行体
- `AppArgs`: 実行体へ渡した引数列
- `NaturalExit`: タイムアウトによる強制終了でないこと
- `ExitCode`: 自然終了時のプロセス終了コード
- `Killed`: タイムアウトによる強制終了の有無
- `Result`: `PASS` または `FAIL`

