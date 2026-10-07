#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include "Models.h"

class BodegaProveedoresView : public QWidget {
    Q_OBJECT

public:
    explicit BodegaProveedoresView(QWidget *parent = nullptr);
    void cargarProveedores();

private slots:
    void onCrearProveedor();
    void onEditarProveedor();
    void onEliminarProveedor();

private:
    QTableWidget *m_table;
    QPushButton *m_btnNuevo;
    QPushButton *m_btnEditar;
    QPushButton *m_btnEliminar;
    QPushButton *m_btnRecargar;
    QLabel *m_lblTotal;
    QList<ProveedorDto> m_proveedores;
};
