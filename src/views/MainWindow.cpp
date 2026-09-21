#include "MainWindow.h"
#include "Theme.h"
#include "ApiClient.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>

// Jefe Views
#include "JefeDashboardView.h"
#include "JefeBitacoraView.h"
#include "JefeDevolucionesView.h"
#include "JefeUsuariosView.h"

// Bodega Views
#include "BodegaDashboardView.h"
#include "BodegaRegistroView.h"
#include "BodegaTrasladoView.h"
#include "BodegaInventarioView.h"

// Vendedor Views
#include "VendedorDashboardView.h"
#include "VendedorConsultaView.h"
#include "VendedorVentasView.h"
#include "VendedorTrasladoView.h"
#include "VendedorDevolucionView.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("NovaBook — Sistema de Gestión");
    resize(1200, 750);
    setMinimumSize(1050, 650);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar();
    mainLayout->addWidget(m_sidebarWidget);

    m_stack = new QStackedWidget(this);
    m_stack->setStyleSheet(QString("background-color: %1;").arg(Theme::BgDark));
    mainLayout->addWidget(m_stack, 1);
}

void MainWindow::setupSidebar() {
    m_sidebarWidget = new QWidget(this);
    m_sidebarWidget->setFixedWidth(240);
    m_sidebarWidget->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-right: 1px solid %2;
        }
    )").arg(Theme::BgSidebar).arg(Theme::Border));

    auto *sbLayout = new QVBoxLayout(m_sidebarWidget);
    sbLayout->setContentsMargins(16, 20, 16, 20);
    sbLayout->setSpacing(14);

    // Logo
    auto *logoLayout = new QHBoxLayout();
    auto *lblNova = new QLabel("NOVA", m_sidebarWidget);
    lblNova->setStyleSheet(QString("font-size: 20px; font-weight: 900; color: %1; letter-spacing: 1.5px; border:none;")
                           .arg(Theme::TextPrimary));
    auto *lblBook = new QLabel("BOOK", m_sidebarWidget);
    lblBook->setStyleSheet(QString("font-size: 20px; font-weight: 900; color: %1; letter-spacing: 1.5px; border:none;")
                           .arg(Theme::Accent));
    logoLayout->addWidget(lblNova);
    logoLayout->addWidget(lblBook);
    logoLayout->addStretch();
    sbLayout->addLayout(logoLayout);

    // Separator
    auto *sep1 = new QFrame(m_sidebarWidget);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet(QString("border-top: 1px solid %1; margin: 4px 0;").arg(Theme::Border));
    sbLayout->addWidget(sep1);

    // User Profile
    m_lblUserName = new QLabel("Cargando...", m_sidebarWidget);
    m_lblUserName->setStyleSheet(QString("font-size: 13px; font-weight: bold; color: %1; border:none;")
                                .arg(Theme::TextPrimary));
    m_lblUserName->setWordWrap(true);
    sbLayout->addWidget(m_lblUserName);

    m_lblRoleBadge = new QLabel("Rol", m_sidebarWidget);
    m_lblRoleBadge->setAlignment(Qt::AlignCenter);
    sbLayout->addWidget(m_lblRoleBadge);

    // Separator
    auto *sep2 = new QFrame(m_sidebarWidget);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet(QString("border-top: 1px solid %1; margin: 4px 0;").arg(Theme::Border));
    sbLayout->addWidget(sep2);

    // Navigation Container
    auto *navContainer = new QWidget(m_sidebarWidget);
    navContainer->setStyleSheet("background-color: transparent; border: none;");
    m_navLayout = new QVBoxLayout(navContainer);
    m_navLayout->setContentsMargins(0, 0, 0, 0);
    m_navLayout->setSpacing(6);
    sbLayout->addWidget(navContainer);

    sbLayout->addStretch();

    // Logout Button
    m_btnLogout = new QPushButton("⎋  Cerrar Sesión", m_sidebarWidget);
    m_btnLogout->setStyleSheet(QString(R"(
        QPushButton {
            background-color: transparent;
            color: %1;
            border: 1px solid %2;
            border-radius: 6px;
            padding: 8px 12px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: %3;
            color: %4;
            border-color: %4;
        }
    )").arg(Theme::TextMuted).arg(Theme::Border).arg(Theme::ErrorBg).arg(Theme::Error));
    m_btnLogout->setCursor(Qt::PointingHandCursor);
    sbLayout->addWidget(m_btnLogout);

    connect(m_btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::initUser(const UsuarioDto &user) {
    m_user = user;
    m_lblUserName->setText(user.nombreCompleto());
    m_lblRoleBadge->setText(Theme::roleName(user.id_roles));
    m_lblRoleBadge->setStyleSheet(Theme::roleBadgeStyle(user.id_roles));

    buildRoleViews();
}

QPushButton* MainWindow::createNavButton(const QString &text, int pageIndex) {
    auto *btn = new QPushButton(text, m_sidebarWidget);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(QString(R"(
        QPushButton {
            background-color: transparent;
            color: %1;
            text-align: left;
            padding: 10px 14px;
            border: none;
            border-radius: 6px;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #1A1A1A;
            color: %2;
        }
    )").arg(Theme::TextSidebar).arg(Theme::TextPrimary));

    connect(btn, &QPushButton::clicked, this, [this, pageIndex]() {
        onNavButtonClicked(pageIndex);
    });

    return btn;
}

void MainWindow::updateActiveNavButton(int activeIndex) {
    QString activeColor = Theme::roleColor(m_user.id_roles);

    for (int i = 0; i < m_navButtons.size(); ++i) {
        auto *btn = m_navButtons.at(i);
        if (i == activeIndex) {
            btn->setStyleSheet(QString(R"(
                QPushButton {
                    background-color: #1F1B12;
                    color: %1;
                    text-align: left;
                    padding: 10px 14px;
                    border-left: 3px solid %1;
                    border-radius: 4px;
                    font-size: 13px;
                    font-weight: bold;
                }
            )").arg(activeColor));
        } else {
            btn->setStyleSheet(QString(R"(
                QPushButton {
                    background-color: transparent;
                    color: %1;
                    text-align: left;
                    padding: 10px 14px;
                    border: none;
                    border-radius: 6px;
                    font-size: 13px;
                }
                QPushButton:hover {
                    background-color: #1A1A1A;
                    color: %2;
                }
            )").arg(Theme::TextSidebar).arg(Theme::TextPrimary));
        }
    }
}

void MainWindow::buildRoleViews() {
    // Clear existing nav buttons
    for (auto *b : m_navButtons) {
        m_navLayout->removeWidget(b);
        delete b;
    }
    m_navButtons.clear();

    // Clear existing stack pages
    while (m_stack->count() > 0) {
        QWidget *w = m_stack->widget(0);
        m_stack->removeWidget(w);
        delete w;
    }

    if (m_user.id_roles == 1 || m_user.id_roles == 4) {
        // --- JEFE / GERENTE ---
        m_jefeDashboard = new JefeDashboardView(m_stack);
        m_jefeBitacora = new JefeBitacoraView(m_stack);
        m_jefeDevoluciones = new JefeDevolucionesView(m_stack);
        m_jefeUsuarios = new JefeUsuariosView(m_stack);

        m_stack->addWidget(m_jefeDashboard);    // index 0
        m_stack->addWidget(m_jefeBitacora);     // index 1
        m_stack->addWidget(m_jefeDevoluciones); // index 2
        m_stack->addWidget(m_jefeUsuarios);     // index 3

        m_navButtons.append(createNavButton("⊞  Panel Principal", 0));
        m_navButtons.append(createNavButton("☰  Historial del Día", 1));
        m_navButtons.append(createNavButton("↩  Devoluciones", 2));
        m_navButtons.append(createNavButton("✦  Gestión de Usuarios", 3));

    } else if (m_user.id_roles == 2) {
        // --- BODEGA ---
        m_bodegaDashboard = new BodegaDashboardView(m_stack);
        m_bodegaRegistro = new BodegaRegistroView(m_stack);
        m_bodegaTraslado = new BodegaTrasladoView(m_stack);
        m_bodegaInventario = new BodegaInventarioView(m_stack);

        m_stack->addWidget(m_bodegaDashboard);  // index 0
        m_stack->addWidget(m_bodegaRegistro);   // index 1
        m_stack->addWidget(m_bodegaTraslado);   // index 2
        m_stack->addWidget(m_bodegaInventario); // index 3

        m_navButtons.append(createNavButton("⊞  Panel Principal", 0));
        m_navButtons.append(createNavButton("＋  Registrar Mercancía", 1));
        m_navButtons.append(createNavButton("➔  Requisición de Salida", 2));
        m_navButtons.append(createNavButton("◎  Consultar Inventario", 3));

    } else {
        // --- VENDEDOR ---
        m_vendedorDashboard = new VendedorDashboardView(m_stack);
        m_vendedorConsulta = new VendedorConsultaView(m_stack);
        m_vendedorVentas = new VendedorVentasView(m_stack);
        m_vendedorTraslado = new VendedorTrasladoView(m_stack);
        m_vendedorDevolucion = new VendedorDevolucionView(m_stack);

        m_stack->addWidget(m_vendedorDashboard);   // index 0
        m_stack->addWidget(m_vendedorConsulta);    // index 1
        m_stack->addWidget(m_vendedorVentas);      // index 2
        m_stack->addWidget(m_vendedorTraslado);    // index 3
        m_stack->addWidget(m_vendedorDevolucion);  // index 4

        m_navButtons.append(createNavButton("⊞  Panel Principal", 0));
        m_navButtons.append(createNavButton("◎  Consultar Existencias", 1));
        m_navButtons.append(createNavButton("↓  Baja por Venta", 2));
        m_navButtons.append(createNavButton("➔  Requisición a Bodega", 3));
        m_navButtons.append(createNavButton("↩  Generar Devolución", 4));
    }

    for (auto *b : m_navButtons) {
        m_navLayout->addWidget(b);
    }

    // Default to page 0
    onNavButtonClicked(0);
}

void MainWindow::onNavButtonClicked(int index) {
    if (index >= 0 && index < m_stack->count()) {
        m_stack->setCurrentIndex(index);
        updateActiveNavButton(index);

        // Trigger data load on page activation
        if (m_user.id_roles == 1 || m_user.id_roles == 4) {
            if (index == 0 && m_jefeDashboard) m_jefeDashboard->refreshData();
            else if (index == 1 && m_jefeBitacora) m_jefeBitacora->cargarMovimientos();
            else if (index == 2 && m_jefeDevoluciones) m_jefeDevoluciones->cargarDevoluciones();
            else if (index == 3 && m_jefeUsuarios) m_jefeUsuarios->cargarUsuarios();
        } else if (m_user.id_roles == 2) {
            if (index == 0 && m_bodegaDashboard) m_bodegaDashboard->refreshData();
            else if (index == 3 && m_bodegaInventario) m_bodegaInventario->cargarInventario();
        } else {
            if (index == 0 && m_vendedorDashboard) m_vendedorDashboard->refreshData();
            else if (index == 1 && m_vendedorConsulta) m_vendedorConsulta->buscarExistencias();
        }
    }
}

void MainWindow::onLogoutClicked() {
    if (QMessageBox::question(this, "Cerrar Sesión", "¿Deseas cerrar tu sesión actual?",
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        ApiClient::instance()->logout();
        emit logoutRequested();
    }
}
