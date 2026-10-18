# アプリケーション起動引数ルール

## 対象

この文書で扱うのは、`bin/D3D12Triangle-Hello.exe` 自身が受け付ける起動引数です。

PowerShellの実行ラッパーが受け付ける `-TimeoutSeconds`、 `-ExePath`、 `-AppArgs` とは別の規約です。ラッパーを使用する場合は、アプリケーション引数を `-AppArgs` の文字列として渡します。

```powershell
.\.opencode\skills\execute-app\scripts\run-with-timeout.ps1 -AppArgs "--loop-frame 3 --capture-frame 1 --capture-path capture.png"
```

## 引数一覧

| 引数 | 値 | 既定値 | 説明 |
|---|---|---|---|
| `--loop-frame <N>` | 正の整数 | `3` | ウィンドウメッセージ処理と描画を行うフレーム数。指定したフレーム数の処理後に通常終了する |
| `--capture-frame <N>` | 0以上の整数 | なし | キャプチャを実行するフレーム番号を指定する。指定したフレームでバックバッファをPNGとして保存する |
| `--capture-path <path>` | パス | なし | Present直前のバックバッファをPNGとして保存する。相対パスは実行体のある `bin` ディレクトリを基準にする |

`--loop-frame` が指定されないとき、ウィンドウをユーザー操作で閉じるまでアプリが動作する。
`--capture-frame` が指定された場合のみ、指定されたフレームでキャプチャを実行する。
`--capture-frame` 未指定時はキャプチャを要求しない。
`--capture-frame` を指定した場合は、`--capture-path` で保存先を指定できる。
`--capture-path` 未指定時の既定の保存先は `bin/capture.png` である。

`--loop-frame` は `--capture-frame` よりも大きい必要がある。

## 使用例

実行体を3フレーム処理して終了する。

```text
D3D12Triangle-Hello.exe --loop-frame 3
```

10フレーム処理して終了する。

```text
D3D12Triangle-Hello.exe --loop-frame 10
```

3フレーム処理し、フレーム1でキャプチャして `bin/capture.png` を生成する。

```text
D3D12Triangle-Hello.exe --loop-frame 3 --capture-frame 1 --capture-path capture.png
```

## パースとエラー処理

- 起動直後に引数をパースし、`Args: argc=N` をログへ記録する
- 未知の引数、値が不足している引数、数値として解釈できない値はエラーとしてログへ記録する
- 引数エラーでメッセージボックスや対話入力を発生させない
- 引数エラー時は通常の初期化や描画へ進まず、失敗を示す終了コードを返す
- 引数の意味や既定値を暗黙に変更せず、変更時はこの文書と実装を同時に更新する
- `--capture-frame` が指定され、`--loop-frame` が `--capture-frame` 以下の場合はエラーとする

## パスと実行環境

アプリケーションは起動時に実行体のパスを取得し、その親ディレクトリをカレントディレクトリに設定する。そのため、相対パスの引数はプロジェクトルートではなく実行体のディレクトリを基準に解決する。通常の配置ではこのディレクトリが `bin` になる。

- `--capture-path capture.png` は `bin/capture.png` を指定する
- `--capture-path captures/test.png` は `bin/captures/test.png` を指定する
- 絶対パスは指定された絶対パスとして扱う

保存先の親ディレクトリを自動作成するかどうかは、実装時に決定してこの文書へ追記する。作成しない場合は、存在しない親ディレクトリをエラーとしてログへ記録する。

## 関連資料

- `.opencode/skills/execute-app/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
- `.opencode/skills/capture-backbuffer-png/SKILL.md`
- `docs/png-capture.md`
- `rules/coding-rule.md`
