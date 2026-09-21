#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class VendedorConsultaView : public QWidget {
    Q_OBJECT

public:
    explicit VendedorConsultaView(QWidget *parent = nullptr);
    void buscarExistencias();

private:
    QLineEdit *m_txtBuscar;
    QComboBox *m_comboTipo;
    QComboBox *m_comboUbicacion;
    QPushButton *m_btnBuscar;
    QPushButton *m_btnLimpiar;
    QTableWidget *m_table;
};
