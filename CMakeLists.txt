cmake_minimum_required(VERSION 3.16)

project(pWireServer VERSION "0.8.0" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

#include(ExternalProject)
#ExternalProject_Add(
#        SPWL
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/spwl
#        CMAKE_ARGS
#        ${GLOBAL_DEFAULT_ARGS}
#        ${GLOBAL_THIRDPARTY_LIB_ARGS}
#        TARGET_STEPS SPWL
#        INSTALL_COMMAND ""
#)
#set(SPWL_root ${CMAKE_CURRENT_BINARY_DIR}/SPWL-prefix/src)
#set(SPWL_include ${CMAKE_SOURCE_DIR}/spwl/lib/include/)
#set(SPWL_lib ${SPWL_root}/SPWL-build)
#
#ExternalProject_Add(
#        wiring_np
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/wiring_np/wiringPi
#        CONFIGURE_COMMAND ""
#        BUILD_COMMAND make -j -C <SOURCE_DIR> static
#)
#set(wiring_np_root ${CMAKE_SOURCE_DIR}/third_party/wiring_np/wiringPi)
#set(wiring_np_lib ${wiring_np_root})
#set(wiring_np_include ${wiring_np_root})


include(GNUInstallDirs)


set(${PROJECT_NAME}_SOURCES
        lib/src/logger.cpp
        lib/src/lora-module.cpp
        lib/src/pwire-server-lib.cpp
        lib/src/redis-service.cpp
        )

add_subdirectory("spwl" EXCLUDE_FROM_ALL)
add_subdirectory("third_party/wiring_np" EXCLUDE_FROM_ALL)
add_library(${PROJECT_NAME}
        STATIC
        ${${PROJECT_NAME}_SOURCES})
add_dependencies(${PROJECT_NAME} SPWL)
target_link_libraries(${PROJECT_NAME} PRIVATE SPWL)
add_dependencies(${PROJECT_NAME} wiringNP)
target_link_libraries(${PROJECT_NAME} PRIVATE wiringNP)

target_compile_features(${PROJECT_NAME} PRIVATE
        "cxx_std_17"
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        #WINDOWS_EXPORT_ALL_SYMBOLS YES
        )

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ./lib/include/
        ./spwl/lib/include
        ./third_party/wiring_np/wiringPi
)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE redis++)
target_link_libraries(${PROJECT_NAME} PRIVATE hiredis)

## Library installation
## Installation
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "lib/include/pwire-server-lib.h;lib/include/logger.h")
#INSTALL(TARGETS ${PROJECT_NAME}
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
#        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
#        COMPONENT pwire-server-backend-library
#        )

# Executable
add_executable(
        pWireServerExecutable
        exe/src/pwire-server.cpp
)

add_dependencies(pWireServerExecutable ${PROJECT_NAME})
target_link_libraries(pWireServerExecutable PRIVATE ${PROJECT_NAME})

target_include_directories(
        pWireServerExecutable
        PRIVATE
        ./exe/include/
)
set_target_properties(pWireServerExecutable PROPERTIES OUTPUT_NAME pwire-server)

INSTALL(TARGETS pWireServerExecutable
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )

### Testing
#enable_testing()
#add_executable(
#        unit_tests
#        test/src/pwire-server-test.cpp
#        ${${PROJECT_NAME}_SOURCES})
#target_include_directories(
#        unit_tests
#        PRIVATE
#        ./test/include/
#        ./test/cute
#)
#
#target_compile_features(unit_tests PUBLIC
#        cxx_std_17
#        )
#
#set_target_properties(unit_tests PROPERTIES
#        CXX_STANDARD_REQUIRED YES
#        CXX_EXTENSIONS NO
#        )
#target_compile_options(unit_tests PRIVATE -g -O0 -fprofile-arcs -ftest-coverage -fPIC)
#target_link_libraries(
#        unit_tests
#        gcov
#)
## End Testing
