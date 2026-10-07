#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include "Models.h"

struct CompraCartItem {
    qint64 ean = 0;
    QString tipo;
    QString nombre;
    int cantidad = 0;
    double costo_unitario = 0.0;
    double subtotal = 0.0;
};

class BodegaComprasView : public QWidget {
    Q_OBJECT

public:
    explicit BodegaComprasView(QWidget *parent = nullptr);
    void cargarProveedores();

private slots:
    void onEanChanged(const QString &text);
    void onAgregarArticulo();
    void onQuitarFila(int row);
    void onRegistrarCompra();
    void limpiarCarrito();

private:
    void recalcularTotales();

    QComboBox *m_comboProveedor;
    QLineEdit *m_txtEan;
    QLabel *m_lblEanStatus;
    QSpinBox *m_spnCantidad;
    QDoubleSpinBox *m_spnCosto;
    QPushButton *m_btnAgregar;

    QTableWidget *m_tableCart;
    QLineEdit *m_txtObs;
    QLabel *m_lblTotalArticulos;
    QLabel *m_lblTotalPagar;
    QPushButton *m_btnVaciar;
    QPushButton *m_btnProcesar;

    QList<CompraCartItem> m_items;
    QList<ProveedorDto> m_proveedores;
};
