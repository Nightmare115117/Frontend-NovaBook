#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "Models.h"

class JefeDevolucionesView : public QWidget {
    Q_OBJECT

public:
    explicit JefeDevolucionesView(QWidget *parent = nullptr);
    void cargarDevoluciones();

private slots:
    void onEvaluarDevolucion();

private:
    QComboBox *m_comboEstado;
    QPushButton *m_btnRecargar;
    QPushButton *m_btnEvaluar;
    QTableWidget *m_table;
    QList<Devolucion> m_devoluciones;
};
