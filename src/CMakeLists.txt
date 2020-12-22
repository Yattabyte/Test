###########
### Exe ###
###########
set(Module KeyTestExe)

# Configure and acquire files
set(FILES
    # Header files
    ${PROJECT_SOURCE_DIR}/external/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/external/glad/include/KHR/khrplatform.h
	${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.h
	${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.h
	${PROJECT_SOURCE_DIR}/external/imgui/imgui.h
    # Source files
    ${PROJECT_SOURCE_DIR}/external/glad/src/glad.c
	${PROJECT_SOURCE_DIR}/external/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
	${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
    main.cpp
)

# Create Library using the supplied files
add_executable(${Module} ${FILES})
target_include_directories(${Module}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/external/glad/include/
    PUBLIC ${PROJECT_SOURCE_DIR}/external/glfw/
    PUBLIC ${PROJECT_SOURCE_DIR}/external/imgui/
    PUBLIC ${PROJECT_SOURCE_DIR}/external/imgui/backends/
)

# Add library dependencies
target_compile_features(${Module} PRIVATE cxx_std_17)
target_link_libraries(${Module} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${Module} PUBLIC glfw OpenGL::GL)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CXX_COMPILER_VERSION}" LESS_EQUAL "9.0")
    target_link_libraries(${Module} PRIVATE c++experimental stdc++fs>)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(${Module} PRIVATE stdc++fs)
endif()

# Set all project settings
target_compile_Definitions(${Module} PRIVATE $<$<CONFIG:DEBUG>:DEBUG>)
set_target_properties(${Module} PROPERTIES VERSION ${PROJECT_VERSION})