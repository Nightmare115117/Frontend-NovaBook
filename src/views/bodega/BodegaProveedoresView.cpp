#include "BodegaProveedoresView.h"
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

BodegaProveedoresView::BodegaProveedoresView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    // Header
    auto *title = new QLabel("Catálogo y Gestión de Proveedores", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Toolbar
    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(12);

    m_btnNuevo = new QPushButton("＋ Registrar Proveedor", this);
    m_btnNuevo->setStyleSheet(Theme::buttonStyle(Theme::Role2, Theme::BgDark, "#7BB9D9"));
    toolbar->addWidget(m_btnNuevo);

    m_btnEditar = new QPushButton("✎ Modificar", this);
    m_btnEditar->setStyleSheet(Theme::secondaryButtonStyle());
    toolbar->addWidget(m_btnEditar);

    m_btnEliminar = new QPushButton("🗑 Dar de Baja", this);
    m_btnEliminar->setStyleSheet(Theme::dangerButtonStyle());
    toolbar->addWidget(m_btnEliminar);

    toolbar->addStretch();

    m_lblTotal = new QLabel("Total de proveedores: 0", this);
    m_lblTotal->setStyleSheet(QString("color: %1; font-weight: bold;").arg(Theme::Accent));
    toolbar->addWidget(m_lblTotal);

    m_btnRecargar = new QPushButton("Actualizar", this);
    m_btnRecargar->setStyleSheet(Theme::secondaryButtonStyle());
    toolbar->addWidget(m_btnRecargar);

    layout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"ID", "Razón Social / Nombre", "RFC", "Teléfono", "Correo", "Contacto", "Estatus"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    connect(m_btnRecargar, &QPushButton::clicked, this, &BodegaProveedoresView::cargarProveedores);
    connect(m_btnNuevo, &QPushButton::clicked, this, &BodegaProveedoresView::onCrearProveedor);
    connect(m_btnEditar, &QPushButton::clicked, this, &BodegaProveedoresView::onEditarProveedor);
    connect(m_btnEliminar, &QPushButton::clicked, this, &BodegaProveedoresView::onEliminarProveedor);

    cargarProveedores();
}

void BodegaProveedoresView::cargarProveedores() {
    ApiClient::instance()->listarProveedores([this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isArray()) {
            Toast::showToast(this, "Error al cargar proveedores: " + msg, Toast::Error);
            return;
        }

        m_proveedores.clear();
        QJsonArray arr = data.toArray();
        m_table->setRowCount(arr.size());
        m_lblTotal->setText(QString("Total de proveedores: %1").arg(arr.size()));

        for (int i = 0; i < arr.size(); ++i) {
            ProveedorDto p = ProveedorDto::fromJson(arr.at(i).toObject());
            m_proveedores.append(p);

            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id_proveedor)));
            m_table->setItem(i, 1, new QTableWidgetItem(p.nombre_proveedor));
            m_table->setItem(i, 2, new QTableWidgetItem(p.rfc));
            m_table->setItem(i, 3, new QTableWidgetItem(p.telefono.isEmpty() ? "-" : p.telefono));
            m_table->setItem(i, 4, new QTableWidgetItem(p.correo.isEmpty() ? "-" : p.correo));
            m_table->setItem(i, 5, new QTableWidgetItem(p.persona_contacto.isEmpty() ? "-" : p.persona_contacto));

            auto *statusItem = new QTableWidgetItem(p.estatus);
            if (p.estatus.toUpper() == "ACTIVO") {
                statusItem->setForeground(QColor(Theme::Success));
            } else {
                statusItem->setForeground(QColor(Theme::TextMuted));
            }
            m_table->setItem(i, 6, statusItem);
        }
    });
}

void BodegaProveedoresView::onCrearProveedor() {
    QDialog dlg(this);
    dlg.setWindowTitle("Registrar Nuevo Proveedor");
    dlg.setFixedWidth(460);

    auto *form = new QFormLayout(&dlg);
    form->setSpacing(12);

    auto *txtNombre = new QLineEdit(&dlg);
    txtNombre->setPlaceholderText("Razón social o nombre comercial");
    form->addRow("Razón Social:", txtNombre);

    auto *txtRfc = new QLineEdit(&dlg);
    txtRfc->setPlaceholderText("Ej. PME850101AB1");
    form->addRow("RFC / Id Fiscal:", txtRfc);

    auto *txtTelefono = new QLineEdit(&dlg);
    txtTelefono->setPlaceholderText("Teléfono de contacto");
    form->addRow("Teléfono:", txtTelefono);

    auto *txtCorreo = new QLineEdit(&dlg);
    txtCorreo->setPlaceholderText("contacto@editorial.com");
    form->addRow("Correo:", txtCorreo);

    auto *txtDireccion = new QLineEdit(&dlg);
    txtDireccion->setPlaceholderText("Calle, número, colonia, ciudad");
    form->addRow("Dirección:", txtDireccion);

    auto *txtContacto = new QLineEdit(&dlg);
    txtContacto->setPlaceholderText("Nombre del representante o contacto");
    form->addRow("Persona de Contacto:", txtContacto);

    auto *comboEstatus = new QComboBox(&dlg);
    comboEstatus->addItem("ACTIVO", "ACTIVO");
    comboEstatus->addItem("INACTIVO", "INACTIVO");
    form->addRow("Estatus:", comboEstatus);

    auto *btnBox = new QHBoxLayout();
    auto *btnGuardar = new QPushButton("✔ Guardar Proveedor", &dlg);
    btnGuardar->setStyleSheet(Theme::buttonStyle(Theme::Role2, Theme::BgDark, "#7BB9D9"));
    auto *btnCancelar = new QPushButton("Cancelar", &dlg);
    btnCancelar->setStyleSheet(Theme::secondaryButtonStyle());

    btnBox->addStretch();
    btnBox->addWidget(btnCancelar);
    btnBox->addWidget(btnGuardar);
    form->addRow(btnBox);

    connect(btnCancelar, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnGuardar, &QPushButton::clicked, [&]() {
        QString nom = txtNombre->text().trimmed();
        QString rfc = txtRfc->text().trimmed();
        if (nom.isEmpty()) {
            Toast::showToast(&dlg, "La razón social del proveedor es obligatoria", Toast::Warning);
            return;
        }
        if (rfc.isEmpty()) {
            Toast::showToast(&dlg, "El RFC del proveedor es obligatorio", Toast::Warning);
            return;
        }

        QJsonObject req;
        req["nombre_proveedor"] = nom;
        req["rfc"] = rfc;
        if (!txtTelefono->text().trimmed().isEmpty()) req["telefono"] = txtTelefono->text().trimmed();
        if (!txtCorreo->text().trimmed().isEmpty()) req["correo"] = txtCorreo->text().trimmed();
        if (!txtDireccion->text().trimmed().isEmpty()) req["direccion"] = txtDireccion->text().trimmed();
        if (!txtContacto->text().trimmed().isEmpty()) req["persona_contacto"] = txtContacto->text().trimmed();
        req["estatus"] = comboEstatus->currentData().toString();

        ApiClient::instance()->crearProveedor(req, [this, &dlg](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Proveedor registrado exitosamente", Toast::Success);
                dlg.accept();
                cargarProveedores();
            } else {
                Toast::showToast(&dlg, "Error al registrar proveedor: " + msg, Toast::Error);
            }
        });
    });

    dlg.exec();
}

void BodegaProveedoresView::onEditarProveedor() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_proveedores.size()) {
        Toast::showToast(this, "Selecciona un proveedor de la lista", Toast::Warning);
        return;
    }

    const ProveedorDto &p = m_proveedores.at(row);

    QDialog dlg(this);
    dlg.setWindowTitle(QString("Editar Proveedor — %1").arg(p.nombre_proveedor));
    dlg.setFixedWidth(460);

    auto *form = new QFormLayout(&dlg);
    form->setSpacing(12);

    auto *txtNombre = new QLineEdit(p.nombre_proveedor, &dlg);
    form->addRow("Razón Social:", txtNombre);

    auto *txtRfc = new QLineEdit(p.rfc, &dlg);
    form->addRow("RFC / Id Fiscal:", txtRfc);

    auto *txtTelefono = new QLineEdit(p.telefono, &dlg);
    form->addRow("Teléfono:", txtTelefono);

    auto *txtCorreo = new QLineEdit(p.correo, &dlg);
    form->addRow("Correo:", txtCorreo);

    auto *txtDireccion = new QLineEdit(p.direccion, &dlg);
    form->addRow("Dirección:", txtDireccion);

    auto *txtContacto = new QLineEdit(p.persona_contacto, &dlg);
    form->addRow("Persona de Contacto:", txtContacto);

    auto *comboEstatus = new QComboBox(&dlg);
    comboEstatus->addItem("ACTIVO", "ACTIVO");
    comboEstatus->addItem("INACTIVO", "INACTIVO");
    comboEstatus->setCurrentText(p.estatus);
    form->addRow("Estatus:", comboEstatus);

    auto *btnBox = new QHBoxLayout();
    auto *btnGuardar = new QPushButton("✔ Actualizar", &dlg);
    btnGuardar->setStyleSheet(Theme::buttonStyle(Theme::Role2, Theme::BgDark, "#7BB9D9"));
    auto *btnCancelar = new QPushButton("Cancelar", &dlg);
    btnCancelar->setStyleSheet(Theme::secondaryButtonStyle());

    btnBox->addStretch();
    btnBox->addWidget(btnCancelar);
    btnBox->addWidget(btnGuardar);
    form->addRow(btnBox);

    connect(btnCancelar, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnGuardar, &QPushButton::clicked, [&]() {
        QString nom = txtNombre->text().trimmed();
        QString rfc = txtRfc->text().trimmed();
        if (nom.isEmpty()) {
            Toast::showToast(&dlg, "La razón social del proveedor es obligatoria", Toast::Warning);
            return;
        }
        if (rfc.isEmpty()) {
            Toast::showToast(&dlg, "El RFC del proveedor es obligatorio", Toast::Warning);
            return;
        }

        QJsonObject req;
        req["nombre_proveedor"] = nom;
        req["rfc"] = rfc;
        req["telefono"] = txtTelefono->text().trimmed();
        req["correo"] = txtCorreo->text().trimmed();
        req["direccion"] = txtDireccion->text().trimmed();
        req["persona_contacto"] = txtContacto->text().trimmed();
        req["estatus"] = comboEstatus->currentData().toString();

        ApiClient::instance()->actualizarProveedor(p.id_proveedor, req, [this, &dlg](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Proveedor actualizado exitosamente", Toast::Success);
                dlg.accept();
                cargarProveedores();
            } else {
                Toast::showToast(&dlg, "Error al actualizar proveedor: " + msg, Toast::Error);
            }
        });
    });

    dlg.exec();
}

void BodegaProveedoresView::onEliminarProveedor() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_proveedores.size()) {
        Toast::showToast(this, "Selecciona un proveedor para dar de baja", Toast::Warning);
        return;
    }

    const ProveedorDto &p = m_proveedores.at(row);

    auto reply = QMessageBox::question(
        this,
        "Confirmar Baja de Proveedor",
        QString("¿Deseas dar de baja al proveedor '%1'?\n\nSi tiene mercancía, compras o devoluciones asociadas, su estatus cambiará a 'INACTIVO' para proteger el histórico.").arg(p.nombre_proveedor),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        ApiClient::instance()->eliminarProveedor(p.id_proveedor, [this, p](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, QString("Proveedor '%1' procesado para baja").arg(p.nombre_proveedor), Toast::Success);
                cargarProveedores();
            } else {
                Toast::showToast(this, "Error al procesar baja del proveedor: " + msg, Toast::Error);
            }
        });
    }
}
