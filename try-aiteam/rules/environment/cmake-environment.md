## CMake 環境

- CMake のバージョンは 4.x である
- CMake に Pathは通っているので、`cmake --version` を実行すると結果が返ってくる

### Windowsデスクトップアプリ向け情報

- CMake + Visual Studio ジェネレーターは Windows SDK ヘッダーの「パス解決」を自動で行うが 「マクロ定義」は行わない
- `WIN32_LEAN_AND_MEAN`、`NOMINMAX`、`_CRT_SECURE_NO_WARNINGS` は `CMakeLists.txt` で `add_compile_definitions()` により明示的に定義する必要がある
- `UNICODE`, `_UNICODE` の定義マクロを使用する
- サブシステムは Windows を設定し、デスクトップアプリケーションとする
    - `add_executable(<target> WIN32 <sources...>)` のように `WIN32` キーワードを指定する
      （または `set_target_properties(<target> PROPERTIES WIN32_EXECUTABLE TRUE)`）
    - これを指定しないと既定でコンソールサブシステムになり、`main` エントリポイントが要求される。
      エントリポイントには `wWinMain` を使う。
- `WIN32_LEAN_AND_MEAN` によるマクロの定義により、一部のヘッダファイルが読み込まれない。以下のヘッダは明示的にインクルードしておくのが良い
    - `winuser.h` ( `IDC_***` のマクロ用)
- UTF-8のソースコード(.h/.cpp)を使用するため、オプションを指定する
    - `add_compile_options(/utf-8)`

### 出力先情報

`CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG` と `CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE` を使用して、
本プロジェクトルールに記載のある出力先(例: `(プロジェクトルート)/bin/(実行体)`) に出力されるように構成する。
**Visual Studio標準では、Debug,Releaseなどのフォルダがさらに作成されるため、この設定は重要である。**

### FetchContent の使用指針

- 外部ライブラリ取得には CMake FetchContent を使用する
- 必ず `GIT_SHALLOW TRUE` を指定すること
- 特定のコミットハッシュを使う場合は `GIT_SHALLOW FALSE` にするが、取得に長時間かかるため避けること
- 複数の FetchContent を使う場合は小さいリポジトリから先に宣言する

### シェーダー(HLSL)のビルド

シェーダーは`DXC`を使用し、**ビルド時にコンパイル**する。アプリケーションの実行時にコンパイルしてはならない。

- Vertex Shader: エントリポイント VSMain、ターゲット `vs_6_0`
- Pixel Shader: エントリポイント PSMain、ターゲット `ps_6_0`
- シェーダーコンパイルに失敗した場合、C++のビルドも失敗させる
- コンパイル結果は `bin/shaders/` に配置する
- 実行時コンパイルは行わない

CMake側の制約:

- HLSL シェーダーのビルド時コンパイルには `add_custom_target` を使用する
- `$<CONFIG>` の生成期評価ジェネレータを `add_dependencies` に使わない
  (`add_dependencies` は構成期に解決されるため)
- **`-Fo` の出力先ディレクトリ（例: `bin/shaders/`）は事前に存在させること。**
  DXC は出力先ディレクトリが存在しないと、エラーメッセージを一切出さずに終了コード1で失敗する
  （原因が非常に分かりにくい）。DXCコンパイルコマンドの前に
  `COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin/shaders"`
  のようなディレクトリ作成コマンドを必ず追加する。

使用するシェーダーコンパイラは、Windows SDK 同梱の `dxc.exe` とする（Vulkan SDK 等に惑わされないこと）。

- "C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\dxc.exe"

**Agility SDK との違い（混同しないこと）**：DirectX12 Agility SDK は、D3D12ランタイム本体
（`D3D12Core.dll`等）をOSのアップデートを待たずに新しいバージョンへ差し替えてアプリに同梱配布するための
仕組みであり、`dxc.exe`（HLSLコンパイラ）の代替ではない。
本プロジェクトの要件（README.md）は標準的なDirectX12機能のみで完結するため、Agility SDKの導入は不要。
必要になった場合は技術リーダーへの報告・承認を経てから検討する（README.md「依存ライブラリ」の追加ライブラリ導入ルールに従う）。
