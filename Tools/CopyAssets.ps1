Param(
    [string]$AssetsDirectory,
    [string]$OutputDirectory
)

function Get-AssetsDirectory {
    param (
        [Parameter(Mandatory)]
        [string]$Directory
    )
    # DirectoryがAssetsフォルダを指すかどうか
    if ((Test-Path $Directory -PathType Container) -And (Split-Path -Leaf $Directory) -ieq "Assets")
    {
        $testPath = $Directory
    }
    # Directoryの子にAssetsフォルダがあるかどうか
    elseif (Test-Path (Join-Path $Directory "Assets") -PathType Container)
    {
        $testPath = Join-Path $Directory "Assets";
    }
    else 
    {
        throw "Assets folder not found in $Directory"
    }

    return $testPath    
}


try {
    $SrcPath = Get-AssetsDirectory($AssetsDirectory)    
}
catch {
    <#Do this if a terminating exception happens#>
    Write-Error ("exception: " + $_.Exception.Message)
    exit 1
}

if(Test-Path $OutputDirectory -PathType Container) {
    Write-Output "Synchronizing assets..."
    $outputAssetsDir = Join-Path $OutputDirectory "Assets"
    # /MIR: コピー先と元を一致 
    # /XO: コピー元がコピー先より古い場合、そのファイルをコピー対象から除外
    # /FFT: タイムスタンプの2秒の誤差は許容する?
    # /R: コピー失敗したときのリトライ数。デフォルトはなんと100万回
    # /W: コピー再試行までの待ち時間。デフォルトだと30秒
    & robocopy.exe "$SrcPath" "$outputAssetsDir" /MIR /XO /FFT /R:2 /W:1
    $lastCode = $LASTEXITCODE
    if ($last -ge 8){
        Write-Error "robocopy failed with exit code $lastCode"
        exit 1
    }
}
else {
    Write-Error("Output Directory not found in $OutputDirectory")
    exit 1
}


