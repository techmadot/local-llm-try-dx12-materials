# Project Milestone

本プロジェクト開発にあたり、マイルストーンを定義する。

## 共通ルール

1. マイルストーンは記載順に完了させる。
2. 後続マイルストーンの機能を先行実装しない。
3. 各マイルストーン完了時に、次の検証を必須とする。
   - 正規手順によるビルド成功
   - 正規手順によるアプリケーション実行成功
   - `trace.log` の確認（空でない、`FAILED` が存在しない）
   - `=== APP END ===` マーカの存在
   - GPU とキャプチャの保留処理を含むクリーン終了
   - タイムアウト、クラッシュ、assert がない
4. ビルド、実行、検証、レビューは Team Leader が直接実行せず、各専門エージェントへ委譲する。
5. 1 マイルストーンの製品コード追加量は、原則として 500 行以下を目安とする。超過が見込まれる場合は、マイルストーン内の Implementation Phase を再分割する。
6. 外部依存は Dear ImGui と STB のみ許可する。
7. `d3dx12.h`、`d3dx12.inl`、`glad` は使用しない。
8. DirectX 12 API は raw API を使用する。
9. 完了後は Conventional Commits に従ってコミットする。

## マイルストーン順序

1. M01: Windowsアプリケーションの基本とCMakeビルドシステムの整備、動作ロギングの実装
2. M02: DirectX12の初期化、Debug Layerの有効化とロギングシステムへの接続
3. M03: スワップチェインの初期化、描画ループを実装して画面クリアの実装
4. M04: バックバッファをコピーし、PNGファイルへの保存を実装
5. M05: 色づけされた三角形ポリゴンの描画を実装
6. M06: Dear ImGui の組み込みと描画を実装

各マイルストーン開始前に、依存する前段マイルストーンの検証完了を確認する。
この順序を変更してはならない。
キャプチャ機能が早期の実装になっているのは、後段で視覚的に読み取り機能を使って検証が必要なため。

## マイルストーン詳細

マイルストーン詳細をそれぞれ以下のファイルに記載。

- M01: `milestones/milestone1-buildup-env.md`
- M02: `milestones/milestone2-init-d3d12.md`
- M03: `milestones/milestone3-clear-screen.md`
- M04: `milestones/milestone4-capture-backbuffer.md`
- M05: `milestones/milestone5-draw-triangle.md`
- M06: `milestones/milestone6-integrate-imgui.md`
