cmake_minimum_required(VERSION 3.17)
project(XingDBG)

set(CMAKE_CXX_STANDARD 20)
set(KDDockWidgets_QT6 ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 6)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt6::Core Qt6::Gui Qt6::Widgets)


find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

set(KDDockWidgets_STATIC true)
add_subdirectory(3rdparty/KDDockWidgets)

include_directories(3rdparty/KDDockWidgets/src/fwd_headers)

add_executable(${PROJECT_NAME} WIN32 main.cpp MainWindow.cpp MainWindow.h LLDBCore.cpp LLDBCore.h App.cpp App.h DisassemblyView.cpp DisassemblyView.h RegisterView.cpp RegisterView.h ThreadView.cpp ThreadView.h CallStackView.cpp CallStackView.h)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED} KDAB::kddockwidgets lldb.dll)
