# CHANGELOG - Frontend de Escritorio C++ / Qt 6 (Librería NovaBook)

Todos los cambios notables, decisiones de arquitectura, especificaciones técnicas, contratos de integración y reglas de diseño para evitar regresiones se documentan en este archivo.

El formato está basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.0.0/) y este proyecto se adhiere a [Semantic Versioning](https://semver.org/).

---

## 🤖 Guía Contra Regresiones para Agentes de IA y Desarrolladores

> [!IMPORTANT]
> **LEER ANTES DE MODIFICAR CUALQUIER ARCHIVO DEL PROYECTO.**  
> Este frontend está acoplado al backend REST `api_migrado`. Para hacer cambios, agregar funcionalidades o refactorizar sin romper lo que ya funciona, sigue obligatoriamente estas directivas:

### 1. Sistema de Estilos y Paleta de Colores Centralizada
- **NUNCA** coloques colores hexadecimales en bruto (`#...`) directamente en archivos `.cpp` o `.ui`.
- **SIEMPRE** utiliza las constantes y métodos utilitarios de [`src/core/Theme.h`](file:///home/iron/Proyectos/Ingenieria%20de%20Software/frontend_migrado/src/core/Theme.h):
  - Fondos: `Theme::BgDark` (`#0D0D0D`), `Theme::BgCard` (`#161616`), `Theme::BgInput` (`#1F1F1F`), `Theme::BgSidebar` (`#111111`), `Theme::BgRowAlt` (`#1A1A1A`).
  - Acentos: `Theme::Accent` (`#C8973A`), `Theme::AccentHover` (`#E0AE52`), `Theme::AccentDim` (`#7A5A1E`).
  - Textos: `Theme::TextPrimary` (`#F0EDE8`), `Theme::TextMuted` (`#7A7570`), `Theme::TextSidebar` (`#A09890`).
  - Bordes: `Theme::Border` (`#2A2A2A`), `Theme::BorderFocus` (`#C8973A`).
  - Estados: `Theme::Error` (`#E05C5C`), `Theme::Success` (`#5CBF8A`), `Theme::Warning` (`#E0B84A`).
  - Roles: `Theme::roleColor(roleId)`, `Theme::Role1` (Jefe - `#C8973A`), `Theme::Role2` (Bodega - `#5C9FBF`), `Theme::Role3` (Vendedor - `#8ABF5C`).
  - Botones: `Theme::buttonStyle(...)`, `Theme::secondaryButtonStyle()`, `Theme::dangerButtonStyle()`.
- Cualquier modificación a la hoja de estilos global debe hacerse en `Theme::globalStyleSheet()`.

### 2. Contrato de Integración con `api_migrado` (Backend)
- Toda comunicación HTTP pasa por [`ApiClient`](file:///home/iron/Proyectos/Ingenieria%20de%20Software/frontend_migrado/src/core/ApiClient.h) (`ApiClient::instance()`).
- **Estructura de respuesta esperada:**
  - Éxito: `{"success": true, "data": <T>, "message": "..."}`
  - Error: `{"success": false, "error": {"code": "...", "message": "..."}}`
- **Tipos de datos numéricos grandes:**
  - Códigos EAN (`codigo_ean`), identificadores de usuario (`id_usuarios`), números de teléfono (`telefono`) y ventas (`id_venta`) superan los 32 bits. **SIEMPRE** deben manejarse como `qint64`, nunca como `int`.
- **Autenticación:**
  - El token se almacena en memoria en `ApiClient` y se inyecta como cabecera `Authorization: Bearer <token>`.
  - Códigos HTTP 401 emiten la señal `ApiClient::sessionExpired()`, la cual redirige de forma segura al Login.

### 3. Modelo por Roles y Permisos (RBAC)
No mezclar ni exponer vistas entre roles no autorizados:
- **Rol 1 (Jefe de Departamento) y Rol 4 (Gerente):** Tienen acceso a `JefeDashboardView`, `JefeBitacoraView`, `JefeDevolucionesView` y `JefeUsuariosView`.
- **Rol 2 (Personal de Bodega):** Tiene acceso a `BodegaDashboardView`, `BodegaRegistroView`, `BodegaTrasladoView` y `BodegaInventarioView`.
- **Rol 3 (Vendedor):** Tiene acceso a `VendedorDashboardView`, `VendedorConsultaView`, `VendedorVentasView`, `VendedorTrasladoView` y `VendedorDevolucionView`.

### 4. Manejo de Memoria y Ciclo de Vida en Qt 6
- Pasa siempre el puntero `parent` a los constructores de `QWidget`, `QDialog`, `QLayout` y `QFrame` para garantizar la destrucción automática en cascada por el árbol de objetos de Qt.
- En peticiones de red asíncronas con `QNetworkReply`, llamar siempre a `reply->deleteLater()` al terminar.
- Diálogos modales secundarios (`QDialog`) deben ejecutarse con `.exec()` o `.open()`.

### 5. Compilación y MOC (Meta-Object Compiler)
- Si una clase utiliza `signals:`, `slots:` o propiedades de Qt, **es obligatorio** incluir la macro `Q_OBJECT` al inicio de su definición.
- Si agregas una nueva clase o archivo `.cpp` / `.h`, **debes actualizar obligatoriamente** `CMakeLists.txt`:
  - Agregar el `.cpp` en la lista `set(SOURCES ...)`.
  - Agregar el `.h` en la lista `set(HEADERS ...)`.
  - Si creas un nuevo subdirectorio, agregarlo en `include_directories(...)` en `CMakeLists.txt` y en `.vscode/c_cpp_properties.json`.
- **Verificación obligatoria de compilación:**
  ```bash
  cmake -B build -S . && cmake --build build -j$(nproc)
  ```
  El binario resultante debe generar `[100%] Built target novabook_frontend` con código de retorno 0.

### 6. 📝 PROTOCOLO OBLIGATORIO DE REGISTRO DE CAMBIOS PARA IAs
> [!CAUTION]
> **OBLIGATORIO:** Cada vez que un agente de IA o programador realice una modificación en este proyecto (creación de archivos, corrección de bugs, adición de endpoints, cambio de diseño o refactorización), **TIENE QUE ESCRIBIR OBLIGATORIAMENTE SUS CAMBIOS EN ESTE ARCHIVO** en la sección `[Unreleased]` antes de concluir su respuesta.

**Estructura requerida para documentar cada cambio:**
```markdown
### ➕ Añadido (Added)
- **[NombreComponente/Archivo]**: Descripción detallada de qué se agregó y para qué sirve.

### 🔄 Modificado (Changed)
- **[NombreComponente/Archivo]**: Qué lógica o diseño cambió, por qué fue necesario y qué archivos impacta.

### 🐛 Corregido (Fixed)
- **[NombreComponente/Archivo]**: Descripción del bug o error corregido y causa raíz.

### 🗑️ Eliminado (Removed)
- **[NombreComponente/Archivo]**: Elementos deprecados o removidos y su sustituto.
```
- **Regla de oro:** No sobrescribir el historial de versiones previas. Siempre agregar los cambios nuevos en `[Unreleased]` o bajo una nueva versión fechada.

---

## [Unreleased]

### ➕ Añadido (Added)
- **Web SPA (`web/index.html`, `web/styles.css`, `web/app.js`)**: Aplicación web SPA completa responsiva y corporativa construida con la paleta oficial (`#0D0D0D`, `#161616`, `#C8973A`), con vistas dedicadas para Gerente (CRUD Usuarios), Bodega (Alta libros/revistas y traslados Bodega $\to$ Tienda), Jefe (Historial de devoluciones, evaluación/aprobación y bitácora) y Vendedor (POS interactivo multilínea, consulta en vivo, traslados y solicitudes con descarga directa de PDF).
- **Script de Pruebas E2E (`api_migrado/test_e2e.sh`)**: Suite automatizada con curl que verifica todos los endpoints REST y transacciones atómicas en MySQL para los 4 roles.

### 🔄 Modificado (Changed)
- **`LoginWindow.cpp`**: Integración de botones interactivos de acceso rápido para los 4 roles (Gerente `1001`, Jefe `350976899`, Bodega `628777130`, Vendedor `628777129`) con auto-llenado y envío de login automático para pruebas rápidas. Adición del distintivo dorado del rol Gerente.
- **`MainWindow.cpp`**: Sincronización y confirmación de soporte para el rol Gerente (`id_roles = 4`), habilitando acceso directo al panel administrativo y al módulo completo de CRUD de usuarios (`JefeUsuariosView`).
- **`api_migrado/src/productos/repo/producto.rs`**: Adaptación a `sqlx 0.9` envolviendo consultas SQL dinámicas con `sqlx::AssertSqlSafe(sql.as_str())`. Eliminación de imports en desuso.
- **`api_migrado/src/productos/service/producto.rs`**: Conexión a métodos transaccionales del repositorio y limpieza de advertencias de compilación.

### 🐛 Corregido (Fixed)
- **Transacciones de Venta en Almacén (`repo/producto.rs`)**: Corrección de validación de `tipo_producto` ("libro" vs "revista") para el descuento atómico de stock en Piso de Ventas (`id_ubicacion = 1`) y serialización correcta en `detalle_ventas`.
- **Restricciones de Llaves Foráneas (`test_e2e.sh`)**: Corrección de valores de `id_genero` e `id_mueble` a claves existentes (`11`) en la base de datos `Libreria`.

---

## [0.1.0] - 2026-09-18

### 🚀 Implementación Inicial

#### 1. Arquitectura y Módulos de Código
- **Lenguaje y Framework:** C++17 nativo con **Qt 6** (`Qt6Widgets`, `Qt6Network`, `Qt6Core`, `Qt6Gui`).
- **Herramienta de Construcción:** **CMake** 3.16+ con `CMAKE_AUTOMOC`, `CMAKE_AUTORCC`, `CMAKE_AUTOUIC` y `CMAKE_EXPORT_COMPILE_COMMANDS`.
- **Estructura del Proyecto:**
  - `src/core/`:
    - `Theme.h` / `Theme.cpp`: Sistema de temas en modo oscuro con la paleta oficial (`#0D0D0D`, `#161616`, `#C8973A`, etc.), badges por rol y estilos de inputs, botones y tablas.
    - `Models.h`: Modelos de datos para serializar y deserializar respuestas JSON (`UsuarioDto`, `ExistenciaInventario`, `ItemVenta`, `VentaResponse`, `TrasladoResponse`, `Devolucion`, `ResumenMovimientoDiario`).
    - `ApiClient.h` / `ApiClient.cpp`: Cliente HTTP asíncrono singleton basado en `QNetworkAccessManager`, inyección automática de JWT `Bearer`, manejo de errores y descarga binaria de reportes PDF.
  - `src/widgets/`:
    - `StatCard.h` / `StatCard.cpp`: Tarjetas de resumen métrico reutilizables con acentos cromáticos.
    - `Toast.h` / `Toast.cpp`: Notificaciones flotantes animadas temporizadas (Éxito, Error, Advertencia, Información).
  - `src/views/`:
    - `LoginWindow.h` / `LoginWindow.cpp`: Ventana de autenticación centrada estilo tarjeta, selector de endpoint API, validaciones y píldoras indicadoras de roles.
    - `MainWindow.h` / `MainWindow.cpp`: Shell contenedor con barra de navegación lateral fija adaptativa por rol, tarjeta de perfil de usuario y control de sesión.
  - `src/views/jefe/`:
    - `JefeDashboardView`: Métricas operacionales del día.
    - `JefeBitacoraView`: Visor y filtro por fecha de la bitácora general de movimientos de la sucursal.
    - `JefeDevolucionesView`: Historial de devoluciones a proveedores con diálogo modal de evaluación (autorizar/rechazar con notas).
    - `JefeUsuariosView`: CRUD completo de usuarios del sistema (alta, edición de datos/contraseña y baja).
  - `src/views/bodega/`:
    - `BodegaDashboardView`: Indicadores de inventario en almacén.
    - `BodegaRegistroView`: Formulario de alta de mercancía (libros y revistas) con validación estricta de EAN-13 (rango 9500000000000..9999999999999), SKU (6-7 dígitos), género, proveedor y estantería.
    - `BodegaTrasladoView`: Requisición de salida (Bodega $\to$ Piso de Tienda) con cálculo inmediato de stock restante.
    - `BodegaInventarioView`: Consulta y búsqueda rápida de existencias en almacén y tienda.
  - `src/views/vendedor/`:
    - `VendedorDashboardView`: Indicadores de catálogo y ventas.
    - `VendedorConsultaView`: Buscador en tiempo real con filtros por ubicación (Piso/Bodega) y tipo de producto.
    - `VendedorVentasView`: Punto de venta (POS) con carrito dinámico, cálculo de subtotales, totales y generación de comprobante de venta digital.
    - `VendedorTrasladoView`: Requisición de entrada (Piso de Tienda $\to$ Bodega).
    - `VendedorDevolucionView`: Solicitud formal de devolución a proveedor y botón de descarga de acta oficial en formato PDF.

---

### 🎨 Paleta de Colores Implementada

| Variable de Color | Código Hex | Rol / Elemento de la Interfaz |
| :--- | :---: | :--- |
| `bg_dark` | `#0D0D0D` | Fondo general de ventanas y vistas de contenido |
| `bg_card` | `#161616` | Contenedores de formularios, tarjetas de datos y tablas |
| `bg_input` | `#1F1F1F` | Cajas de texto, selectores y spinboxes |
| `bg_sidebar` | `#111111` | Barra lateral fija de navegación |
| `bg_row_alt` | `#1A1A1A` | Filas alternadas en tablas para legibilidad |
| `accent` | `#C8973A` | Color primario corporativo (Dorado NovaBook) |
| `accent_hover` | `#E0AE52` | Estado hover de botones de acción principal |
| `accent_dim` | `#7A5A1E` | Acentos secundarios y bordes suaves |
| `text_primary` | `#F0EDE8` | Texto principal, encabezados y datos clave |
| `text_muted` | `#7A7570` | Textos secundarios, etiquetas de campo y placeholders |
| `text_sidebar` | `#A09890` | Opciones de navegación no seleccionadas |
| `border` | `#2A2A2A` | Separadores, divisores y contornos de tarjetas |
| `border_focus` | `#C8973A` | Resaltado al enfocar campos de entrada |
| `error` / `error_bg` | `#E05C5C` / `#2A1010` | Mensajes de error, alertas y botón de rechazo |
| `success` / `success_bg` | `#5CBF8A` / `#0F2A1A` | Ventas procesadas, autorizaciones y confirmaciones |
| `warning` | `#E0B84A` | Devoluciones pendientes y avisos |
| `role1` | `#C8973A` | Distintivo dorado: Jefe de Departamento / Gerente |
| `role2` | `#5C9FBF` | Distintivo azul: Personal de Bodega |
| `role3` | `#8ABF5C` | Distintivo verde: Vendedor |

---

### 🔧 Entorno de Desarrollo y Configuración IDE
- **VSCode:** Se configuró `.vscode/c_cpp_properties.json` y `.vscode/settings.json` apuntando a `build/compile_commands.json` y a las cabeceras de sistema de Qt 6 (`/usr/include/qt6`), eliminando errores de IntelliSense en inclusiones.
- **Pruebas de inicialización:** Verificado mediante ejecución *offscreen* (`QT_QPA_PLATFORM=offscreen`).
