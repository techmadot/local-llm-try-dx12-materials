# UTF-8 BOM付きでCRLFに変換するスクリプト
param(
    [Parameter(Mandatory = $true, Position = 0, ValueFromRemainingArguments = $true)]
    [string[]]$Path
)

$ErrorActionPreference = 'Stop'

$utf8WithBom = [System.Text.UTF8Encoding]::new($true, $true)
$failedPaths = [System.Collections.Generic.List[string]]::new()

foreach ($filePath in $Path) {
    try {
        $resolvedPath = (Resolve-Path -LiteralPath $filePath -ErrorAction Stop).Path

        # BOMの有無を自動判定して読み込む。
        $content = [System.IO.File]::ReadAllText($resolvedPath)

        # CRLF、LF、単独CRをすべてCRLFへ統一する。
        $content = $content -replace "\r\n|\r|\n", "`r`n"

        [System.IO.File]::WriteAllText($resolvedPath, $content, $utf8WithBom)

        Write-Output ("変換に成功しました: {0}" -f $resolvedPath)
    }
    catch {
        $failedPaths.Add($filePath)
        Write-Error ("変換に失敗しました: {0}: {1}" -f $filePath, $_.Exception.Message) -ErrorAction Continue
    }
}

if ($failedPaths.Count -gt 0) {
    Write-Error ("変換に失敗したファイル: {0}" -f ($failedPaths -join ', ')) -ErrorAction Continue
    exit 1
}
exit 0
