cmake_minimum_required(VERSION 3.16)

project(pWireServer VERSION "0.8.0" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)
ExternalProject_Add(
        SPWL
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/spwl
        CMAKE_ARGS
        ${GLOBAL_DEFAULT_ARGS}
        ${GLOBAL_THIRDPARTY_LIB_ARGS}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/spwl
        #    BUILD_COMMAND make
)
set(SPWL_root ${CMAKE_CURRENT_BINARY_DIR}/spwl)
set(SPWL_include ${SPWL_root}/include)
set(SPWL_lib ${SPWL_root}/lib)

ExternalProject_Add(
        wiring_np
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/wiring_np/wiringPi
        #CMAKE_ARGS
        #${GLOBAL_DEFAULT_ARGS}
        #${GLOBAL_THIRDPARTY_LIB_ARGS}
        #-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/wiring_np
        CONFIGURE_COMMAND ""
        BUILD_COMMAND make -j -C <SOURCE_DIR> static
        INSTALL_COMMAND ""
)
set(wiring_np_root ${CMAKE_SOURCE_DIR}/third_party/wiring_np/wiringPi)
set(wiring_np_lib ${wiring_np_root})
set(wiring_np_include ${wiring_np_root})


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
add_dependencies(${PROJECT_NAME} SPWL)
add_dependencies(${PROJECT_NAME} wiring_np)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SPWL_lib}/libSPWL.a)
target_link_libraries(${PROJECT_NAME} PRIVATE ${wiring_np_lib}/libwiringPi.a)

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
        ${SPWL_include}
        ${wiring_np_include}
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

#target_link_libraries(eOmegaTestAdapterExecutable PRIVATE Threads::Threads)
#
#target_link_directories(eOmegaTestAdapterExecutable PRIVATE ${zBleAdapter_lib})
#target_link_libraries(eOmegaTestAdapterExecutable PRIVATE zBleAdapter0_x86)
#
## Testing
#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
#)
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)
#
#enable_testing()
#
#add_executable(
#        unit_tests
#        test/src/common/helper.cpp
#        test/src/zwickInterface/interfaceStringAnalyzer.cc
#        test/src/zwickInterface/interfaceStringBuilder.cc
#        test/src/medicalInteractionProtocol/messageDispatcherTest.cc
#        test/src/medicalInteractionProtocol/userMailTest.cc
#        test/src/eOmegaTestAdapter/pmUartTest.cc
#        test/src/eOmegaTestAdapter/pmDeviceStatusTest.cc
#        ${${PROJECT_NAME}_SOURCES})
#add_dependencies(unit_tests zBleAdapter)
#
#target_link_directories(unit_tests PRIVATE ${zBleAdapter_lib})
#target_link_libraries(
#        unit_tests
#        PRIVATE
#        gtest_main
#        gmock_main
#        zBleAdapter0_x86
#)
#
#target_include_directories(
#        unit_tests
#        PRIVATE
#        ./include
#        ./test/include
#        ${zBleAdapter_include}
#)
#
#target_compile_options(unit_tests PRIVATE "-DGTEST")
#
#target_compile_features(unit_tests PUBLIC
#        cxx_std_17
#        )
#
#set_target_properties(unit_tests PROPERTIES
#        CXX_STANDARD_REQUIRED YES
#        CXX_EXTENSIONS NO
#        )
#
#target_link_libraries(unit_tests PRIVATE gmock_main gtest_main)
#target_link_libraries(unit_tests PRIVATE Threads::Threads)
#
## Mockable macro
#target_compile_definitions(unit_tests PRIVATE "MAKE_MOCKABLE=virtual")
#target_compile_definitions(eOmegaTestAdapterExecutable PRIVATE "MAKE_MOCKABLE=/**/")
#target_compile_definitions(${LIBRARY_NAME} PRIVATE "MAKE_MOCKABLE=/**/")
#
#include(GoogleTest)
# End Testing