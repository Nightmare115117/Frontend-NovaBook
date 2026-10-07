#include "BodegaAutoresView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>

BodegaAutoresView::BodegaAutoresView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    // Header
    auto *title = new QLabel("Catálogo y Gestión de Autores", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Toolbar
    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(12);

    m_btnNuevo = new QPushButton("＋ Registrar Autor", this);
    m_btnNuevo->setStyleSheet(Theme::buttonStyle(Theme::Role2, Theme::BgDark, "#7BB9D9"));
    toolbar->addWidget(m_btnNuevo);

    m_btnEditar = new QPushButton("✎ Modificar", this);
    m_btnEditar->setStyleSheet(Theme::secondaryButtonStyle());
    toolbar->addWidget(m_btnEditar);

    m_btnEliminar = new QPushButton("🗑 Eliminar", this);
    m_btnEliminar->setStyleSheet(Theme::dangerButtonStyle());
    toolbar->addWidget(m_btnEliminar);

    toolbar->addStretch();

    m_lblTotal = new QLabel("Total de autores: 0", this);
    m_lblTotal->setStyleSheet(QString("color: %1; font-weight: bold;").arg(Theme::Accent));
    toolbar->addWidget(m_lblTotal);

    m_btnRecargar = new QPushButton("Actualizar", this);
    m_btnRecargar->setStyleSheet(Theme::secondaryButtonStyle());
    toolbar->addWidget(m_btnRecargar);

    layout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"ID", "Nombre", "Apellidos", "Nacionalidad", "Biografía"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    connect(m_btnRecargar, &QPushButton::clicked, this, &BodegaAutoresView::cargarAutores);
    connect(m_btnNuevo, &QPushButton::clicked, this, &BodegaAutoresView::onCrearAutor);
    connect(m_btnEditar, &QPushButton::clicked, this, &BodegaAutoresView::onEditarAutor);
    connect(m_btnEliminar, &QPushButton::clicked, this, &BodegaAutoresView::onEliminarAutor);

    cargarAutores();
}

void BodegaAutoresView::cargarAutores() {
    ApiClient::instance()->listarAutores([this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isArray()) {
            Toast::showToast(this, "Error al cargar autores: " + msg, Toast::Error);
            return;
        }

        m_autores.clear();
        QJsonArray arr = data.toArray();
        m_table->setRowCount(arr.size());
        m_lblTotal->setText(QString("Total de autores: %1").arg(arr.size()));

        for (int i = 0; i < arr.size(); ++i) {
            AutorDto a = AutorDto::fromJson(arr.at(i).toObject());
            m_autores.append(a);

            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(a.id_autor)));
            m_table->setItem(i, 1, new QTableWidgetItem(a.nombre));
            m_table->setItem(i, 2, new QTableWidgetItem(a.apellidos));
            m_table->setItem(i, 3, new QTableWidgetItem(a.nacionalidad.isEmpty() ? "-" : a.nacionalidad));
            m_table->setItem(i, 4, new QTableWidgetItem(a.biografia.isEmpty() ? "-" : a.biografia));
        }
    });
}

void BodegaAutoresView::onCrearAutor() {
    QDialog dlg(this);
    dlg.setWindowTitle("Registrar Nuevo Autor");
    dlg.setFixedWidth(440);

    auto *form = new QFormLayout(&dlg);
    form->setSpacing(12);

    auto *txtNombre = new QLineEdit(&dlg);
    txtNombre->setPlaceholderText("Nombre de pila");
    form->addRow("Nombre:", txtNombre);

    auto *txtApellidos = new QLineEdit(&dlg);
    txtApellidos->setPlaceholderText("Apellidos del autor");
    form->addRow("Apellidos:", txtApellidos);

    auto *txtNacionalidad = new QLineEdit(&dlg);
    txtNacionalidad->setPlaceholderText("Ej. Mexicana, Colombiana, Española...");
    form->addRow("Nacionalidad:", txtNacionalidad);

    auto *txtBio = new QTextEdit(&dlg);
    txtBio->setPlaceholderText("Breve reseña biográfica...");
    txtBio->setMaximumHeight(80);
    form->addRow("Biografía:", txtBio);

    auto *btnBox = new QHBoxLayout();
    auto *btnGuardar = new QPushButton("✔ Guardar Autor", &dlg);
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
        QString ape = txtApellidos->text().trimmed();
        if (nom.isEmpty() || ape.isEmpty()) {
            Toast::showToast(&dlg, "Nombre y apellidos son obligatorios", Toast::Warning);
            return;
        }

        QJsonObject req;
        req["nombre"] = nom;
        req["apellidos"] = ape;
        if (!txtNacionalidad->text().trimmed().isEmpty()) req["nacionalidad"] = txtNacionalidad->text().trimmed();
        if (!txtBio->toPlainText().trimmed().isEmpty()) req["biografia"] = txtBio->toPlainText().trimmed();

        ApiClient::instance()->crearAutor(req, [this, &dlg](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Autor registrado exitosamente", Toast::Success);
                dlg.accept();
                cargarAutores();
            } else {
                Toast::showToast(&dlg, "Error al registrar autor: " + msg, Toast::Error);
            }
        });
    });

    dlg.exec();
}

void BodegaAutoresView::onEditarAutor() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_autores.size()) {
        Toast::showToast(this, "Selecciona un autor de la lista", Toast::Warning);
        return;
    }

    const AutorDto &a = m_autores.at(row);

    QDialog dlg(this);
    dlg.setWindowTitle(QString("Editar Autor — %1").arg(a.nombreCompleto()));
    dlg.setFixedWidth(440);

    auto *form = new QFormLayout(&dlg);
    form->setSpacing(12);

    auto *txtNombre = new QLineEdit(a.nombre, &dlg);
    form->addRow("Nombre:", txtNombre);

    auto *txtApellidos = new QLineEdit(a.apellidos, &dlg);
    form->addRow("Apellidos:", txtApellidos);

    auto *txtNacionalidad = new QLineEdit(a.nacionalidad, &dlg);
    form->addRow("Nacionalidad:", txtNacionalidad);

    auto *txtBio = new QTextEdit(&dlg);
    txtBio->setText(a.biografia);
    txtBio->setMaximumHeight(80);
    form->addRow("Biografía:", txtBio);

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
        QString ape = txtApellidos->text().trimmed();
        if (nom.isEmpty() || ape.isEmpty()) {
            Toast::showToast(&dlg, "Nombre y apellidos son obligatorios", Toast::Warning);
            return;
        }

        QJsonObject req;
        req["nombre"] = nom;
        req["apellidos"] = ape;
        req["nacionalidad"] = txtNacionalidad->text().trimmed();
        req["biografia"] = txtBio->toPlainText().trimmed();

        ApiClient::instance()->actualizarAutor(a.id_autor, req, [this, &dlg](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Autor actualizado exitosamente", Toast::Success);
                dlg.accept();
                cargarAutores();
            } else {
                Toast::showToast(&dlg, "Error al actualizar autor: " + msg, Toast::Error);
            }
        });
    });

    dlg.exec();
}

void BodegaAutoresView::onEliminarAutor() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_autores.size()) {
        Toast::showToast(this, "Selecciona un autor para eliminar", Toast::Warning);
        return;
    }

    const AutorDto &a = m_autores.at(row);

    auto reply = QMessageBox::question(
        this,
        "Confirmar Eliminación de Autor",
        QString("¿Estás seguro de eliminar al autor '%1'?\n\nNota: La operación fallará si el autor tiene libros asociados en el catálogo.").arg(a.nombreCompleto()),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        ApiClient::instance()->eliminarAutor(a.id_autor, [this, a](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, QString("Autor '%1' eliminado exitosamente").arg(a.nombreCompleto()), Toast::Success);
                cargarAutores();
            } else {
                Toast::showToast(this, "No se pudo eliminar el autor: " + msg, Toast::Error);
            }
        });
    }
}
