# M06: Dear ImGuiの組み込みと描画

## 目的

Dear ImGuiをWin32/D3D12へ統合し、FPSと解像度を表示する情報ウィンドウを描画する。

## 実装範囲

- 固定したバージョンのDear ImGuiを `third_party/imgui/` に配置する
    - CMakeによるFetchContent、PowerShellの `Invoke-WebRequest` などでリポジトリより取得する
- ImGui本体とWin32/DX12公式バックエンドをCMakeターゲットへ追加する
- ImGui用のShader Visible SRV/UAV/CBVディスクリプタヒープを作成する
- Win32入力をImGuiへ渡す
- 各フレームでNewFrame、UI構築、Renderを実行する
- 三角形描画後、Present前にImGuiのDrawDataを記録する
- FPSと解像度を情報ウィンドウへ表示する
- GPU完了後にImGuiバックエンドとリソースを解放する

## 実装しないもの

- `imgui_demo.cpp` の組み込み
- RTVヒープのImGui用SRVヒープとしての流用
- 新たな外部依存ライブラリの導入

## 期待する成果物

- `src/ImGuiHelper.h`、`src/ImGuiHelper.cpp`
- 固定バージョンの `third_party/imgui/`
- ImGuiの依存関係記録
- FPS・解像度表示UI

## 完了条件

- M05までの検証条件を維持する
- ImGuiの情報ウィンドウが表示される
- FPSが表示される
- 解像度が表示される
- Win32入力がImGuiへ渡る
- ImGuiを含むバックバッファがPNGへ保存される
- Debug LayerにImGui由来のエラーがない
- 複数フレーム実行後もGPU同期エラーがない
- GPU完了後に全リソースが解放される

## 参照資料

- `README.md`
- `docs/imgui_integration.md`
- `.opencode/skills/integrate-imgui/SKILL.md`
- `.opencode/skills/capture-backbuffer-png/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
