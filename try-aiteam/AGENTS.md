# 本プロジェクトについて

作成するアプリケーションに関する詳細は、README.md を参照してください。
本ファイルは、AIによる開発エージェント群が守るルールを記載します。

## ディレクトリ構成

本プロジェクトのディレクトリ構造は `rules/environment/directory-structure.md` に記載している。

## 各エージェント共通のルール

### 作業の開始時

全ファイルを無条件に読むのではなく、自分の責務と現在のタスクに必要な範囲だけを読むこと。

作業開始時に確認するもの:

1. `AGENTS.md`
2. Team Leader から渡された現在のタスク
3. Team Leader または前工程から渡された引き継ぎファイル
4. 現在のタスクに直接関係する `rules/`、`docs/`、`skills/`

必要に応じて確認するもの:

- `README.md`
- `Project-Milestone.md`
- `rules/README.md`（フォルダ構造は `rules/environment/directory-structure.md` を参照）
- `docs/README.md`
- `skills/**`

引き継ぎファイルが指定されている場合は、その内容をプロンプトへ再展開するのではなく、担当エージェント自身が必要な範囲を読み取ること。

### 作業の終了時

作業の終了時、各エージェントは呼び出し元に簡潔に結果を返す。
このときの書式は以下とする。

```text
STATUS: SUCCESS | FAILED | DONT_CARE
RESULT_FILE: .opencode/handoff/<role>-<task-id>.md
SUMMARY: <one-line summary>
```

- `STATUS` : 作業の完了状態を示す
    - `SUCCESS` : 正常完了
    - `FAILED` : 作業の失敗、または未完了
    - `DONT_CARE` : 本エージェントにおける作業タスクがない
- `RESULT_FILE` : 詳細な作業結果を記録した `.opencode/handoff/*.md` のパス
- `SUMMARY` : 呼び出し元が状態遷移を判断するための1行要約

**`SUMMARY` に詳細説明、長いログ、コード、調査内容を含めないこと。**
詳細説明が不要なタスクは、 `RESULT_FILE` フィールドを省略できる。

各エージェントは詳細結果を `.opencode/handoff/*.md` の引き継ぎカードへ記録し、呼び出し元へは簡潔な状態のみ返却する。
必要な場合は担当エージェントが `.opencode/handoff/` を作成する。

### エージェント間の情報引き継ぎ

エージェント間で詳細情報を長文メッセージとして直接受け渡さないこと。
次のような詳細情報は、指定された引き継ぎファイルへ記録する。

- 調査結果
- 実装内容
- 変更ファイルや変更箇所
- ビルドログ
- 実行結果
- エラー内容
- 失敗原因
- レビュー結果
- デバッグ内容
- 次工程で必要となる技術情報

前工程の詳細情報を次のエージェントへ渡す場合、内容をプロンプトへ転載してはならない。
引き継ぎファイルのパスを渡し、次のエージェント自身に必要な情報を読み込ませること。

Team Leader は原則として `RESULT_FILE` の詳細内容を読み込まない。
Team Leader が管理する情報は、状態遷移に必要な次の情報に限定する。

- `STATUS`
- `RESULT_FILE`
- Milestone ID
- Task ID
- current state
- retry count

技術的な詳細判断が必要な場合は、Team Leader 自身が解釈せず、該当する担当エージェントへ `RESULT_FILE` のパスを渡すこと。

Team Leader はサブエージェントから受け取った詳細情報を別のサブエージェント向けプロンプトへ再構成・転載しない。
状態とファイルパスのみを中継すること。

## API仕様の調査

Win32 API、DirectX 12、DXGI、D3D12、WICなど、Microsoftが仕様を管理するAPIを実装するときは、Microsoft Learnを第一優先の資料とする。
Microsoft Learn MCPが利用可能な場合は、MCP経由で公式ドキュメントを調査する。

実装前に、APIの正式なシグネチャ、呼び出し順序、HRESULT、オブジェクトやコールバックの寿命、同期、リソースステート、OS/SDK条件を確認する。
GPU同期やリソースステートなど、失敗時の影響が大きい仕様を推測で決めない。

調査手順の詳細は次を参照する。

```text
.opencode/skills/research-microsoft-api/SKILL.md
docs/api-research.md
```
