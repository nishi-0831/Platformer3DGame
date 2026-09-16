cmake_minimum_required(VERSION 3.19.0)

# 使用するOSのバージョン
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
# プロジェクト名 使用言語
project(Game LANGUAGES C CXX)
set(PROJECT_NAME Project1)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/Release)



################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

# compile_commands.jsonの生成を有効にする
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    # ビルドモード共通のコンパイルオプション
    set(CMAKE_CXX_FLAGS "")
    # デバッグビルド時に追加されるコンパイルオプション
    set(CMAKE_CXX_FLAGS_DEBUG "")
    # リリースビルド時に追加されるコンパイルオプション
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()


################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Use solution folders feature
# ファイルのフィルタリング機能を有効にする
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################

################################################################################
# Source groups
# source_group("任意のフィルタ名" FILES <ソースコードのパス>)
################################################################################
set(no_group_source_files
    ".clang-format"
    ".pre-commit-config.yaml"
)

file(GLOB_RECURSE MTGB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/mtgb/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mtgb/*.h"
)
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")

set(02Assets__Shader
    "Assets/Shader/2DCommon.hlsli"
    "Assets/Shader/3DCommon.hlsli"
)
source_group("02Assets\\Shader" FILES ${02Assets__Shader})

set(04Library__ImGui
    "ThirdParty/ImGui/imconfig.h"
    "ThirdParty/ImGui/imgui.cpp"
    "ThirdParty/ImGui/imgui.h"
    "ThirdParty/ImGui/imgui_demo.cpp"
    "ThirdParty/ImGui/imgui_draw.cpp"
    "ThirdParty/ImGui/imgui_impl_dx11.cpp"
    "ThirdParty/ImGui/imgui_impl_dx11.h"
    "ThirdParty/ImGui/imgui_impl_win32.cpp"
    "ThirdParty/ImGui/imgui_impl_win32.h"
    "ThirdParty/ImGui/imgui_internal.h"
    "ThirdParty/ImGui/imgui_ja_gryph_ranges.cpp"
    "ThirdParty/ImGui/imgui_tables.cpp"
    "ThirdParty/ImGui/imgui_widgets.cpp"
    "ThirdParty/ImGui/imstb_rectpack.h"
    "ThirdParty/ImGui/imstb_textedit.h"
    "ThirdParty/ImGui/imstb_truetype.h"
)
source_group("04Library\\ImGui" FILES ${04Library__ImGui})

set(04Library__ImGui__ImGuizmo
    "ThirdParty/ImGui/GraphEditor.cpp"
    "ThirdParty/ImGui/GraphEditor.h"
    "ThirdParty/ImGui/ImCurveEdit.cpp"
    "ThirdParty/ImGui/ImCurveEdit.h"
    "ThirdParty/ImGui/ImGradient.cpp"
    "ThirdParty/ImGui/ImGradient.h"
    "ThirdParty/ImGui/ImGuizmo.cpp"
    "ThirdParty/ImGui/ImGuizmo.h"
    "ThirdParty/ImGui/ImSequencer.cpp"
    "ThirdParty/ImGui/ImSequencer.h"
    "ThirdParty/ImGui/ImZoomSlider.h"
)
source_group("04Library\\ImGui\\ImGuizmo" FILES ${04Library__ImGui__ImGuizmo})

set(Workflow
    ".github/workflows/DeployDoxygen.yml"
    ".github/workflows/ReleaseBuildAndPublish.yml"
)
source_group("Workflow" FILES ${Workflow})

set(ALL_FILES
    ${02Assets__Shader}
    ${04Library__ImGui}
    ${04Library__ImGui__ImGuizmo}
    ${Workflow}
    ${Workflow__Utility}
    ${Unfiltered}
    ${MTGB_SOURCES}
    ${CLIENT_SOURCES}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/mtgb PREFIX "mtgb" FILES ${MTGB_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX "Source" FILES ${CLIENT_SOURCES})
# プリコンパイル済みヘッダを設定する
target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/mtgb/stdafx.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE AirPlaneGame)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        # リリースビルド時のインクルードディレクトリ
        "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Program Files/Autodesk/FBX/FBX SDK/2020.3.7/include;$ENV{REFL_CPP}>"
        # どのビルド・モードでも共通
        "$ENV{FBX_SDK}/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty;"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/EffekseeLib/install_msvc2022_x64/include/Effekseer;"
        "${CMAKE_CURRENT_SOURCE_DIR}/mtgb"
        "${CMAKE_CURRENT_SOURCE_DIR}/Generated"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        # デバッグビルド
        "$<$<CONFIG:Debug>:_DEBUG>"
        # リリースビルド
        "$<$<CONFIG:Release>:NDEBUG>"
        "_WINDOWS;"
        "_AMD64_;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "WIN32;"
        "_WINDOWS;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

set(MSVC_COMMON_OPTIONS
    # デバッグビルド時のオプション
    $<$<CONFIG:Debug>:/wd4005>
    # リリースビルド時のオプション
    $<$<CONFIG:Release>:/Oi;/Gy>
    # C++の標準規格に準拠したコンパイルを強制する
    /permissive-;
    # __cplusplusの値でC++のバージョンを判定している箇所が正しく機能するように
    /Zc:__cplusplus;
    /sdl;
    /W4;
    # 無名共用体/構造体を定義した際の警告を抑制
    /wd4201;
    # 認識されない属性を付与した際の警告を抑制
    /wd5030;
    # 警告をエラーとして扱う
    # /WX;
    /FI ${CMAKE_CURRENT_SOURCE_DIR}/mtgb/stdafx.h
    ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
    /utf-8;
    ${DEFAULT_CXX_EXCEPTION_HANDLING}
    /Od
)

set(CLANGCL_ADDITIONAL_OPTIONS
    $<$<CONFIG:Debug>:/Zi;-g;-O0;-fno-omit-frame-pointer>
    -fms-extensions 
    -fms-compatibility 
    -fms-compatibility-version=19.0
    -D_MSC_VER=1928 
    -target x86_64-pc-windows-msvc 
    -fdelayed-template-parsing 
    -fno-complete-member-pointers 
    -Wno-c++98-compat 
    -Wno-c++98-compat-pedantic 
    -Wno-reserved-id-macro
    -Wno-pragma-pack 
    -Wno-unknown-pragmas
    -Wno-unused-command-line-argument
)
################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER MATCHES ".*clang-cl.*")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    else()
        target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            # 指定したC++標準バージョンを必須とする。コンパイラがサポートしていないならばビルド失敗
            CXX_STANDARD_REQUIRED ON
            # コンパイラ固有の拡張機能を無効にする
            CXX_EXTENSIONS OFF
    )
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE ${MSVC_COMMON_OPTIONS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER MATCHES ".*clang-cl.*")
        target_compile_options(${PROJECT_NAME} PRIVATE ${MSVC_COMMON_OPTIONS} ${CLANGCL_ADDITIONAL_OPTIONS})
    endif()

    # "/Y-" はプリコンパイル済みヘッダを無視するオプション
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    # プリコンパイル済みヘッダを無視するソース
    source_file_compile_options(ImGui/GraphEditor.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImCurveEdit.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImGradient.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImGuizmo.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_ja_gryph_ranges.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImSequencer.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_demo.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_draw.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_impl_dx11.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_impl_win32.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_tables.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_widgets.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:NOREF;
                /OPT:NOICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

option(ENABLE_AUTO_CODE_GEN "Run code generator tool" ON)

# コード生成ツールを実行するコマンド
set(GENERATE_SOURCE_CODE_COMMAND "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/Tools/ClangSourceGenerator/bin/ClangSourceGenerator.exe> " --config "Tools/ClangSourceGenerator" --projDir "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(generate_source_code
    COMMAND ${GENERATE_SOURCE_CODE_COMMAND} || exit 0
    USES_TERMINAL)

# コード生成の自動実行が有効になっている場合
if(ENABLE_AUTO_CODE_GEN)
    # コンパイル前に実行されるように依存させる
     add_dependencies(${PROJECT_NAME} generate_source_code)
endif()

 # キャッシュを削除し、再生成を行う
 add_custom_target(force_generate_source_code
    COMMAND ${GENERATE_SOURCE_CODE_COMMAND} --force
    USES_TERMINAL
 )

 if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
     set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
     set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
endif()

 # アセットを実行ファイルのフォルダにコピー
 add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
            "${OUTPUT_DIRECTORY}/Assets"
    COMMENT "Copying assets to output directory..."
)

 include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/RunClangTidy.cmake")
 include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/GeneratePch.cmake")

 # 自動生成されたソースファイルが配置されるGeneratedフォルダのcppを列挙して、ターゲットに追加する
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Generated")
    file(GLOB_RECURSE GENERATED_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Generated/*.gen.cpp")
    target_sources(${PROJECT_NAME}
        PRIVATE ${GENERATED_SOURCE})
    source_group("Generated" FILES ${GENERATED_SOURCE})
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Generated"
    )
 endif()

################################################################################
# Dependencies (リンク先)
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        # デバッグビルド
        "$<$<CONFIG:Debug>:"
            "$ENV{FBX_SDK}/lib/x64/debug;"
        ">"
        # リリースビルド
        "$<$<CONFIG:Release>:"
            "$ENV{FBX_SDK}/lib/x64/release;"
        ">"
        # どのビルドモードでも共通
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/EffekseeLib/install_msvc2022_x64/lib"
    )
endif()