param(
    [Parameter(Mandatory = $true)]
    [string] $ServerBinary,

    [Parameter(Mandatory = $true)]
    [string] $TestBinary,

    [int] $CurrentPort = 19098,
    [int] $LegacyPort = 19096
)

$ErrorActionPreference = 'Stop'

$serverPath = (Resolve-Path -LiteralPath $ServerBinary).Path
$testPath = (Resolve-Path -LiteralPath $TestBinary).Path
$stamp = [DateTimeOffset]::UtcNow.ToUnixTimeMilliseconds()
$workDir = Join-Path ([IO.Path]::GetTempPath()) "sonobus-auto-reconnect-$PID-$stamp"
$profileDir = Join-Path $workDir 'profile'
New-Item -ItemType Directory -Path $profileDir | Out-Null

$oldAppData = $env:APPDATA
$oldLocalAppData = $env:LOCALAPPDATA
$env:APPDATA = $profileDir
$env:LOCALAPPDATA = $profileDir

try {
    foreach ($legacyOnly in @($false, $true)) {
        $phaseOffset = if ($legacyOnly) { 10 } else { 0 }
        $phaseCurrentPort = $CurrentPort + $phaseOffset
        $phaseLegacyPort = $LegacyPort + $phaseOffset
        $phaseTestPort = if ($legacyOnly) { $phaseLegacyPort } else { $phaseCurrentPort }
        $phaseName = if ($legacyOnly) { 'legacy' } else { 'current' }
        $serverLog = Join-Path $workDir "aooserver-$phaseName.log"
        $serverError = Join-Path $workDir "aooserver-$phaseName-error.log"
        $serverArgs = @('-p', "$phaseCurrentPort", '--legacy-port', "$phaseLegacyPort", '-l', '3')
        if ($legacyOnly) {
            $serverArgs += '--legacy-only'
        }

        $server = $null
        try {
            $server = Start-Process -FilePath $serverPath `
                -ArgumentList $serverArgs `
                -RedirectStandardOutput $serverLog -RedirectStandardError $serverError `
                -WindowStyle Hidden -PassThru

            $ready = $false
            for ($attempt = 0; $attempt -lt 40; ++$attempt) {
                $server.Refresh()
                if ($server.HasExited) {
                    throw "aooserver ($phaseName) exited before binding ports`n$(Get-Content $serverLog -Raw)`n$(Get-Content $serverError -Raw)"
                }
                if ((Get-NetUDPEndpoint -LocalPort $phaseCurrentPort -ErrorAction SilentlyContinue) -and
                    (Get-NetUDPEndpoint -LocalPort $phaseLegacyPort -ErrorAction SilentlyContinue)) {
                    $ready = $true
                    break
                }
                Start-Sleep -Milliseconds 250
            }

            if (-not $ready) {
                throw "aooserver ($phaseName) did not bind both ports`n$(Get-Content $serverLog -Raw)`n$(Get-Content $serverError -Raw)"
            }

            & $testPath '127.0.0.1' "$phaseTestPort"
            if ($LASTEXITCODE -ne 0) {
                throw "No-editor $phaseName auto reconnect test failed with exit code $LASTEXITCODE`nSERVER:`n$(Get-Content $serverLog -Raw)`nSERVER ERROR:`n$(Get-Content $serverError -Raw)"
            }
        }
        finally {
            if ($server) {
                $server.Refresh()
            }
            if ($server -and -not $server.HasExited) {
                Stop-Process -Id $server.Id -Force
                Wait-Process -Id $server.Id -ErrorAction SilentlyContinue
            }
        }
    }
}
finally {
    $env:APPDATA = $oldAppData
    $env:LOCALAPPDATA = $oldLocalAppData
}
