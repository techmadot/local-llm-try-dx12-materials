---
name: convert-utf8-crlf
description: PowerShellを使用してファイルをUTF-8 BOM付き・CRLFに変換する
usage: ./.opencode/skills/convert-utf8-crlf/scripts/ConvertTo-Utf8BomCrlf.ps1 <file> [<file> ...]
       .\.opencode\skills\convert-utf8-crlf\scripts\ConvertTo-Utf8BomCrlf.ps1 <file> [<file> ...]
---

# UTF-8 BOM付き・CRLFへの正規化

指定したテキストファイルのエンコードをUTF-8 BOM付きにし、改行コードをCRLFに正規化する。
テキストファイルは、どの改行コードであってもよく、スクリプト実行後にはすべてCRLFに統一される。

## 手順

1. 依頼されたファイルの作成と編集をすべて完了する。
2. UTF-8 BOM付き・CRLFへの変換が必要な変更済みテキストファイルのパスを正確に列挙する。
3. バイナリファイル、生成された依存物、第三者が管理するファイル、今回のタスクと無関係な既存ファイルを対象に含めない。
4. 対象ファイルをまとめて、付属スクリプトを1回実行する。付属スクリプトの実行パスは、このSKILLファイル内に記述したパスを使うこと.
    - 変換対象ファイルが存在しない場合はスクリプトを実行せず、その旨を報告する。
    - `./.opencode/skills/convert-utf8-crlf/scripts/ConvertTo-Utf8BomCrlf.ps1 <file> [<file> ...]`
    - `.\.opencode\skills\convert-utf8-crlf\scripts\ConvertTo-Utf8BomCrlf.ps1 <file> [<file> ...]`
    - スクリプトのパスは、このSKILL.mdを基準として解決する。空白を含むパスは引用符で囲む。
    - スクリプトファイルが存在しない、または実行できない場合は、その旨をエラーとして報告し処理を中止する。
5. 終了コードが0以外なら失敗として扱う。すべての対象ファイルの変換に成功するまで、完了を報告しない。
    - 失敗した場合は、どのファイルが変換に失敗したかを特定し、再試行または個別報告を行う。
6. 正規化したファイルを報告する。

## 制約

- 元の内容が末尾改行を持つかどうかを維持する。
- 改行コードの正規化とUTF-8 BOMの付加を除き、テキストの内容を追加または削除しない。
- .editorconfigまたは.gitattributesがBOMなしまたはLFを指定している場合はこのスキルを使用しない。それ以外でリポジトリ方針と依頼内容が矛盾する場合はリポジトリ方針を優先する。
