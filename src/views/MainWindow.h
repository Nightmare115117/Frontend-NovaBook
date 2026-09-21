#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMap>
#include "Models.h"

class JefeDashboardView;
class JefeBitacoraView;
class JefeDevolucionesView;
class JefeUsuariosView;

class BodegaDashboardView;
class BodegaRegistroView;
class BodegaTrasladoView;
class BodegaInventarioView;

class VendedorDashboardView;
class VendedorConsultaView;
class VendedorVentasView;
class VendedorTrasladoView;
class VendedorDevolucionView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void initUser(const UsuarioDto &user);

signals:
    void logoutRequested();

private slots:
    void onLogoutClicked();
    void onNavButtonClicked(int index);

private:
    void setupSidebar();
    void buildRoleViews();
    QPushButton* createNavButton(const QString &text, int pageIndex);
    void updateActiveNavButton(int activeIndex);

    UsuarioDto m_user;

    QWidget *m_sidebarWidget;
    QVBoxLayout *m_navLayout;
    QLabel *m_lblUserName;
    QLabel *m_lblRoleBadge;
    QPushButton *m_btnLogout;

    QStackedWidget *m_stack;
    QList<QPushButton*> m_navButtons;

    // Jefe Views
    JefeDashboardView *m_jefeDashboard = nullptr;
    JefeBitacoraView *m_jefeBitacora = nullptr;
    JefeDevolucionesView *m_jefeDevoluciones = nullptr;
    JefeUsuariosView *m_jefeUsuarios = nullptr;

    // Bodega Views
    BodegaDashboardView *m_bodegaDashboard = nullptr;
    BodegaRegistroView *m_bodegaRegistro = nullptr;
    BodegaTrasladoView *m_bodegaTraslado = nullptr;
    BodegaInventarioView *m_bodegaInventario = nullptr;

    // Vendedor Views
    VendedorDashboardView *m_vendedorDashboard = nullptr;
    VendedorConsultaView *m_vendedorConsulta = nullptr;
    VendedorVentasView *m_vendedorVentas = nullptr;
    VendedorTrasladoView *m_vendedorTraslado = nullptr;
    VendedorDevolucionView *m_vendedorDevolucion = nullptr;
};
