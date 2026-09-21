cmake_minimum_required(VERSION 3.16)
project(novabook_frontend VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

include_directories(
    src
    src/core
    src/widgets
    src/views
    src/views/jefe
    src/views/bodega
    src/views/vendedor
)

set(SOURCES
    src/main.cpp
    src/core/Theme.cpp
    src/core/ApiClient.cpp
    src/widgets/StatCard.cpp
    src/widgets/Toast.cpp
    src/views/LoginWindow.cpp
    src/views/MainWindow.cpp
    src/views/jefe/JefeDashboardView.cpp
    src/views/jefe/JefeBitacoraView.cpp
    src/views/jefe/JefeDevolucionesView.cpp
    src/views/jefe/JefeUsuariosView.cpp
    src/views/bodega/BodegaDashboardView.cpp
    src/views/bodega/BodegaRegistroView.cpp
    src/views/bodega/BodegaTrasladoView.cpp
    src/views/bodega/BodegaInventarioView.cpp
    src/views/vendedor/VendedorDashboardView.cpp
    src/views/vendedor/VendedorConsultaView.cpp
    src/views/vendedor/VendedorVentasView.cpp
    src/views/vendedor/VendedorTrasladoView.cpp
    src/views/vendedor/VendedorDevolucionView.cpp
)

set(HEADERS
    src/core/Theme.h
    src/core/Models.h
    src/core/ApiClient.h
    src/widgets/StatCard.h
    src/widgets/Toast.h
    src/views/LoginWindow.h
    src/views/MainWindow.h
    src/views/jefe/JefeDashboardView.h
    src/views/jefe/JefeBitacoraView.h
    src/views/jefe/JefeDevolucionesView.h
    src/views/jefe/JefeUsuariosView.h
    src/views/bodega/BodegaDashboardView.h
    src/views/bodega/BodegaRegistroView.h
    src/views/bodega/BodegaTrasladoView.h
    src/views/bodega/BodegaInventarioView.h
    src/views/vendedor/VendedorDashboardView.h
    src/views/vendedor/VendedorConsultaView.h
    src/views/vendedor/VendedorVentasView.h
    src/views/vendedor/VendedorTrasladoView.h
    src/views/vendedor/VendedorDevolucionView.h
)

add_executable(novabook_frontend ${SOURCES} ${HEADERS})

target_link_libraries(novabook_frontend PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
