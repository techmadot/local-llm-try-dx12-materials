---
name: integrate-imgui
description: Integrate Dear ImGui with the Win32 window and D3D12 renderer.
---

# ImGui 組み込みスキル

Dear ImGuiを既存のWin32/D3D12描画へ組み込むためのスキルです。

## 組み込み前の確認

1. 公式リポジトリから取得したDear ImGuiが `third_party/imgui/` に存在することを確認する
2. リリースタグ、コミットID、取得元URLが依存関係記録に固定されていることを確認する
3. `imgui.cpp`、描画モジュール、Win32/DX12バックエンドのバージョンが同一であることを確認する
4. CMakeがビルド時に必要なソースをターゲットへ追加していることを確認する
5. バックエンドAPIの変更がある場合は、Microsoft Learn MCPで確認したWin32/D3D12 API仕様と、同梱バックエンドのAPIを照合する

@`docs/imgui_integration.md` にも注意点をまとめてある。
こちらも読み込んで確認すること。


## 手順

1. `third_party/imgui/` に同梱されている本体と公式バックエンドのファイルを確認する
2. 既存のウィンドウ、デバイス、コマンドキュー、RTV、フレーム同期の所有箇所を特定する
3. ImGui用のShader Visible CBV/SRV/UAVディスクリプタヒープを用意する
4. D3D12デバイスとウィンドウの初期化完了後にImGuiを初期化する. (`ImGui_ImplDX12_InitInfo` 構造体を使用した初期化)
5. Win32のウィンドウプロシージャから `ImGui_ImplWin32_WndProcHandler` へ入力を渡す。この関数は非公開になっているため、宣言とともに直接呼び出す。
6. 各フレームでWin32/DX12のNewFrame、UI構築、Renderを順に呼び出す
7. アプリケーション描画の後、Present前にImGuiのDrawDataを同じコマンドリストへ記録する
8. 終了時にGPU処理を待ち、ImGuiバックエンド、コンテキスト、ディスクリプタヒープを逆順で解放する

## 実装上の制約

- ImGuiをFetchContentでビルド時に取得してもよい。ただしバージョンを守ること
  - ただしライブラリとしてビルドせず、ソースを直接ターゲットへ追加する
- RTVディスクリプタヒープをImGui用SRVヒープとして渡さない
- GPU処理完了前のコマンドアロケータやフレームリソースを再利用しない
- ウィンドウプロシージャからD3D12リソースを直接操作しない
- メッセージボックスを使用しない
- Debug Layerのエラーを無視しない
- `ImGui::Render()` より前に `ImGui::GetDrawData()` を描画しない

## キャプチャとの関係

PNGキャプチャをImGui込みで取得する場合、ImGuiのDrawDataを記録した後のバックバッファをコピーする。
キャプチャの具体的なリソース遷移とReadbackは、次のスキルへ分離する。

```text
.opencode/skills/capture-backbuffer-png/SKILL.md
```

## 検証

- FPSまたは診断UIが表示される
- 入力がImGuiへ渡る
- Debug LayerにImGui由来のエラーがない
- 複数フレーム実行後もGPU同期エラーがない
- `verify-app-execution` の確認条件を満たす

## 参照資料

- `.opencode/skills/research-microsoft-api/SKILL.md`
- `docs/imgui_integration.md`
- `.opencode/skills/capture-backbuffer-png/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
