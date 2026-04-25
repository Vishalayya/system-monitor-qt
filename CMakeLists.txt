cmake_minimum_required(VERSION 3.19)
project(System_info LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets charts)

qt_standard_project_setup()

qt_add_executable(System_info
    WIN32 MACOSX_BUNDLE
    main.cpp



    systeminfo.h systeminfo.cpp
    mainwindow.h mainwindow.cpp mainwindow.ui





)

target_link_libraries(System_info
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Charts
)

include(GNUInstallDirs)

install(TARGETS System_info
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET System_info
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
