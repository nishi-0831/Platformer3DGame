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
    # falseでBOM無しを指定
    $utf8NoBom = New-Object System.Text.UTF8Encoding $false
    Write-Host "Success clang-tidy. output to JSON"
    # 書き込むファイル、ファイルに書き込む文字列、エンコード
    [System.IO.File]::WriteAllText($OutputJson,$output,$utf8NoBom)
    Write-Host "Success output to JSON"
    return 0
}

function ConvertToSarif
{
    # JSONファイルの内容をConverterに渡して、SARIFに変換
    try 
    {
        $output = Get-Content -Raw $OutputJson | & $SarifConverter
        $exitCode = $LASTEXITCODE
        if($exitCode -ne 0)
        {
            Write-Error("ErrorCode: $exitCode $output")
            return $exitCode;   
        }
        # falseでBOM無しを指定
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false
        # 書き込むファイル、ファイルに書き込む文字列、エンコード
        [System.IO.File]::WriteAllText($OutputSarif,$output,$utf8NoBom)
    }
    catch 
    {
        Write-Host "Error: $($_.Exception.Message)"
        return 1;
    }
    return 0
}

RunClangTidy
ConvertToSarif

exit