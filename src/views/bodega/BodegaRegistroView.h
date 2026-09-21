#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>

class BodegaRegistroView : public QWidget {
    Q_OBJECT

public:
    explicit BodegaRegistroView(QWidget *parent = nullptr);

private slots:
    void onTipoChanged(int index);
    void onRegistrar();
    void limpiarCampos();

private:
    QComboBox *m_comboTipo;
    QLineEdit *m_txtEan;
    QLineEdit *m_txtSku;
    QLineEdit *m_txtNombre;
    QDoubleSpinBox *m_spnPrecio;
    QSpinBox *m_spnCantidad;

    // Campos de Libro
    QWidget *m_panelLibro;
    QComboBox *m_comboGenero;
    QLineEdit *m_txtAutor;

    // Campos de Revista
    QWidget *m_panelRevista;
    QSpinBox *m_spnEdicion;
    QLineEdit *m_txtPeriodicidad;

    QComboBox *m_comboMueble;
    QComboBox *m_comboProveedor;
    QPushButton *m_btnGuardar;
    QPushButton *m_btnLimpiar;
};
