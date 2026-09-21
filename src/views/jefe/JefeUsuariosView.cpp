#include "JefeUsuariosView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>

JefeUsuariosView::JefeUsuariosView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    // Header
    auto *title = new QLabel("Administración y Gestión de Usuarios", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Toolbar
    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(12);

    m_btnNuevo = new QPushButton("＋ Registrar Nuevo Usuario", this);
    m_btnNuevo->setStyleSheet(Theme::buttonStyle(Theme::Accent, Theme::BgDark, Theme::AccentHover));
    toolbar->addWidget(m_btnNuevo);

    m_btnEditar = new QPushButton("✎ Modificar", this);
    m_btnEditar->setStyleSheet(Theme::secondaryButtonStyle());
    toolbar->addWidget(m_btnEditar);

    m_btnEliminar = new QPushButton("🗑 Dar de Baja", this);
    m_btnEliminar->setStyleSheet(Theme::dangerButtonStyle());
    toolbar->addWidget(m_btnEliminar);

    toolbar->addStretch();

    m_btnRecargar = new QPushButton("Actualizar", this);
    m_btnRecargar->setStyleSheet(Theme::secondaryButtonStyle());
    toolbar->addWidget(m_btnRecargar);

    layout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"ID Usuario", "Rol", "Nombre", "Ap. Paterno", "Ap. Materno", "Teléfono"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    connect(m_btnRecargar, &QPushButton::clicked, this, &JefeUsuariosView::cargarUsuarios);
    connect(m_btnNuevo, &QPushButton::clicked, this, &JefeUsuariosView::onCrearUsuario);
    connect(m_btnEditar, &QPushButton::clicked, this, &JefeUsuariosView::onEditarUsuario);
    connect(m_btnEliminar, &QPushButton::clicked, this, &JefeUsuariosView::onEliminarUsuario);
}

void JefeUsuariosView::cargarUsuarios() {
    ApiClient::instance()->listarUsuarios([this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isArray()) {
            Toast::showToast(this, "Error al obtener usuarios: " + msg, Toast::Error);
            return;
        }

        m_usuarios.clear();
        QJsonArray arr = data.toArray();
        m_table->setRowCount(arr.size());

        for (int i = 0; i < arr.size(); ++i) {
            UsuarioDto u = UsuarioDto::fromJson(arr.at(i).toObject());
            m_usuarios.append(u);

            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(u.id_usuarios)));

            auto *rolItem = new QTableWidgetItem(Theme::roleName(u.id_roles));
            rolItem->setForeground(QColor(Theme::roleColor(u.id_roles)));
            m_table->setItem(i, 1, rolItem);

            m_table->setItem(i, 2, new QTableWidgetItem(u.nombre));
            m_table->setItem(i, 3, new QTableWidgetItem(u.apellido_paterno));
            m_table->setItem(i, 4, new QTableWidgetItem(u.apellido_materno));
            m_table->setItem(i, 5, new QTableWidgetItem(u.telefono > 0 ? QString::number(u.telefono) : "-"));
        }
    });
}

void JefeUsuariosView::onCrearUsuario() {
    QDialog dlg(this);
    dlg.setWindowTitle("Registrar Nuevo Usuario");
    dlg.setFixedWidth(440);

    auto *dlgLayout = new QVBoxLayout(&dlg);
    auto *form = new QFormLayout();
    form->setSpacing(10);

    auto *txtId = new QLineEdit(&dlg);
    txtId->setPlaceholderText("Ej. 350976899");

    auto *comboRol = new QComboBox(&dlg);
    comboRol->addItem("Jefe de Departamento", 1);
    comboRol->addItem("Personal de Bodega", 2);
    comboRol->addItem("Vendedor", 3);
    comboRol->addItem("Gerente", 4);

    auto *txtNombre = new QLineEdit(&dlg);
    auto *txtApPaterno = new QLineEdit(&dlg);
    auto *txtApMaterno = new QLineEdit(&dlg);
    auto *txtTelefono = new QLineEdit(&dlg);
    txtTelefono->setPlaceholderText("10 dígitos numéricos");
    auto *txtPass = new QLineEdit(&dlg);
    txtPass->setEchoMode(QLineEdit::Password);

    form->addRow("ID Usuario (Numérico):", txtId);
    form->addRow("Rol en el Sistema:", comboRol);
    form->addRow("Nombre:", txtNombre);
    form->addRow("Apellido Paterno:", txtApPaterno);
    form->addRow("Apellido Materno:", txtApMaterno);
    form->addRow("Teléfono:", txtTelefono);
    form->addRow("Contraseña:", txtPass);
    dlgLayout->addLayout(form);

    auto *btnBox = new QHBoxLayout();
    auto *btnGuardar = new QPushButton("Guardar Usuario", &dlg);
    btnGuardar->setStyleSheet(Theme::buttonStyle(Theme::Success, Theme::BgDark, "#72D4A0"));
    auto *btnCancelar = new QPushButton("Cancelar", &dlg);
    btnCancelar->setStyleSheet(Theme::secondaryButtonStyle());
    btnBox->addWidget(btnGuardar);
    btnBox->addWidget(btnCancelar);
    dlgLayout->addLayout(btnBox);

    connect(btnCancelar, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnGuardar, &QPushButton::clicked, [&]() {
        bool ok = false;
        qint64 id = txtId->text().trimmed().toLongLong(&ok);
        if (!ok || id <= 0) {
            Toast::showToast(&dlg, "El ID de usuario debe ser numérico y válido", Toast::Warning);
            return;
        }
        if (txtNombre->text().trimmed().isEmpty() || txtPass->text().isEmpty()) {
            Toast::showToast(&dlg, "Nombre y contraseña son obligatorios", Toast::Warning);
            return;
        }

        QJsonObject obj;
        obj["id_usuarios"] = id;
        obj["id_roles"] = comboRol->currentData().toInt();
        obj["nombre"] = txtNombre->text().trimmed();
        if (!txtApPaterno->text().trimmed().isEmpty()) obj["apellido_paterno"] = txtApPaterno->text().trimmed();
        if (!txtApMaterno->text().trimmed().isEmpty()) obj["apellido_materno"] = txtApMaterno->text().trimmed();
        if (!txtTelefono->text().trimmed().isEmpty()) obj["telefono"] = txtTelefono->text().trimmed().toLongLong();
        obj["contrasena"] = txtPass->text();

        ApiClient::instance()->crearUsuario(obj, [this, &dlg](bool success, const QJsonValue &, const QString &msg) {
            if (success) {
                Toast::showToast(this, "Usuario creado exitosamente", Toast::Success);
                dlg.accept();
                cargarUsuarios();
            } else {
                Toast::showToast(&dlg, "Error al crear: " + msg, Toast::Error);
            }
        });
    });

    dlg.exec();
}

void JefeUsuariosView::onEditarUsuario() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_usuarios.size()) {
        Toast::showToast(this, "Selecciona un usuario para modificar", Toast::Warning);
        return;
    }

    const UsuarioDto &u = m_usuarios.at(row);

    QDialog dlg(this);
    dlg.setWindowTitle(QString("Modificar Usuario #%1").arg(u.id_usuarios));
    dlg.setFixedWidth(440);

    auto *dlgLayout = new QVBoxLayout(&dlg);
    auto *form = new QFormLayout();
    form->setSpacing(10);

    auto *comboRol = new QComboBox(&dlg);
    comboRol->addItem("Jefe de Departamento", 1);
    comboRol->addItem("Personal de Bodega", 2);
    comboRol->addItem("Vendedor", 3);
    comboRol->addItem("Gerente", 4);
    int idx = comboRol->findData(u.id_roles);
    if (idx >= 0) comboRol->setCurrentIndex(idx);

    auto *txtNombre = new QLineEdit(u.nombre, &dlg);
    auto *txtApPaterno = new QLineEdit(u.apellido_paterno, &dlg);
    auto *txtApMaterno = new QLineEdit(u.apellido_materno, &dlg);
    auto *txtTelefono = new QLineEdit(u.telefono > 0 ? QString::number(u.telefono) : "", &dlg);
    auto *txtPass = new QLineEdit(&dlg);
    txtPass->setPlaceholderText("Dejar vacío para mantener contraseña actual");
    txtPass->setEchoMode(QLineEdit::Password);

    form->addRow("Rol en el Sistema:", comboRol);
    form->addRow("Nombre:", txtNombre);
    form->addRow("Apellido Paterno:", txtApPaterno);
    form->addRow("Apellido Materno:", txtApMaterno);
    form->addRow("Teléfono:", txtTelefono);
    form->addRow("Nueva Contraseña:", txtPass);
    dlgLayout->addLayout(form);

    auto *btnBox = new QHBoxLayout();
    auto *btnGuardar = new QPushButton("Actualizar Datos", &dlg);
    btnGuardar->setStyleSheet(Theme::buttonStyle());
    auto *btnCancelar = new QPushButton("Cancelar", &dlg);
    btnCancelar->setStyleSheet(Theme::secondaryButtonStyle());
    btnBox->addWidget(btnGuardar);
    btnBox->addWidget(btnCancelar);
    dlgLayout->addLayout(btnBox);

    connect(btnCancelar, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnGuardar, &QPushButton::clicked, [&]() {
        QJsonObject obj;
        obj["id_roles"] = comboRol->currentData().toInt();
        obj["nombre"] = txtNombre->text().trimmed();
        obj["apellido_paterno"] = txtApPaterno->text().trimmed();
        obj["apellido_materno"] = txtApMaterno->text().trimmed();
        if (!txtTelefono->text().trimmed().isEmpty()) {
            obj["telefono"] = txtTelefono->text().trimmed().toLongLong();
        }
        if (!txtPass->text().isEmpty()) {
            obj["contrasena"] = txtPass->text();
        }

        ApiClient::instance()->actualizarUsuario(u.id_usuarios, obj, [this, &dlg](bool success, const QJsonValue &, const QString &msg) {
            if (success) {
                Toast::showToast(this, "Usuario actualizado correctamente", Toast::Success);
                dlg.accept();
                cargarUsuarios();
            } else {
                Toast::showToast(&dlg, "Error al actualizar: " + msg, Toast::Error);
            }
        });
    });

    dlg.exec();
}

void JefeUsuariosView::onEliminarUsuario() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_usuarios.size()) {
        Toast::showToast(this, "Selecciona un usuario para dar de baja", Toast::Warning);
        return;
    }

    const UsuarioDto &u = m_usuarios.at(row);

    if (QMessageBox::question(this, "Confirmar Baja",
        QString("¿Estás seguro de que deseas dar de baja al usuario %1 (%2)?").arg(u.nombreCompleto()).arg(u.id_usuarios),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    ApiClient::instance()->eliminarUsuario(u.id_usuarios, [this](bool ok, const QJsonValue &, const QString &msg) {
        if (ok) {
            Toast::showToast(this, "Usuario dado de baja exitosamente", Toast::Success);
            cargarUsuarios();
        } else {
            Toast::showToast(this, "Error al dar de baja: " + msg, Toast::Error);
        }
    });
}
