#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include "Models.h"

class BodegaAutoresView : public QWidget {
    Q_OBJECT

public:
    explicit BodegaAutoresView(QWidget *parent = nullptr);
    void cargarAutores();

private slots:
    void onCrearAutor();
    void onEditarAutor();
    void onEliminarAutor();

private:
    QTableWidget *m_table;
    QPushButton *m_btnNuevo;
    QPushButton *m_btnEditar;
    QPushButton *m_btnEliminar;
    QPushButton *m_btnRecargar;
    QLabel *m_lblTotal;
    QList<AutorDto> m_autores;
};
