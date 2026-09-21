#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "Models.h"

struct CartItem {
    qint64 ean = 0;
    QString tipo = "libro";
    QString nombre;
    int cantidad = 1;
    double precio = 0.0;
    double subtotal = 0.0;
};

class VendedorVentasView : public QWidget {
    Q_OBJECT

public:
    explicit VendedorVentasView(QWidget *parent = nullptr);

private slots:
    void onAgregarAlCarrito();
    void onQuitarFila(int row);
    void onProcesarVenta();
    void limpiarCarrito();

private:
    void recalcularTotales();

    QLineEdit *m_txtEan;
    QComboBox *m_comboTipo;
    QSpinBox *m_spnCantidad;
    QPushButton *m_btnAgregar;

    QTableWidget *m_tableCart;
    QLabel *m_lblTotalArticulos;
    QLabel *m_lblTotalPagar;
    QPushButton *m_btnProcesar;
    QPushButton *m_btnVaciar;

    QList<CartItem> m_cart;
};
