param(
    [int]$TimeoutSeconds = 20,
    [string]$ExePath = "bin/D3D12Triangle-Hello.exe",
    [string]$AppArgs = "--loop-frame 3"
)

# UTF-8 BOM付きで出力
$OutputEncoding = [System.Text.Encoding]::UTF8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$ProjectRoot = $PSScriptRoot
$ProjectRoot = Join-Path $PSScriptRoot "..\..\..\.." | Resolve-Path -ErrorAction SilentlyContinue
if (-not $ProjectRoot) {
    $ProjectRoot = (Get-Item $PSScriptRoot).Parent.Parent.Parent.Parent.FullName
}

$FullExePath = Join-Path $ProjectRoot $ExePath
$SummaryLog = Join-Path $ProjectRoot "bin/run-summary.log"
$TraceLog = Join-Path $ProjectRoot "bin/trace.log"

# 実行体 existence check
if (-not (Test-Path $FullExePath)) {
    $result = [PSCustomObject]@{
        ExePath = $FullExePath
        AppArgs = $AppArgs
        NaturalExit = $false
        ExitCode = -1
        Killed = $false
        Result = "FAIL"
    }
    $result | Out-File -FilePath $SummaryLog -Encoding utf8
    Write-Error "Exe not found: $FullExePath"
    exit 1
}

# 既存の trace.log を削除
if (Test-Path $TraceLog) {
    Remove-Item $TraceLog -Force
}

# プロセス開始
$startInfo = New-Object System.Diagnostics.ProcessStartInfo
$startInfo.FileName = $FullExePath
$startInfo.Arguments = $AppArgs
$startInfo.UseShellExecute = $false
$startInfo.RedirectStandardOutput = $true
$startInfo.RedirectStandardError = $true
$startInfo.WorkingDirectory = (Get-Item $PSScriptRoot\..\..\..\..).FullName

$process = New-Object System.Diagnostics.Process
$process.StartInfo = $startInfo
$process.Start() | Out-Null

Write-Host "Started: $FullExePath Args: $AppArgs"

# タイムアウト待ち
$naturalExit = $false
$killed = $false
$exitCode = -1

$hasTimedOut = $process.WaitForExit($TimeoutSeconds * 1000)

if ($hasTimedOut) {
    $naturalExit = $true
    $exitCode = $process.ExitCode
    Write-Host "Exited naturally (code=$exitCode)"
} else {
    $killed = $true
    $process.Kill()
    $process.WaitForExit()
    Write-Host "Killed due to timeout"
}

# trace.log 確認
$traceContent = ""
$traceExists = Test-Path $TraceLog
if ($traceExists) {
    $traceContent = Get-Content $TraceLog -Raw -ErrorAction SilentlyContinue
}

# 検証
$isPass = $true
$reasons = @()

if (-not $naturalExit) {
    $isPass = $false
    $reasons += "Timed out"
}

if ($killed) {
    $isPass = $false
    $reasons += "Process killed"
}

if ($exitCode -ne 0) {
    $isPass = $false
    $reasons += "Exit code: $exitCode"
}

if (-not $traceExists) {
    $isPass = $false
    $reasons += "trace.log not found"
} elseif ([string]::IsNullOrWhiteSpace($traceContent)) {
    $isPass = $false
    $reasons += "trace.log is empty"
} else {
    if ($traceContent -notmatch "=== APP START ===") {
        $isPass = $false
        $reasons += "Missing '=== APP START ==='"
    }
    if ($traceContent -notmatch "=== APP END ===") {
        $isPass = $false
        $reasons += "Missing '=== APP END ==='"
    }
    if ($traceContent -match "FAILED") {
        $isPass = $false
        $reasons += "FAILED found in trace.log"
    }
}

$result = if ($isPass) { "PASS" } else { "FAIL" }

# サマリー出力
$schema = [PSCustomObject]@{
    ExePath = $FullExePath
    AppArgs = $AppArgs
    NaturalExit = $naturalExit
    ExitCode = $exitCode
    Killed = $killed
    Result = $result
}

if ($reasons.Count -gt 0) {
    $schema.PSObject.Properties.Add([System.Management.Automation.PSNoteProperty]::new("Reasons", ($reasons -join ", ")))
}

$schema | Out-File -FilePath $SummaryLog -Encoding utf8

Write-Host "=== Run Summary ==="
$schema | Format-List | Out-String | Write-Host
Write-Host "Summary saved to: $SummaryLog"

if (-not $isPass) {
    exit 1
}
exit 0
