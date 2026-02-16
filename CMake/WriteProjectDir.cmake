# 変数
# - OUT : 出力先ファイルパス
# - TEXT : 書き込む内容
if(NOT DEFINED OUT)
	message(FATAL_ERROR "WriteProjectDir.cmake: OUT not defined")
endif()
if(NOT DEFINED TEXT)
	set(TEXT "")
endif()
file(WRITE "${OUT}" "${TEXT}\n")