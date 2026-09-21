#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "Models.h"

class JefeUsuariosView : public QWidget {
    Q_OBJECT

public:
    explicit JefeUsuariosView(QWidget *parent = nullptr);
    void cargarUsuarios();

private slots:
    void onCrearUsuario();
    void onEditarUsuario();
    void onEliminarUsuario();

private:
    QPushButton *m_btnNuevo;
    QPushButton *m_btnEditar;
    QPushButton *m_btnEliminar;
    QPushButton *m_btnRecargar;
    QTableWidget *m_table;
    QList<UsuarioDto> m_usuarios;
};
