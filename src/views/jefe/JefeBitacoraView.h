#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>

class JefeBitacoraView : public QWidget {
    Q_OBJECT

public:
    explicit JefeBitacoraView(QWidget *parent = nullptr);
    void cargarMovimientos();

private:
    QDateEdit *m_dateEdit;
    QPushButton *m_btnBuscar;
    QPushButton *m_btnHoy;
    QTableWidget *m_table;
    QLabel *m_lblTotal;
};
