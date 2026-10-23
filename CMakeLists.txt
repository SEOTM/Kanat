cmake_minimum_required(VERSION 3.20)

# Project identity: change these two values and src/Identity.h to derive a new application.
set(APP_PROJECT_NAME "Kanat")
set(APP_DOC_NAME "Kanat.json")

project(${APP_PROJECT_NAME} LANGUAGES CXX)

if(MSVC)
	# Sources are UTF-8 without BOM; /Zc:char8_t- keeps u8"..." as char* when built as C++20.
	add_compile_options(/utf-8 /Zc:char8_t-)
endif()

# Static CRT (/MT, /MTd), self-contained binaries.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Native dependencies are vendored as an offline NuGet archive: Dear ImGui and RapidJSON.
# The archive is extracted at configure time, so no package feed or network access is required.
set(KANAT_DEPS_ARCHIVE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/Kanat.Deps.Native.1.0.0.nupkg")
set(KANAT_DEPS_DIR "${CMAKE_CURRENT_BINARY_DIR}/third_party/kanat-deps")
if(NOT EXISTS "${KANAT_DEPS_ARCHIVE}")
	message(FATAL_ERROR "Missing dependency archive: ${KANAT_DEPS_ARCHIVE}")
endif()
file(ARCHIVE_EXTRACT INPUT "${KANAT_DEPS_ARCHIVE}" DESTINATION "${KANAT_DEPS_DIR}")

set(KANAT_DEPS_INCLUDE "${KANAT_DEPS_DIR}/build/native/include")
set(KANAT_DEPS_LIB_DEBUG "${KANAT_DEPS_DIR}/build/native/lib/debug/imguid.lib")
set(KANAT_DEPS_LIB_RELEASE "${KANAT_DEPS_DIR}/build/native/lib/release/imgui.lib")

set(KANAT_SRC
	src/main.cpp
	src/Identity.h
	src/framework/base/AppEnv.cpp
	src/framework/base/BasicDesign.cpp
	src/framework/base/ControlSpec.cpp
	src/framework/base/CtrlBuffer.cpp
	src/framework/base/TextWidth.cpp
	src/framework/base/mem/AllocatorHub.cpp
	src/framework/base/mem/MemoryManager.cpp
	src/framework/base/util/FileCodec.cpp
	src/framework/render/imgui/D3D11Manager.cpp
	src/framework/render/imgui/ImGuiHost.cpp
	src/framework/render/imgui/ability/ImGuiRender.cpp
	src/framework/render/imgui/control/Controls.cpp
	src/framework/render/imgui/control/Widgets.cpp
	src/framework/layout/GeometrySolver.cpp
	src/framework/layout/Layout.cpp
	src/framework/layout/LayoutBuilder.cpp
	src/framework/layout/LayoutModel.cpp
	src/framework/layout/LayoutRender.cpp
	src/framework/layout/LayoutSolver.cpp
	src/framework/dispatch/Dispatch.cpp
	src/framework/dispatch/DispatchScene.cpp
	src/framework/dispatch/FrameEvent.cpp
	src/framework/dispatch/ResourceLoader.cpp
	src/framework/dispatch/UIEventHandler.cpp
	src/demo/DemoEvents.cpp
)

add_executable(${APP_PROJECT_NAME} ${KANAT_SRC})

target_include_directories(${APP_PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${KANAT_DEPS_INCLUDE}"
)

target_link_libraries(${APP_PROJECT_NAME} PRIVATE
	"$<$<CONFIG:Debug>:${KANAT_DEPS_LIB_DEBUG}>"
	"$<$<NOT:$<CONFIG:Debug>>:${KANAT_DEPS_LIB_RELEASE}>"
	d3d11
	dxgi
	dwmapi
	imm32
)

target_compile_definitions(${APP_PROJECT_NAME} PRIVATE _AMD64_ UNICODE _UNICODE)

# Build the binary layout document next to the executable. Without Python the source JSON is copied.
get_filename_component(APP_DOC_STEM "${APP_DOC_NAME}" NAME_WE)
set(KANAT_DOC_BIN "${CMAKE_CURRENT_BINARY_DIR}/${APP_DOC_STEM}.bin")
find_program(KANAT_PYTHON NAMES python python3)

if(KANAT_PYTHON)
	add_custom_command(
		OUTPUT "${KANAT_DOC_BIN}"
		COMMAND "${KANAT_PYTHON}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/json2bin.py"
			"${CMAKE_CURRENT_SOURCE_DIR}/${APP_DOC_NAME}" "${KANAT_DOC_BIN}"
		DEPENDS
			"${CMAKE_CURRENT_SOURCE_DIR}/${APP_DOC_NAME}"
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/json2bin.py"
		COMMENT "Generating ${APP_DOC_STEM}.bin"
		VERBATIM
	)
	add_custom_target(${APP_PROJECT_NAME}DocBin DEPENDS "${KANAT_DOC_BIN}")
	add_dependencies(${APP_PROJECT_NAME} ${APP_PROJECT_NAME}DocBin)
	add_custom_command(TARGET ${APP_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${KANAT_DOC_BIN}"
			"$<TARGET_FILE_DIR:${APP_PROJECT_NAME}>/${APP_DOC_STEM}.bin"
	)
else()
	message(WARNING "Python not found: ${APP_DOC_STEM}.bin will not be generated; the source JSON is copied instead")
	add_custom_command(TARGET ${APP_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_CURRENT_SOURCE_DIR}/${APP_DOC_NAME}"
			"$<TARGET_FILE_DIR:${APP_PROJECT_NAME}>/${APP_DOC_NAME}"
	)
endif()
