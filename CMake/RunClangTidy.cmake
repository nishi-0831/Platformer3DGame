cmake_minimum_required(VERSION 3.17.0)

set(EXE_PATH "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/Tools/RunClangTidyWrapper/RunClangTidyWrapper.exe>")
set(SARIF_CONVERTER "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/clang-tidy-sarif.exe>")
set(OUTPUT_JSON "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/tidy-output.json>")
set(OUTPUT_SARIF "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/tidy-output.sarif>")
set(SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/RunClangTidy.ps1")

# 呼び出し側で登録する
function(register_clang_tidy_target FUNC_NAME)
    add_custom_target(${FUNC_NAME}
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File "${SCRIPT_FILE}"
        -ClangTidyWrapper ${EXE_PATH} -SarifConverter ${SARIF_CONVERTER}
        -OutputJson ${OUTPUT_JSON} -OutputSarif ${OUTPUT_SARIF}
        -SrcDir "$<SHELL_PATH:${CMAKE_SOURCE_DIR}>" -BinDir "$<SHELL_PATH:${CMAKE_BINARY_DIR}>"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Run clang-tidy and convert to SARIF"
        USES_TERMINAL
    )
endfunction()

register_clang_tidy_target(clang-tidy)