cmake_minimum_required(VERSION 3.5.0)
set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(lab_4 VERSION ${PROJECT_VESRION})


add_executable(lab_4 main.cpp lib.cpp)
add_library(lib lib.cpp)

set_target_properties(lab_4 lib PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(lib
    PRIVATE "${CMAKE_BINARY_DIR}"
)
target_link_libraries(lab_4 PRIVATE
lib
)

if (MSVC)
    target_compile_options(lab_4 PRIVATE
        /W4
    )
    target_compile_options(lib PRIVATE
        /W4
    )
else ()
    target_compile_options(lab_4 PRIVATE
        
    )
    target_compile_options(lib PRIVATE
         
    )
endif()

install(TARGETS lab_4 RUNTIME DESTINATION bin)

if (${CMAKE_SYSTEM} MATCHES "Linux")
    set(CPACK_GENERATOR DEB)
else ()
    set(CPACK_GENERATOR ZIP)
endif()

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})