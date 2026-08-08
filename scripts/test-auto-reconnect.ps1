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

$serverLog = Join-Path $workDir 'aooserver.log'
$serverError = Join-Path $workDir 'aooserver-error.log'
$oldAppData = $env:APPDATA
$oldLocalAppData = $env:LOCALAPPDATA
$env:APPDATA = $profileDir
$env:LOCALAPPDATA = $profileDir

$server = $null

try {
    $server = Start-Process -FilePath $serverPath `
        -ArgumentList @('-p', "$CurrentPort", '--legacy-port', "$LegacyPort", '-l', '3') `
        -RedirectStandardOutput $serverLog -RedirectStandardError $serverError `
        -WindowStyle Hidden -PassThru

    $ready = $false
    for ($attempt = 0; $attempt -lt 40; ++$attempt) {
        $server.Refresh()
        if ($server.HasExited) {
            throw "aooserver exited before binding ports`n$(Get-Content $serverLog -Raw)`n$(Get-Content $serverError -Raw)"
        }
        if ((Get-NetUDPEndpoint -LocalPort $CurrentPort -ErrorAction SilentlyContinue) -and
            (Get-NetUDPEndpoint -LocalPort $LegacyPort -ErrorAction SilentlyContinue)) {
            $ready = $true
            break
        }
        Start-Sleep -Milliseconds 250
    }

    if (-not $ready) {
        throw "aooserver did not bind both ports`n$(Get-Content $serverLog -Raw)`n$(Get-Content $serverError -Raw)"
    }

    & $testPath '127.0.0.1' "$CurrentPort"
    if ($LASTEXITCODE -ne 0) {
        throw "No-editor auto reconnect test failed with exit code $LASTEXITCODE`nSERVER:`n$(Get-Content $serverLog -Raw)`nSERVER ERROR:`n$(Get-Content $serverError -Raw)"
    }
}
finally {
    $env:APPDATA = $oldAppData
    $env:LOCALAPPDATA = $oldLocalAppData
    if ($server) {
        $server.Refresh()
    }
    if ($server -and -not $server.HasExited) {
        Stop-Process -Id $server.Id -Force
        Wait-Process -Id $server.Id -ErrorAction SilentlyContinue
    }
}
