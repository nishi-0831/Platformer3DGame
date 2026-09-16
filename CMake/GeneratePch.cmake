cmake_minimum_required(VERSION 3.17.0)

function(register_generate_pch_target)
	if("${CMAKE_GENERATOR}" STREQUAL "Ninja Multi-Config")
		# サポートされているビルド・モードを全て取得
		set(SUPPORTED_CONFIGS ${CMAKE_CONFIGURATION_TYPES})

		# 全ビルド・モードのPCHを生成
		add_custom_target(generate-pch
			COMMENT "Generate PCH for all configurations"
		)
		# 各ビルド・モード毎にPCHを生成するターゲットを登録
		foreach(CONFIG ${SUPPORTED_CONFIGS})
			# ターゲット名
			set(PCH_TARGET "generate-pch-${CONFIG}")
			add_custom_target(${PCH_TARGET}
				COMMAND echo "Generating PCH for ${CONFIG} configuration"
				COMMAND ninja -f "build-${CONFIG}.ninja" "CMakeFiles/${PROJECT_NAME}.dir/${CONFIG}/cmake_pch.cxx.pch"
				WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
				USES_TERMINAL
			)
			add_dependencies(generate-pch "generate-pch-${CONFIG}")
		endforeach()

	elseif("${CMAKE_GENERATOR}" STREQUAL "Ninja")
		add_custom_target(generate-pch
			COMMAND echo "Generating PCH"
			COMMAND ninja "CMakeFiles/${PROJECT_NAME}.dir/cmake_pch.cxx.pch"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
			USES_TERMINAL
		)
	else()
        message(WARNING "PCH generation not implemented for generator: ${CMAKE_GENERATOR}")
	endif()
endfunction()
register_generate_pch_target()