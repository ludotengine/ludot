# Install or update the latest published Ludot release for the current user.
$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
if (-not [Environment]::Is64BitOperatingSystem) {
	throw 'Ludot requires 64-bit Windows.'
}
$Repository = 'https://github.com/ludotengine/ludot'
$Release = Invoke-RestMethod 'https://api.github.com/repos/ludotengine/ludot/releases/latest'
$Tag = $Release.tag_name
if ($Tag -notmatch '^v[0-9][A-Za-z0-9._-]*$') { throw 'Invalid release tag.' }
$Version = $Tag.Substring(1)
$Asset = "ludot-$Version-windows-x86_64"
$Installer = "$Asset-setup.exe"
$Work = Join-Path ([IO.Path]::GetTempPath()) ([Guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $Work | Out-Null
try {
	$File = Join-Path $Work $Installer
	Invoke-WebRequest -UseBasicParsing "$Repository/releases/download/$Tag/$Installer" -OutFile $File
	$Checksums = (Invoke-WebRequest -UseBasicParsing "$Repository/releases/download/$Tag/$Asset.sha256").Content
	if ($Checksums -is [byte[]]) { $Checksums = [Text.Encoding]::UTF8.GetString($Checksums) }
	$Match = [regex]::Match($Checksums, "(?m)^([a-fA-F0-9]{64})  $([regex]::Escape($Installer))\r?$")
	if (-not $Match.Success) { throw 'The release checksum is missing.' }
	if ((Get-FileHash $File -Algorithm SHA256).Hash -ne $Match.Groups[1].Value) {
		throw 'The installer checksum did not match. Installation stopped.'
	}
	$Process = Start-Process -FilePath $File -ArgumentList '/VERYSILENT', '/SUPPRESSMSGBOXES', '/NORESTART', '/TASKS=modifypath' -Wait -PassThru
	if ($Process.ExitCode -ne 0) { throw "Installer exited with code $($Process.ExitCode)." }
	Write-Host "Installed Ludot $Version. Open a new terminal to use ludot."
} finally {
	Remove-Item -Recurse -Force $Work
}
