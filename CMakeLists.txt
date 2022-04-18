cmake_minimum_required(VERSION 3.16)

project(pWireServer VERSION "0.8.0" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

#include(ExternalProject)
#ExternalProject_Add(
#        spwl_install
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/spwl
#        CMAKE_ARGS
#        ${GLOBAL_DEFAULT_ARGS}
#        ${GLOBAL_THIRDPARTY_LIB_ARGS}
#        STEP_TARGETS  SPWL
#)
#
#ExternalProject_Add(
#        wiring_np_install
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/wiring_np/wiringPi
#        CONFIGURE_COMMAND ""
#        BUILD_COMMAND make -j -C <SOURCE_DIR> install-static
#        INSTALL_COMMAND ""
#)

include(GNUInstallDirs)


set(${PROJECT_NAME}_SOURCES
        lib/src/logger.cpp
        lib/src/lora-module.cpp
        lib/src/pwire-server-lib.cpp
        lib/src/redis-service.cpp
        )

add_library(${PROJECT_NAME}
        STATIC
        ${${PROJECT_NAME}_SOURCES})
#add_dependencies(${PROJECT_NAME} spwl_install)
#add_dependencies(${PROJECT_NAME} wiring_np_install)
#target_link_libraries(${PROJECT_NAME} PRIVATE SPWL)
#target_link_libraries(${PROJECT_NAME} PRIVATE wiringPi)

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
)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE redis++)
target_link_libraries(${PROJECT_NAME} PRIVATE hiredis)
target_link_libraries(${PROJECT_NAME} PRIVATE hiredis)

## Library installation
## Installation
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "lib/include/pwire-server-lib.h;lib/include/logger.h")
INSTALL(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        )

#target_link_directories(${LIBRARY_NAME} PRIVATE ${zBleAdapter_lib})
#target_link_libraries(${LIBRARY_NAME} PRIVATE zBleAdapter0_x86)


# Packaging
#file(GLOB_RECURSE HEADER_DATA_TYPES include/eOmegaTestAdapter/zwickInterface/*.h)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.dll
#        DESTINATION lib
#        COMPONENT library)
#if(MSVC)
#    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.lib
#            DESTINATION lib
#            COMPONENT libraryHeader)
#endif()
#install(FILES ${CMAKE_SOURCE_DIR}/assets/conf/eOmegaTestAdapter.ini
#        DESTINATION conf
#        COMPONENT conf)
# install(FILES ${HEADER_DATA_TYPES}
#         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/eOmegaTestAdapter/zwickInterface
#         COMPONENT headers)
# install(FILES include/eOmegaTestAdapter/eOmegaTestAdapter.h
#         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/eOmegaTestAdapter
#         COMPONENT headers)

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