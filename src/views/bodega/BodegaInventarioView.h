#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class BodegaInventarioView : public QWidget {
    Q_OBJECT

public:
    explicit BodegaInventarioView(QWidget *parent = nullptr);
    void cargarInventario();

private:
    QLineEdit *m_txtBuscar;
    QComboBox *m_comboTipo;
    QPushButton *m_btnBuscar;
    QPushButton *m_btnRecargar;
    QTableWidget *m_table;
};
