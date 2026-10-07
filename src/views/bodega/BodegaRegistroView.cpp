#include "BodegaRegistroView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include "EanClassifier.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFrame>

BodegaRegistroView::BodegaRegistroView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    auto *title = new QLabel("Alta de Mercancía en Almacén (Ingreso por EAN)", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Form Container
    auto *card = new QFrame(this);
    card->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(14);

    auto *form = new QFormLayout();
    form->setSpacing(12);

    // 1. Unified EAN Input with Automatic Detection
    auto *eanContainer = new QWidget(card);
    auto *eanLayout = new QVBoxLayout(eanContainer);
    eanLayout->setContentsMargins(0, 0, 0, 0);
    eanLayout->setSpacing(4);

    m_txtEan = new QLineEdit(eanContainer);
    m_txtEan->setPlaceholderText("Escanear o ingresar EAN-13 (13 dígitos)...");
    eanLayout->addWidget(m_txtEan);

    m_lblEanStatus = new QLabel("Ingresa el código EAN-13 para clasificar automáticamente como Libro o Revista", eanContainer);
    m_lblEanStatus->setStyleSheet(QString("font-size: 12px; color: %1; font-weight: 500;").arg(Theme::TextMuted));
    eanLayout->addWidget(m_lblEanStatus);

    form->addRow("Código EAN-13:", eanContainer);

    m_txtSku = new QLineEdit(card);
    m_txtSku->setPlaceholderText("SKU numérico interno (opcional)");
    form->addRow("SKU Interno:", m_txtSku);

    m_txtNombre = new QLineEdit(card);
    m_txtNombre->setPlaceholderText("Título del libro o nombre de la revista");
    form->addRow("Título / Nombre:", m_txtNombre);

    m_spnPrecio = new QDoubleSpinBox(card);
    m_spnPrecio->setRange(0.0, 99999.0);
    m_spnPrecio->setValue(199.0);
    m_spnPrecio->setPrefix("$ ");
    form->addRow("Precio Unitario:", m_spnPrecio);

    m_spnCantidad = new QSpinBox(card);
    m_spnCantidad->setRange(1, 9999);
    m_spnCantidad->setValue(10);
    form->addRow("Cantidad Recibida:", m_spnCantidad);

    // 2. Dynamic fields for Libro (ISBN 978 / 979)
    m_panelLibro = new QWidget(card);
    auto *libroLayout = new QFormLayout(m_panelLibro);
    libroLayout->setContentsMargins(0, 0, 0, 0);
    libroLayout->setSpacing(12);

    m_comboAutor = new QComboBox(m_panelLibro);
    libroLayout->addRow("Autor Registrado:", m_comboAutor);

    form->addRow(m_panelLibro);
    m_panelLibro->hide();

    // 3. Dynamic fields for Revista (ISSN 977)
    m_panelRevista = new QWidget(card);
    auto *revistaLayout = new QFormLayout(m_panelRevista);
    revistaLayout->setContentsMargins(0, 0, 0, 0);
    revistaLayout->setSpacing(12);

    m_comboAutorEditorial = new QComboBox(m_panelRevista);
    m_comboAutorEditorial->setPlaceholderText("Selecciona un autor o editorial");
    revistaLayout->addRow("Autor / Editorial:", m_comboAutorEditorial);

    m_spnEdicion = new QSpinBox(m_panelRevista);
    m_spnEdicion->setRange(1, 9999);
    m_spnEdicion->setValue(1);
    revistaLayout->addRow("Número de Edición:", m_spnEdicion);

    m_comboPeriodicidad = new QComboBox(m_panelRevista);
    m_comboPeriodicidad->addItem("Selecciona una periodicidad", "");
    m_comboPeriodicidad->addItem("Mensual", "Mensual");
    m_comboPeriodicidad->addItem("Semanal", "Semanal");
    m_comboPeriodicidad->addItem("Quincenal", "Quincenal");
    revistaLayout->addRow("Periodicidad:", m_comboPeriodicidad);

    form->addRow(m_panelRevista);
    m_panelRevista->hide();

    // 4. Multiple Genres Selection (Relación Muchos a Muchos N:M)
    m_listGeneros = new QListWidget(card);
    m_listGeneros->setMaximumHeight(100);
    m_listGeneros->setStyleSheet(QString(R"(
        QListWidget {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 4px;
            color: %3;
        }
        QListWidget::item {
            padding: 3px 6px;
        }
    )").arg(Theme::BgInput).arg(Theme::Border).arg(Theme::TextPrimary));
    form->addRow("Géneros Literarios (N:M):", m_listGeneros);

    // 5. Mueble y Proveedor
    m_comboMueble = new QComboBox(card);
    m_comboMueble->addItem("Mueble 11", 11);
    m_comboMueble->addItem("Mueble 12", 12);
    m_comboMueble->addItem("Mueble 13", 13);
    m_comboMueble->addItem("Mueble 61", 61);
    m_comboMueble->addItem("Mueble 62", 62);
    form->addRow("Mueble de Asignación:", m_comboMueble);

    m_comboProveedor = new QComboBox(card);
    form->addRow("Proveedor Oficial:", m_comboProveedor);

    cardLayout->addLayout(form);

    // Buttons
    auto *btnLayout = new QHBoxLayout();
    m_btnGuardar = new QPushButton("✔ Registrar en Bodega", card);
    m_btnGuardar->setStyleSheet(Theme::buttonStyle(Theme::Role2, Theme::BgDark, "#7BB9D9"));

    m_btnLimpiar = new QPushButton("Limpiar", card);
    m_btnLimpiar->setStyleSheet(Theme::secondaryButtonStyle());

    btnLayout->addWidget(m_btnGuardar);
    btnLayout->addWidget(m_btnLimpiar);
    btnLayout->addStretch();
    cardLayout->addLayout(btnLayout);

    layout->addWidget(card);
    layout->addStretch();

    connect(m_txtEan, &QLineEdit::textChanged, this, &BodegaRegistroView::onEanChanged);
    connect(m_btnGuardar, &QPushButton::clicked, this, &BodegaRegistroView::onRegistrar);
    connect(m_btnLimpiar, &QPushButton::clicked, this, &BodegaRegistroView::limpiarCampos);

    cargarCatalogos();
}

void BodegaRegistroView::cargarCatalogos() {
    // 1. Géneros
    ApiClient::instance()->listarGeneros([this](bool ok, const QJsonValue &data, const QString &) {
        if (!ok || !data.isArray()) return;
        m_listGeneros->clear();
        m_generos.clear();
        QJsonArray arr = data.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            GeneroDto g = GeneroDto::fromJson(arr.at(i).toObject());
            m_generos.append(g);

            auto *item = new QListWidgetItem(g.genero_literario, m_listGeneros);
            item->setData(Qt::UserRole, g.id_genero);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }
    });

    // 2. Autores
    ApiClient::instance()->listarAutores([this](bool ok, const QJsonValue &data, const QString &) {
        if (!ok || !data.isArray()) return;
        m_comboAutor->clear();
        m_comboAutorEditorial->clear();
        m_autores.clear();
        QJsonArray arr = data.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            AutorDto a = AutorDto::fromJson(arr.at(i).toObject());
            m_autores.append(a);
            QString label = QString("%1 %2").arg(a.nombre, a.apellidos);
            if (!a.nacionalidad.isEmpty()) label += QString(" (%1)").arg(a.nacionalidad);
            m_comboAutor->addItem(label, a.id_autor);
            m_comboAutorEditorial->addItem(label, a.id_autor);
        }
    });

    // 3. Proveedores
    ApiClient::instance()->listarProveedores([this](bool ok, const QJsonValue &data, const QString &) {
        if (!ok || !data.isArray()) return;
        m_comboProveedor->clear();
        m_proveedores.clear();
        QJsonArray arr = data.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            ProveedorDto p = ProveedorDto::fromJson(arr.at(i).toObject());
            m_proveedores.append(p);
            m_comboProveedor->addItem(QString("%1 (RFC: %2)").arg(p.nombre_proveedor, p.rfc), p.id_proveedor);
        }
    });
}

void BodegaRegistroView::onEanChanged(const QString &text) {
    auto res = EanClassifier::clasificar(text);

    m_lblEanStatus->setText(res.mensaje);
    m_lblEanStatus->setStyleSheet(QString("font-size: 12px; color: %1; font-weight: %2;")
                                     .arg(res.colorEstilo, res.esValido || res.colorEstilo == Theme::Error ? "bold" : "500"));

    if (res.tipo == EanClassifier::Tipo::Libro) {
        m_tipoDetectado = DetectedType::Libro;
        m_panelLibro->show();
        m_panelRevista->hide();
    } else if (res.tipo == EanClassifier::Tipo::Revista) {
        m_tipoDetectado = DetectedType::Revista;
        m_panelLibro->hide();
        m_panelRevista->show();
    } else {
        m_tipoDetectado = DetectedType::Desconocido;
        m_panelLibro->hide();
        m_panelRevista->hide();
    }
}

void BodegaRegistroView::limpiarCampos() {
    m_txtEan->clear();
    m_txtSku->clear();
    m_txtNombre->clear();
    m_comboAutorEditorial->setCurrentIndex(-1);
    m_comboPeriodicidad->setCurrentIndex(0);
    m_spnPrecio->setValue(199.0);
    m_spnCantidad->setValue(10);
    m_spnEdicion->setValue(1);
    m_lblEanStatus->setText("Ingresa el código EAN-13 para clasificar automáticamente como Libro o Revista");
    m_lblEanStatus->setStyleSheet(QString("font-size: 12px; color: %1;").arg(Theme::TextMuted));
    m_panelLibro->hide();
    m_panelRevista->hide();
    m_tipoDetectado = DetectedType::Desconocido;

    for (int i = 0; i < m_listGeneros->count(); ++i) {
        m_listGeneros->item(i)->setCheckState(Qt::Unchecked);
    }
}

void BodegaRegistroView::onRegistrar() {
    if (m_tipoDetectado == DetectedType::Desconocido) {
        Toast::showToast(this, "Ingresa un código EAN-13 válido (978/979 para libros o 977 para revistas)", Toast::Warning);
        return;
    }

    qint64 ean = m_txtEan->text().trimmed().toLongLong();
    QString nombre = m_txtNombre->text().trimmed();
    if (nombre.isEmpty()) {
        Toast::showToast(this, "El título o nombre del producto es obligatorio", Toast::Warning);
        return;
    }

    if (m_comboProveedor->currentIndex() < 0) {
        Toast::showToast(this, "Selecciona un proveedor válido", Toast::Warning);
        return;
    }

    // Obtener géneros seleccionados (Relación N:M)
    QJsonArray generosArr;
    for (int i = 0; i < m_listGeneros->count(); ++i) {
        auto *item = m_listGeneros->item(i);
        if (item->checkState() == Qt::Checked) {
            generosArr.append(item->data(Qt::UserRole).toInt());
        }
    }

    QJsonObject obj;
    obj["codigo_ean"] = ean;
    if (!m_txtSku->text().trimmed().isEmpty()) {
        obj["sku"] = m_txtSku->text().trimmed().toLongLong();
    }
    obj["id_mueble"] = m_comboMueble->currentData().toInt();
    obj["id_proveedor"] = m_comboProveedor->currentData().toInt();
    obj["precio"] = m_spnPrecio->value();
    obj["cantidad"] = m_spnCantidad->value();
    obj["id_ubicacion"] = 2; // Bodega
    obj["generos"] = generosArr;

    if (m_tipoDetectado == DetectedType::Libro) {
        obj["nombre_libro"] = nombre;

        QJsonArray autoresArr;
        if (m_comboAutor->currentIndex() >= 0) {
            autoresArr.append(m_comboAutor->currentData().toInt());
        }
        obj["autores"] = autoresArr;

        ApiClient::instance()->registrarLibroBodega(obj, [this](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Libro registrado exitosamente en Bodega con géneros y autor asociados", Toast::Success);
                limpiarCampos();
            } else {
                Toast::showToast(this, "Error al registrar libro: " + msg, Toast::Error);
            }
        });
    } else {
        obj["nombre_revista"] = nombre;
        if (m_comboAutorEditorial->currentIndex() >= 0) {
            obj["autor_o_editorial"] = m_comboAutorEditorial->currentText();
        }
        obj["numero_edicion"] = m_spnEdicion->value();
        QString periodicidad = m_comboPeriodicidad->currentData().toString();
        if (!periodicidad.isEmpty()) {
            obj["periodicidad"] = periodicidad;
        }

        ApiClient::instance()->registrarRevistaBodega(obj, [this](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Revista registrada exitosamente en Bodega con géneros asociados", Toast::Success);
                limpiarCampos();
            } else {
                Toast::showToast(this, "Error al registrar revista: " + msg, Toast::Error);
            }
        });
    }
}
