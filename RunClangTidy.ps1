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
    & $ClangTidyWrapper $SrcDir -p $BinDir -- | Out-File -FilePath $OutputJson -Encoding utf8
    return 0
}

function ConvertToSarif
{
    # JSONファイルの内容をConverterに渡して、SARIFに変換
    try 
    {
        Get-Content $OutputJson | & $SarifConverter | Out-File -FilePath $OutputSarif -Encoding utf8
        
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