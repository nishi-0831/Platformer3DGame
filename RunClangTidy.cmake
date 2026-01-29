cmake_minimum_required(VERSION 3.16.0)

set(EXE_PATH "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/Tools/RunClangTidyWrapper/RunClangTidyWrapper.exe>")
set(SARIF_CONVERTER "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/clang-tidy-sarif.exe>")
set(OUTPUT_JSON "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/tidy-output.json>")
set(OUTPUT_SARIF "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/tidy-output.sarif>")

# 呼び出し側で登録する
function(register_clang_tidy_target FUNC_NAME)
    # PowerShell 経由で実行してパイプで変換し、UTF-8で出力する
    add_custom_target(${FUNC_NAME}
    # powershellでPowerShellが使えるようになる
    # -NoProfileでプロファイルを読み込まない
    # PowerShellコマンドを実行するには-Command "..."
      COMMAND powershell -NoProfile -Command
        " ${EXE_PATH} ${CMAKE_CURRENT_SOURCE_DIR} -p ${CMAKE_CURRENT_BINARY_DIR} -- |
          ${SARIF_CONVERTER} |
          Out-File -FilePath ${OUTPUT_SARIF} -Encoding UTF8"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      COMMENT "Run clang-tidy and convert to SARIF"
      USES_TERMINAL
    )
endfunction()

register_clang_tidy_target(clang-tidy)