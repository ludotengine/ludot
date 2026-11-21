# Run the PowerShell bootstrapper against local release fixtures, without installing an app.
$ErrorActionPreference = 'Stop'
$FixtureRoot = Join-Path ([IO.Path]::GetTempPath()) ([Guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $FixtureRoot | Out-Null
$script:FixtureFile = Join-Path $FixtureRoot 'installer.exe'
[IO.File]::WriteAllText($script:FixtureFile, 'Ludot installer fixture')
$script:FixtureHash = (Get-FileHash $script:FixtureFile -Algorithm SHA256).Hash
$script:FixtureLaunches = 0

function Invoke-RestMethod {
	param([string]$Uri)
	return @{ tag_name = 'v9.8.7-lu.1' }
}
function Invoke-WebRequest {
	param([switch]$UseBasicParsing, [string]$Uri, [string]$OutFile)
	if ($OutFile) {
		Copy-Item $script:FixtureFile $OutFile
		return
	}
	$Text = "$script:FixtureHash  ludot-9.8.7-lu.1-windows-x86_64-setup.exe`r`n"
	# GitHub release assets can have an application/octet-stream content type.
	return @{ Content = [Text.Encoding]::UTF8.GetBytes($Text) }
}
function Start-Process {
	param([string]$FilePath, [string[]]$ArgumentList, [switch]$Wait, [switch]$PassThru)
	if (-not (Test-Path $FilePath)) { throw 'Missing downloaded installer.' }
	if ($ArgumentList -notcontains '/VERYSILENT' -or $ArgumentList -notcontains '/TASKS=modifypath') {
		throw 'Missing installer flags.'
	}
	$script:FixtureLaunches += 1
	return @{ ExitCode = 0 }
}
try {
	. (Join-Path $PSScriptRoot '../install.ps1')
	if ($script:FixtureLaunches -ne 1) { throw 'The verified installer did not launch.' }
	$script:FixtureHash = '0' * 64
	$Rejected = $false
	try { . (Join-Path $PSScriptRoot '../install.ps1') } catch {
		if ($_.Exception.Message -notmatch 'checksum did not match') { throw }
		$Rejected = $true
	}
	if (-not $Rejected -or $script:FixtureLaunches -ne 1) { throw 'Invalid checksum was not rejected before launch.' }
	Write-Host 'LUDOT_POWERSHELL_INSTALLER_TESTS_PASSED'
} finally {
	Remove-Item -Recurse -Force $FixtureRoot
}
