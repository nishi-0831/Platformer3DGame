Param(
    [string]$ClangTidyWrapper,
    [string]$SarifConverter,
    [string]$OutputJson,
    [string]$OutputSarif,
    [string]$SrcDir,
    [string]$BinDir
    )

function RunClangTidy
{
    # clang-tidyを起動し、出力をJSONファイルにリダイレクト
    
    # clang-tidyの出力を保存
    Write-Host "Running clang-tidy."
    Write-Host "Command: & $ClangTidyWrapper $SrcDir -p $BinDir"
    $output = & $ClangTidyWrapper $SrcDir -p $BinDir

    Write-Host "Success clang-tidy. output to JSON"

    # falseでBOM無しを指定
    $utf8NoBom = New-Object System.Text.UTF8Encoding $false
    # 書き込むファイル、ファイルに書き込む文字列、エンコード
    $streamWriter = New-Object System.IO.StreamWriter($OutputJson,$false,$utf8NoBom)
    try
    {
        foreach($line in $output)
        {
            $streamWriter.WriteLine($line)
        }
    }
    finally
    {
        $streamWriter.Close()
        $streamWriter.Dispose()
    }

    Write-Host "Success output to JSON"
    return 0
}

function ConvertToSarif
{
    # JSONファイルの内容をConverterに渡して、SARIFに変換
    try 
    {
        Write-Host "convert to sarif"
        $output = Get-Content -Raw $OutputJson | & $SarifConverter
        $exitCode = $LASTEXITCODE
        if($exitCode -ne 0)
        {
            Write-Error("ErrorCode: $exitCode $output")
            return $exitCode;   
        }
        Write-Host "write to sarif"

        # falseでBOM無しを指定
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false

        # 書き込むファイル、ファイルに書き込む文字列、エンコード
        $streamWriter = New-Object System.IO.StreamWriter($OutputSarif, $false, $utf8NoBom)
        try
        {
            foreach($line in $output)
            {
                $streamWriter.WriteLine($line)
            }
        }
        finally
        {
            $streamWriter.Close()
            $streamWriter.Dispose()
        }
    }
    catch 
    {
        Write-Host "Error: $($_.Exception.Message)"
        return 1;
    }
    return 0
}

function DeduplicateSarif
{
    Write-Host "Deduplicate sarif"
    $deduplicatorPath = Join-Path -Path $SrcDir -ChildPath "Tools\SarifDeduplicator\SarifDeduplicator.exe"
    & $deduplicatorPath $SrcDir
}


RunClangTidy
ConvertToSarif
DeduplicateSarif

exit