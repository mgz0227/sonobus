param(
    [Parameter(Mandatory = $true)]
    [string] $ServerBinary,

    [Parameter(Mandatory = $true)]
    [string] $ClientBinary,

    [int] $CurrentPort = 19098,
    [int] $LegacyPort = 19096
)

$ErrorActionPreference = 'Stop'

$serverPath = (Resolve-Path -LiteralPath $ServerBinary).Path
$clientPath = (Resolve-Path -LiteralPath $ClientBinary).Path
$stamp = [DateTimeOffset]::UtcNow.ToUnixTimeMilliseconds()
$group = "ci-current-$stamp"
$user = "ci-current-$stamp"
$logDir = Join-Path ([IO.Path]::GetTempPath()) "sonobus-current-smoke-$PID-$stamp"
New-Item -ItemType Directory -Path $logDir | Out-Null

$serverLog = Join-Path $logDir 'aooserver.log'
$serverError = Join-Path $logDir 'aooserver-error.log'
$clientLog = Join-Path $logDir 'sonobus.log'
$clientError = Join-Path $logDir 'sonobus-error.log'

$server = Start-Process -FilePath $serverPath `
    -ArgumentList @('-p', "$CurrentPort", '--legacy-port', "$LegacyPort", '-l', '3') `
    -RedirectStandardOutput $serverLog -RedirectStandardError $serverError `
    -WindowStyle Hidden -PassThru

try {
    $ready = $false
    for ($attempt = 0; $attempt -lt 40; ++$attempt) {
        $server.Refresh()
        if ($server.HasExited) {
            throw "aooserver exited before binding ports`n$(Get-Content $serverLog -Raw)`n$(Get-Content $serverError -Raw)"
        }
        $currentReady = [bool](Get-NetUDPEndpoint -LocalPort $CurrentPort -ErrorAction SilentlyContinue)
        $legacyReady = [bool](Get-NetUDPEndpoint -LocalPort $LegacyPort -ErrorAction SilentlyContinue)
        if ($currentReady -and $legacyReady) {
            $ready = $true
            break
        }
        Start-Sleep -Milliseconds 250
    }
    if (-not $ready) {
        throw "aooserver did not bind both ports`n$(Get-Content $serverLog -Raw)`n$(Get-Content $serverError -Raw)"
    }

    $arguments = "-q -c 127.0.0.1:$CurrentPort -g $group -n $user"
    $client = Start-Process -FilePath $clientPath -ArgumentList $arguments `
        -RedirectStandardOutput $clientLog -RedirectStandardError $clientError `
        -WindowStyle Hidden -PassThru

    try {
        $joined = $false
        $expected = "Add new group '$group'"
        for ($attempt = 0; $attempt -lt 80; ++$attempt) {
            if ((Test-Path -LiteralPath $serverLog) -and
                (Get-Content -LiteralPath $serverLog -Raw) -match [regex]::Escape($expected)) {
                $joined = $true
                break
            }
            $client.Refresh()
            if ($client.HasExited) {
                break
            }
            Start-Sleep -Milliseconds 250
        }

        $clientErrors = if (Test-Path -LiteralPath $clientError) {
            Get-Content -LiteralPath $clientError -Raw
        } else {
            ''
        }
        if ($joined) {
            Start-Sleep -Seconds 1
        }
        $serverOutput = Get-Content -LiteralPath $serverLog -Raw
        $removed = $serverOutput -match [regex]::Escape("Remove group '$group'")
        if (-not $joined -or $removed -or
            $clientErrors -match '/aoo/client/request.*missing data') {
            throw "Current SonoBus client did not join $group`nSERVER:`n$(Get-Content $serverLog -Raw)`nSERVER ERROR:`n$(Get-Content $serverError -Raw)`nCLIENT:`n$(Get-Content $clientLog -Raw)`nCLIENT ERROR:`n$clientErrors"
        }

        Write-Host "Current SonoBus client joined $group on port $CurrentPort"
    }
    finally {
        $client.Refresh()
        if (-not $client.HasExited) {
            Stop-Process -Id $client.Id -Force
            Wait-Process -Id $client.Id -ErrorAction SilentlyContinue
        }
    }
}
finally {
    $server.Refresh()
    if (-not $server.HasExited) {
        Stop-Process -Id $server.Id -Force
        Wait-Process -Id $server.Id -ErrorAction SilentlyContinue
    }
}
