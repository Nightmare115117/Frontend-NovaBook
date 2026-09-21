#include "BodegaRegistroView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
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

    auto *title = new QLabel("Alta de Mercancía en Almacén", this);
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

    m_comboTipo = new QComboBox(card);
    m_comboTipo->addItem("Libro", "libro");
    m_comboTipo->addItem("Revista", "revista");
    form->addRow("Tipo de Producto:", m_comboTipo);

    m_txtEan = new QLineEdit(card);
    m_txtEan->setPlaceholderText("Código de barras EAN-13 (13 dígitos numéricos)");
    form->addRow("Código EAN:", m_txtEan);

    m_txtSku = new QLineEdit(card);
    m_txtSku->setPlaceholderText("SKU numérico (6 a 7 dígitos)");
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

    // Dynamic fields for Libro
    m_panelLibro = new QWidget(card);
    auto *libroLayout = new QFormLayout(m_panelLibro);
    libroLayout->setContentsMargins(0, 0, 0, 0);
    libroLayout->setSpacing(12);

    m_comboGenero = new QComboBox(m_panelLibro);
    m_comboGenero->addItem("Psicología (11)", 11);
    m_comboGenero->addItem("Autoayuda (12)", 12);
    m_comboGenero->addItem("Metafísica (13)", 13);
    m_comboGenero->addItem("Infantiles (61)", 61);
    libroLayout->addRow("Género Literario:", m_comboGenero);

    m_txtAutor = new QLineEdit(m_panelLibro);
    m_txtAutor->setPlaceholderText("Nombre del autor o autores");
    libroLayout->addRow("Autor:", m_txtAutor);

    form->addRow(m_panelLibro);

    // Dynamic fields for Revista
    m_panelRevista = new QWidget(card);
    auto *revistaLayout = new QFormLayout(m_panelRevista);
    revistaLayout->setContentsMargins(0, 0, 0, 0);
    revistaLayout->setSpacing(12);

    m_spnEdicion = new QSpinBox(m_panelRevista);
    m_spnEdicion->setRange(1, 9999);
    m_spnEdicion->setValue(1);
    revistaLayout->addRow("Número de Edición:", m_spnEdicion);

    m_txtPeriodicidad = new QLineEdit(m_panelRevista);
    m_txtPeriodicidad->setPlaceholderText("Mensual, Semanal, Quincenal...");
    revistaLayout->addRow("Periodicidad:", m_txtPeriodicidad);

    form->addRow(m_panelRevista);
    m_panelRevista->hide();

    // Mueble y Proveedor
    m_comboMueble = new QComboBox(card);
    m_comboMueble->addItem("Mueble 11", 11);
    m_comboMueble->addItem("Mueble 12", 12);
    m_comboMueble->addItem("Mueble 13", 13);
    m_comboMueble->addItem("Mueble 61", 61);
    m_comboMueble->addItem("Mueble 62", 62);
    form->addRow("Mueble de Asignación:", m_comboMueble);

    m_comboProveedor = new QComboBox(card);
    m_comboProveedor->addItem("Planeta México (1)", 1);
    m_comboProveedor->addItem("Planeta México Infantil (2)", 2);
    m_comboProveedor->addItem("Penguin Random House (3)", 3);
    form->addRow("Proveedor:", m_comboProveedor);

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

    connect(m_comboTipo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BodegaRegistroView::onTipoChanged);
    connect(m_btnGuardar, &QPushButton::clicked, this, &BodegaRegistroView::onRegistrar);
    connect(m_btnLimpiar, &QPushButton::clicked, this, &BodegaRegistroView::limpiarCampos);
}

void BodegaRegistroView::onTipoChanged(int index) {
    bool isLibro = (index == 0);
    m_panelLibro->setVisible(isLibro);
    m_panelRevista->setVisible(!isLibro);
}

void BodegaRegistroView::limpiarCampos() {
    m_txtEan->clear();
    m_txtSku->clear();
    m_txtNombre->clear();
    m_txtAutor->clear();
    m_txtPeriodicidad->clear();
    m_spnPrecio->setValue(199.0);
    m_spnCantidad->setValue(10);
    m_spnEdicion->setValue(1);
}

void BodegaRegistroView::onRegistrar() {
    QString eanStr = m_txtEan->text().trimmed();
    if (eanStr.length() != 13 || !eanStr.toLongLong()) {
        Toast::showToast(this, "El código EAN debe tener 13 dígitos numéricos", Toast::Warning);
        return;
    }

    qint64 ean = eanStr.toLongLong();
    QString nombre = m_txtNombre->text().trimmed();
    if (nombre.isEmpty()) {
        Toast::showToast(this, "El título o nombre del producto es obligatorio", Toast::Warning);
        return;
    }

    bool isLibro = (m_comboTipo->currentIndex() == 0);

    QJsonObject obj;
    obj["codigo_ean"] = ean;
    if (!m_txtSku->text().trimmed().isEmpty()) {
        obj["sku"] = m_txtSku->text().trimmed().toLongLong();
    }
    obj["id_mueble"] = m_comboMueble->currentData().toInt();
    obj["id_proveedor"] = m_comboProveedor->currentData().toInt();
    obj["precio"] = m_spnPrecio->value();
    obj["cantidad"] = m_spnCantidad->value();
    obj["id_ubicacion"] = 2; // Ubicación Bodega

    if (isLibro) {
        obj["nombre_libro"] = nombre;
        obj["id_genero"] = m_comboGenero->currentData().toInt();
        if (!m_txtAutor->text().trimmed().isEmpty()) {
            obj["autor"] = m_txtAutor->text().trimmed();
        }

        ApiClient::instance()->registrarLibroBodega(obj, [this](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Libro registrado exitosamente en Bodega", Toast::Success);
                limpiarCampos();
            } else {
                Toast::showToast(this, "Error al registrar libro: " + msg, Toast::Error);
            }
        });
    } else {
        obj["nombre_revista"] = nombre;
        obj["numero_edicion"] = m_spnEdicion->value();
        if (!m_txtPeriodicidad->text().trimmed().isEmpty()) {
            obj["periodicidad"] = m_txtPeriodicidad->text().trimmed();
        }

        ApiClient::instance()->registrarRevistaBodega(obj, [this](bool ok, const QJsonValue &, const QString &msg) {
            if (ok) {
                Toast::showToast(this, "Revista registrada exitosamente en Bodega", Toast::Success);
                limpiarCampos();
            } else {
                Toast::showToast(this, "Error al registrar revista: " + msg, Toast::Error);
            }
        });
    }
}
