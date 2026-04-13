param(
    [string]$AsmFile = "output.s",
    [string]$CreatorUrl = "https://creatorsim.github.io/creator/",
    [switch]$AutoPaste
)

if (-not (Test-Path -LiteralPath $AsmFile)) {
    Write-Error "Assembly file not found: $AsmFile"
    exit 1
}

$asmText = Get-Content -LiteralPath $AsmFile -Raw
if ([string]::IsNullOrWhiteSpace($asmText)) {
    Write-Error "Assembly file is empty: $AsmFile"
    exit 1
}

Set-Clipboard -Value $asmText
Start-Process $CreatorUrl
Write-Host "Opened Creator: $CreatorUrl"
Write-Host "Copied '$AsmFile' to clipboard. Click the editor and press Ctrl+V."

if ($AutoPaste) {
    Start-Sleep -Seconds 3
    $wshell = New-Object -ComObject WScript.Shell
    $null = $wshell.AppActivate("Creator")
    Start-Sleep -Milliseconds 500
    $wshell.SendKeys("^v")
    Write-Host "Attempted auto-paste (Ctrl+V). If it missed focus, paste manually."
}
