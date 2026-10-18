# M01: マイルストーン1

## 目的

Windowsデスクトップアプリケーションの最小構成と、以降のマイルストーンで使用するビルド・ログ基盤を整備する。

## 実装範囲

このマイルストーンでは以下の作業をおこなう。

- ディレクトリの作成:
    -  `src/`、`shaders/`、`third_party/`、`bin/`、`build/` のディレクトリ構成を作成する
- CMakeの準備:
    - `CMakeLists.txt` を作成し、Visual Studio 2022向けにC++20のWindowsアプリケーションを構成する
    - 実行体を `bin/D3D12Triangle-Hello.exe` に出力する
- 初期アプリケーションの作成:
    - `wWinMain` をエントリポイントとするウィンドウを作成する
    - クライアント領域を1280x720に設定する
    - ウィンドウメッセージを処理し、通常の終了処理を実装する
- ログ・トレース機能の実装:
    - `logtrace::Initialize()`、`Write()`、`Terminate()` を実装する
    - `bin/trace.log` にUTF-8のログを出力する
    - 起動、引数パース、初期化、メッセージループ、クリーンアップ、終了をログへ記録する
- その他
    - 引数仕様に従って `--loop-frame` を処理し、指定フレーム後に通常終了できるようにする
    - 非対話的なエラー処理を実装し、エラーをログと終了コードへ反映する

## 実装しないもの

このマイルストーンでは、以下の作業を行わない

- D3D12デバイス、スワップチェーン、コマンドキューの実装
- Debug Layerの有効化
- 三角形や画面の描画
- PNGキャプチャ
- Dear ImGui

## 期待する成果物

- `CMakeLists.txt`
- `src/main.cpp`
- `src/Logger.h`、`src/Logger.cpp`
- 必要なウィンドウ関連のソース・ヘッダー
- `bin/D3D12Triangle-Hello.exe`

## 完了条件

- 正規のCMake手順でDebugまたはReleaseビルドが成功する
- 実行体が `bin/D3D12Triangle-Hello.exe` に生成される(Debug,Releaseどちらも同じ出力先)
- スキル: `execute-app` を使用してアプリを実行
    - `bin/trace.log` がUTF-8で作成される
    - 終了コードが0である
- `--loop-frame 5` で10秒以内に終了する
- `=== APP START ===`、`Args: argc=N`、`Initialize: END (hr=0x...)`、`Message loop: exited, frameCount=N`、`Cleanup: START`、`Cleanup: END`、`=== APP END ===` が記録される
- `trace.log` に `FAILED` が存在しない
- メッセージボックスなどの入力待ちが発生しない

## 参照資料

- `README.md`
- `rules/application-arguments-rule.md`
- `rules/coding-rule.md`
- `rules/environment/cmake-environment.md`
- `rules/environment/directory-structure.md`
- `docs/logging-system.md`
- `docs/error-message-logging.md`
- `.opencode/skills/cmake-build/SKILL.md`
- `.opencode/skills/integrate-error-logging/SKILL.md`
