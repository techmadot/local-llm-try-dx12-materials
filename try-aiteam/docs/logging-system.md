# ログ・トレースシステム

## 概要

アプリケーションの動作を `bin/trace.log` に記録するシステムです。

## ファイル

| ファイル | 説明 |
|---------|------|
| `src/Logger.h` | `logtrace::Write()`, `logtrace::Initialize()`, `logtrace::Terminate()` の宣言 |
| `src/Logger.cpp` | 実装 |

## API

### `logtrace::Initialize()`

`trace.log` をオープンします。既にオープン済みの場合は何もしません。

ファイルパスはカレントディレクトリ (`bin/`) を基準に `trace.log` として解決します。

ファイルオープンに失敗した場合は、内部状態が `nullptr` のままになり、`logtrace::Write()` は何もしません。

### `logtrace::Write(const wchar_t* format, ...)`

書式付き文字列を `trace.log` に1行書き込み、フラッシュします。

`Initialize()` の後に呼び出すことを想定しています。`Initialize()` より前に呼び出しても、内部で自動オープンします。

### `logtrace::Terminate()`

`trace.log` をクローズします。プロセス終了前に呼び出すことを想定しています。

## 出力例

```
=== APP START ===
Args: argc=0
Initialize: START
Initialize: END (hr=0x00000000)
RenderFrame: frame 1
RenderFrame: frame 2
RenderFrame: frame 3
Message loop: exited, frameCount=3
Cleanup: START
Cleanup: END
=== APP END ===
```

## 制約

- スレッドセーフではありません。単一スレッドでの使用を想定しています。
- ファイルオープン失敗時は静かに無視します。
- 書式文字列はワイド文字列 (`const wchar_t*`) です。
- `_wfopen(L"trace.log", L"w, ccs=UTF-8");` のようにしてファイルを開く
    - 常に新規作成で、UTF-8出力とする
- `fprintf()`を使用しない。代わりに `fputws` を使用する。
    - ANSI出力は使えず、`wchar_t*` を引数にとる関数を使用する