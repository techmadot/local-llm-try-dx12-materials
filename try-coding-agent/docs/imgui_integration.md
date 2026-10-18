# ImGui 統合

本プロジェクトでは、Dear ImGui をWin32とD3D12へ統合し、FPSなどの状態情報を画面へ表示する。
ImGui本体と公式バックエンドは `third_party/imgui/` に同梱し、外部取得をビルド時の前提にしない。

## 取得とバージョン固定

取得元はDear ImGuiの公式リポジトリとする。
現時点では、 `1.92.8` バージョンを採用する。

```text
https://github.com/ocornut/imgui
```

取得時は移動するブランチやコミットされていない作業ツリーを使用せず、リリースタグまたは特定コミットへ固定する。
取得したファイルは、ビルド環境にネットワーク接続がなくても使用できるよう `third_party/imgui/` へ配置する。

採用したバージョンは、次の情報とともにプロジェクトの依存関係記録へ保存する。

- Dear ImGuiのリリースタグ
- GitコミットID
- 取得元URL
- 取得日
- 本プロジェクトで使用するバックエンド（Win32、DirectX 12）
- 更新理由と、更新後に実施した検証

本プロジェクトで採用する具体的なタグとコミットIDが記録されていない状態では、ImGuiのバージョンを「最新版」と表記しない。
バージョン更新時は、本文書の使用ファイル、バックエンドAPI、CMakeのソース一覧、Debug Layer、入力、終了処理を再確認する。

## ビルドへの追加

Dear ImGuiはアプリケーションの実行ファイルへ直接コンパイルし、別の実行時DLLやビルド時のネットワーク取得に依存させない。
CMakeでは、ImGuiの本体と使用する公式バックエンドだけをアプリケーションターゲットへ追加する。

```cmake
target_sources(D3D12Triangle-Hello PRIVATE
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_win32.cpp
    third_party/imgui/backends/imgui_impl_dx12.cpp
)

target_include_directories(D3D12Triangle-Hello PRIVATE
    third_party/imgui
    third_party/imgui/backends
)
```

`imgui_demo.cpp` はビルド対象に追加しない。
ImGuiのソースを別の静的ライブラリへ分離する場合も、アプリケーションターゲットからヘッダーとバックエンドへアクセスできる構成を維持する。

CMakeの実装では `FetchContent`、`ExternalProject`、ビルド時のGit取得を使用しない。ImGuiのコピーが不足している場合は、ビルドを続行せず、バージョンと配置を確認できるエラーとして扱う。

## 使用ファイル

最低限、次のファイルを使用する。

- `third_party/imgui/imgui.h`
- `third_party/imgui/imgui.cpp`
- `third_party/imgui/imgui_draw.cpp`
- `third_party/imgui/imgui_tables.cpp`
- `third_party/imgui/imgui_widgets.cpp`
- `third_party/imgui/backends/imgui_impl_win32.h/.cpp`
- `third_party/imgui/backends/imgui_impl_dx12.h/.cpp`

使用するDear ImGuiのバージョンは固定し、更新時は変更理由を記録する。

## 初期化順序

ImGuiの初期化は、次のリソースが準備できた後に行う。

1. Win32ウィンドウ
2. D3D12デバイス
3. コマンドキュー
4. RTV用ディスクリプタヒープ
5. ImGui用のShader Visible CBV/SRV/UAVディスクリプタヒープ
6. フレームごとのコマンドアロケータ

初期化では、次のバックエンドを使用する。

```cpp
ImGui::CreateContext();
ImGui_ImplWin32_Init(hwnd);

... ディスクリプタヒープの準備 ...

ImGui_ImplDX12_InitInfo initInfo{};
initInfo.Device = device;
initInfo.CommandQueue = commandQueue;
initInfo.NumFramesInFlight = numFramesInFlight;
initInfo.RTVFormat = rtvFormat;
initInfo.SrvDescriptorHeap = *outSrvHeap;
initInfo.SrvDescriptorAllocFn = SrvDescriptorAllocFn;
initInfo.SrvDescriptorFreeFn = SrvDescriptorFreeFn;
```

実際の引数と所有権は、同梱するDear ImGuiのバックエンドAPIに合わせる。

## フレーム処理

1フレームのImGui処理は、三角形などのアプリケーション描画と同じコマンドリストへ記録する。

1. `ImGui_ImplDX12_NewFrame()`
2. `ImGui_ImplWin32_NewFrame()`
3. `ImGui::NewFrame()`
4. FPSや診断情報のUIを構築する
5. `ImGui::Render()`
6. バックバッファをRender Targetへ遷移する
7. アプリケーションの描画を記録する
8. ImGui向けのディスクリプタヒープを設定する
9. `ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList)` を呼び出す
10. バックバッファをPresent用の状態へ遷移する
11. コマンドリストを実行してPresentする

画面キャプチャを行う場合は、ImGui描画を記録した後のバックバッファをコピーする。
これにより、キャプチャ画像にImGuiの表示を含める。

## Win32入力

ウィンドウプロシージャで `ImGui_ImplWin32_WndProcHandler` を呼び出し、ImGuiへ入力を転送する。
**注意点として、`ImGui_ImplWin32_WndProcHandler` 関数は非公開化されている。**
関数定義を実装から確認し、明示的な宣言を行ってから、呼び出しを実装すること。

メッセージボックスなどのユーザー入力待ちの通知手段は使用しない。
また、 WM_CREATE のタイミングでは、 `GetIO` などのImGuiの関数を使用してはならない。

**サンプルコード**:
```
// imgui_impl_win32.h で #if 0 されているため、自前で宣言する
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

...

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
    }
    ....
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
```

## ディスクリプタヒープ

ImGuiのD3D12バックエンドには、Shader VisibleなCBV/SRV/UAVヒープを渡す。
RTVヒープとImGui用ヒープを混在させない。
ヒープとディスクリプタのCPU/GPUハンドルは、ImGuiの終了まで有効にする。

## フレーム同期

ImGuiの描画に使用するコマンドアロケータとフレームリソースは、GPUの実行完了前に再利用しない。
フレームフェンスの待機とリセットは、既存のD3D12フレーム同期規約に従う。

## 終了処理

終了時は、次の順序で解放する。

1. GPU処理の完了を待つ
2. `ImGui_ImplDX12_Shutdown()`
3. `ImGui_ImplWin32_Shutdown()`
4. `ImGui::DestroyContext()`
5. ImGui用ディスクリプタヒープを解放する

## 検証

- ウィンドウへFPSまたは診断情報が表示される
- マウスとキーボード入力がImGuiへ届く
- Debug LayerにImGui由来のエラーがない
- フレーム終了後に `=== APP END ===` が記録される
- 終了時にImGuiの解放処理が完了する

## 関連資料

- `docs/png-capture.md`
- `.opencode/skills/integrate-imgui/SKILL.md`
- `.opencode/skills/capture-backbuffer-png/SKILL.md`
