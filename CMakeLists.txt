cmake_minimum_required(VERSION 3.17)
project(XingDBG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 6)
set(REQUIRED_LIBS Core Gui Widgets)
set(REQUIRED_LIBS_QUALIFIED Qt6::Core Qt6::Gui Qt6::Widgets)
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

set(KDDockWidgets_QT6 ON)
set(build_for_qt6 ON)
set(build_examples OFF)
set(KDDockWidgets_STATIC ON)
set(build_static ON)
add_subdirectory(3rdparty/KDDockWidgets)

include_directories(3rdparty/KDDockWidgets/src/fwd_headers)

add_executable(${PROJECT_NAME}
#        WIN32
        main.cpp
        MainWindow.cpp
        MainWindow.h
        LLDBCore.cpp
        LLDBCore.h
        App.cpp
        App.h
        DisassemblyView.cpp
        DisassemblyView.h
        RegisterView.cpp
        RegisterView.h
        ThreadView.cpp
        ThreadView.h
        CallStackView.cpp
        CallStackView.h
        OutputView.cpp
        OutputView.h
        HexView.cpp
        HexView.h
        MemoryView.cpp
        MemoryView.h
        AbstractTableView.cpp
        AbstractTableView.h
        OpenExeDlg.cpp OpenExeDlg.h AttachDlg.cpp AttachDlg.h SymbolView.cpp SymbolView.h Utils.h SelectPlatformDlg.cpp SelectPlatformDlg.h)

target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED} KDAB::kddockwidgets lldb)
