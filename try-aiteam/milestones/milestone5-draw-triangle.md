# M05: 色付き三角形ポリゴンの描画

## 目的

頂点バッファとオフラインコンパイル済みシェーダーを使用して、RGBで色付けされた三角形を描画する。

## 実装範囲

- `shaders/TriangleVS.hlsl` と `shaders/TrianglePS.hlsl` を作成する
- DXCでVS/PSをビルド時にコンパイルし、`bin/shaders/` へ出力する
    - アプリケーションの実行時に動的コンパイルしないこと
    - `d3dcompiler.h` ヘッダファイルの利用を禁止
- 頂点バッファを作成して三角形の頂点位置・色を供給する
    -  `SV_VertexID` による三角形組み立てを使用しない
- Input Layoutを明示的に定義する
- ルートシグネチャとパイプラインステートを作成する
- パイプライン作成に使用する構造体は明示的に初期化
    - デフォルト初期化は禁止で、明示的に値を設定する
    - 設定すべき値は Microsoft Learn (MCP)を利用して調査すること
- 三角形を画面中央へ描画する
    - 座標変換は不要で、頂点はNDC空間でよい

## 実装しないもの

- PNGキャプチャ処理の変更
- ImGui
- ウィンドウリサイズ対応
- テクスチャや外部アセットの導入

## 期待する成果物

- `TriangleRenderer` の描画処理
- `shaders/TriangleVS.hlsl`
- `shaders/TrianglePS.hlsl`
- CMakeによるDXCオフラインコンパイル設定
- `bin/shaders/TriangleVS.cso`
- `bin/shaders/TrianglePS.cso`

## 完了条件

- M04までの検証条件を維持する
- 正規ビルドでシェーダーが自動コンパイルされる
- 三角形が画面中央に描画される
- 頂点バッファが使用されている
- キャプチャした画像を read ツールで読み取り、内容を検証する
    - 画像に背景色と異なる三角形が含まれる
    - 画像の三角形にRGBの頂点色が反映される
- Debug Layerにシェーダー、Input Layout、リソースステートのエラーがない
- D3D12 ERROR が発生していないこと

## 参照資料

- `README.md`
- `rules/environment/cmake-environment.md`
- `docs/png-capture.md`
- `.opencode/skills/cmake-build/SKILL.md`
- `.opencode/skills/verify-app-execution/SKILL.md`
