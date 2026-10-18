## フォルダ構造

本プロジェクトのフォルダ構造は以下とする。

```
/（リポジトリルート）
│─ bin/                     # 実行体の出力先
│─ build/                   # ビルド・中間生成物出力
│
│─ src/                     # C/C++ ソース
│   ├─ main.cpp
│   ├─ Logger.h / Logger.cpp               # ロギングシステム
│   ├─ D3D12App.h / D3D12App.cpp         # デバイス・スワップチェーン初期化
│   ├─ TriangleRenderer.h / TriangleRenderer.cpp # 三角形描画、シェーダー読み込み
│   └─ ImGuiHelper.h / ImGuiHelper.cpp   # ImGui 初期化・FPS 表示、マウス入力受け渡し
│
│─ shaders/                # HLSL ソース（TriangleVS.hlsl、TrianglePS.hlsl）
│
│─ third_party/
│   ├─ stb/        # stb_image_write.h
│   └─ imgui/      # ImGui 必要ファイル（imgui.h/.cpp、impl_win32、impl_dx12）
│
│─ docs/                   # Markdown 形式の技術ドキュメント
│   └─ README.md          
│
│─ rules/                  # Markdown 形式のルールドキュメント
│   └─ application-arguments-rule.md
│
│─ CMakeLists.txt
```

- `bin/` フォルダ
    - Debug, Releaseフォルダを作成せず、 どちらの構成でも `bin/D3D12Triangle-Hello.exe` である。
    - 実行時ログファイルが `bin/trace.log` として出力される。
- `src/` フォルダ
    - .h/.cpp などの本プロジェクト用ソースコードを配置する。

### rules/

`rules/` フォルダは本プロジェクトで実装・ビルド・検証を行う際に守るべき規則を格納する。
サブフォルダ構成と各ファイルの内容は `../README.md`（`rules/`フォルダ直下の索引）を参照すること。

### docs/

`docs/` フォルダは実装時に参照する技術調査、過去に発生した問題、解決方法などを格納する。
どのような内容があるかは、`../../docs/README.md` を参照すること。
