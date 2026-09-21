# NovaBook — Frontend de Escritorio en C++ (Qt 6)

Cliente de escritorio nativo desarrollado en **C++ con Qt 6** para el sistema de gestión de librería **NovaBook**, consumiendo los servicios REST de `api_migrado`.

---

## 🎨 Paleta de Colores Corporativa

La aplicación aplica una interfaz en modo oscuro de alto contraste:

- **Fondos:**
  - `bg_dark`: `#0D0D0D` (Fondo de ventana y vistas)
  - `bg_card`: `#161616` (Tarjetas, formularios y tablas)
  - `bg_input`: `#1F1F1F` (Inputs y cajas de selección)
  - `bg_sidebar`: `#111111` (Barra lateral fija de navegación)
  - `bg_row_alt`: `#1A1A1A` (Filas alternas de tablas)
- **Acentos y Estados:**
  - `accent`: `#C8973A` (Dorado distintivo corporativo)
  - `accent_hover`: `#E0AE52`
  - `accent_dim`: `#7A5A1E`
  - `error` / `error_bg`: `#E05C5C` / `#2A1010`
  - `success` / `success_bg`: `#5CBF8A` / `#0F2A1A`
  - `warning`: `#E0B84A`
- **Distintivos de Rol:**
  - **Role 1 (Jefe / Gerente):** `#C8973A` (Dorado)
  - **Role 2 (Personal de Bodega):** `#5C9FBF` (Azul)
  - **Role 3 (Vendedor):** `#8ABF5C` (Verde)

---

## 🏛 Estructura y Arquitectura

```
frontend_migrado/
├── CMakeLists.txt                 # Configuración de compilación con Qt6
├── README.md                      # Documentación del proyecto
└── src/
    ├── main.cpp                  # Inicialización y arranque de QApplication
    ├── core/
    │   ├── Theme.h / .cpp        # Paleta de colores y hoja de estilos global QSS
    │   ├── Models.h              # Estructuras de datos (Usuario, Inventario, Venta, etc.)
    │   └── ApiClient.h / .cpp    # Cliente HTTP asíncrono sobre Qt Network (JWT Bearer)
    ├── widgets/
    │   ├── StatCard.h / .cpp     # Tarjetas de métricas con borde de acento
    │   └── Toast.h / .cpp        # Notificaciones flotantes animadas
    └── views/
        ├── LoginWindow.h / .cpp  # Pantalla de acceso con selector de servidor y validación
        ├── MainWindow.h / .cpp   # Ventana contenedora principal con sidebar dinámico
        ├── jefe/                 # Vistas exclusivas de Jefe / Gerente
        │   ├── JefeDashboardView.h / .cpp
        │   ├── JefeBitacoraView.h / .cpp
        │   ├── JefeDevolucionesView.h / .cpp
        │   └── JefeUsuariosView.h / .cpp
        ├── bodega/               # Vistas exclusivas de Bodega
        │   ├── BodegaDashboardView.h / .cpp
        │   ├── BodegaRegistroView.h / .cpp
        │   ├── BodegaTrasladoView.h / .cpp
        │   └── BodegaInventarioView.h / .cpp
        └── vendedor/             # Vistas exclusivas de Vendedor
            ├── VendedorDashboardView.h / .cpp
            ├── VendedorConsultaView.h / .cpp
            ├── VendedorVentasView.h / .cpp
            ├── VendedorTrasladoView.h / .cpp
            └── VendedorDevolucionView.h / .cpp
```

---

## ⚙️ Requisitos

- Compilador C++ compatible con **C++17** (`g++` o `clang++`).
- **CMake** >= 3.16.
- **Qt 6** con los módulos:
  - `Qt6Core`
  - `Qt6Gui`
  - `Qt6Widgets`
  - `Qt6Network`

---

## 🚀 Compilación y Ejecución

### 1. Configurar y Compilar con CMake

```bash
cd "/home/iron/Proyectos/Ingenieria de Software/frontend_migrado"
cmake -B build -S .
cmake --build build -j$(nproc)
```

### 2. Ejecutar la Aplicación

```bash
./build/novabook_frontend
```

---

## 🔑 Credenciales de Prueba

| Rol | ID Usuario | Contraseña | Vistas Habilitadas |
|---|---|---|---|
| **Jefe de Departamento** | `350976899` | `2501` | Dashboard, Historial del Día, Devoluciones, Usuarios |
| **Personal de Bodega** | `628777130` | `7777` | Dashboard, Registrar Mercancía, Requisición Salida, Inventario |
| **Vendedor** | `628777129` | `8888` | Dashboard, Consultar Existencias, Baja por Venta (POS), Requisición a Bodega, Devoluciones |

> **Nota:** La aplicación apunta por defecto a `http://127.0.0.1:3000`. Puedes cambiar la dirección del servidor en la pantalla de inicio de sesión si el backend `api_migrado` está alojado en otro host o puerto.
