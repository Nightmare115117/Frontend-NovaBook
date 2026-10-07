#include "BodegaComprasView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include "EanClassifier.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QFrame>

BodegaComprasView::BodegaComprasView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(18);

    auto *title = new QLabel("Recepción de Mercancía — Compras a Proveedor", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Header Card: Proveedor y Referencia / Factura
    auto *headerCard = new QFrame(this);
    headerCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 12px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setSpacing(16);

    auto *lblProv = new QLabel("Proveedor:", headerCard);
    lblProv->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    headerLayout->addWidget(lblProv);

    m_comboProveedor = new QComboBox(headerCard);
    m_comboProveedor->setMinimumWidth(260);
    headerLayout->addWidget(m_comboProveedor);

    auto *lblObs = new QLabel("Factura / Observaciones:", headerCard);
    lblObs->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    headerLayout->addWidget(lblObs);

    m_txtObs = new QLineEdit(headerCard);
    m_txtObs->setPlaceholderText("Ej. Factura F-1024 / Pedido mensual");
    headerLayout->addWidget(m_txtObs, 1);

    layout->addWidget(headerCard);

    // Add item card
    auto *addCard = new QFrame(this);
    addCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 12px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *cardLayout = new QVBoxLayout(addCard);
    cardLayout->setSpacing(8);

    auto *addLayout = new QHBoxLayout();
    addLayout->setSpacing(12);

    m_txtEan = new QLineEdit(addCard);
    m_txtEan->setPlaceholderText("Código EAN-13 (13 dígitos)...");
    addLayout->addWidget(m_txtEan, 2);

    auto *lblCant = new QLabel("Cant:", addCard);
    lblCant->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    addLayout->addWidget(lblCant);

    m_spnCantidad = new QSpinBox(addCard);
    m_spnCantidad->setRange(1, 9999);
    m_spnCantidad->setValue(10);
    m_spnCantidad->setFixedWidth(80);
    addLayout->addWidget(m_spnCantidad);

    auto *lblCosto = new QLabel("Costo Unit:", addCard);
    lblCosto->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    addLayout->addWidget(lblCosto);

    m_spnCosto = new QDoubleSpinBox(addCard);
    m_spnCosto->setRange(0.0, 99999.0);
    m_spnCosto->setValue(150.0);
    m_spnCosto->setPrefix("$ ");
    m_spnCosto->setFixedWidth(110);
    addLayout->addWidget(m_spnCosto);

    m_btnAgregar = new QPushButton("＋ Añadir a Compra", addCard);
    m_btnAgregar->setStyleSheet(Theme::buttonStyle(Theme::Role2, Theme::BgDark, "#7BB9D9"));
    addLayout->addWidget(m_btnAgregar);

    cardLayout->addLayout(addLayout);

    m_lblEanStatus = new QLabel("Ingresa el código EAN-13 para clasificar automáticamente como Libro o Revista", addCard);
    m_lblEanStatus->setStyleSheet(QString("font-size: 12px; color: %1;").arg(Theme::TextMuted));
    cardLayout->addWidget(m_lblEanStatus);

    layout->addWidget(addCard);

    // Table
    m_tableCart = new QTableWidget(this);
    m_tableCart->setColumnCount(7);
    m_tableCart->setHorizontalHeaderLabels({"EAN", "Tipo", "Descripción", "Cantidad", "Costo Unit.", "Subtotal", "Acción"});
    m_tableCart->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_tableCart->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_tableCart->verticalHeader()->setVisible(false);
    m_tableCart->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableCart->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_tableCart);

    // Bottom Summary and Actions
    auto *bottomCard = new QFrame(this);
    bottomCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 14px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *bottomLayout = new QHBoxLayout(bottomCard);
    bottomLayout->setSpacing(16);

    m_btnVaciar = new QPushButton("Vaciar Orden", bottomCard);
    m_btnVaciar->setStyleSheet(Theme::secondaryButtonStyle());
    bottomLayout->addWidget(m_btnVaciar);

    bottomLayout->addStretch();

    m_lblTotalArticulos = new QLabel("Piezas Recibidas: 0", bottomCard);
    m_lblTotalArticulos->setStyleSheet(QString("font-size: 14px; font-weight: bold; color: %1;").arg(Theme::TextMuted));
    bottomLayout->addWidget(m_lblTotalArticulos);

    m_lblTotalPagar = new QLabel("Total Factura: $ 0.00", bottomCard);
    m_lblTotalPagar->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(Theme::Accent));
    bottomLayout->addWidget(m_lblTotalPagar);

    m_btnProcesar = new QPushButton("✔ Registrar Compra e Incrementar Bodega", bottomCard);
    m_btnProcesar->setStyleSheet(Theme::buttonStyle(Theme::Success, Theme::BgDark, "#72D4A0"));
    bottomLayout->addWidget(m_btnProcesar);

    layout->addWidget(bottomCard);

    connect(m_txtEan, &QLineEdit::textChanged, this, &BodegaComprasView::onEanChanged);
    connect(m_btnAgregar, &QPushButton::clicked, this, &BodegaComprasView::onAgregarArticulo);
    connect(m_txtEan, &QLineEdit::returnPressed, this, &BodegaComprasView::onAgregarArticulo);
    connect(m_btnVaciar, &QPushButton::clicked, this, &BodegaComprasView::limpiarCarrito);
    connect(m_btnProcesar, &QPushButton::clicked, this, &BodegaComprasView::onRegistrarCompra);

    cargarProveedores();
}

void BodegaComprasView::onEanChanged(const QString &text) {
    EanClassifier::aplicarAControles(text, m_lblEanStatus);
}

void BodegaComprasView::cargarProveedores() {
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

void BodegaComprasView::onAgregarArticulo() {
    QString eanStr = m_txtEan->text().trimmed();
    auto eanRes = EanClassifier::clasificar(eanStr);
    if (!eanRes.esValido) {
        Toast::showToast(this, eanRes.mensaje, Toast::Warning);
        return;
    }

    qint64 ean = eanStr.toLongLong();
    int cant = m_spnCantidad->value();
    double costo = m_spnCosto->value();
    QString tipo = (eanRes.tipo == EanClassifier::Tipo::Libro ? "libro" : "revista");

    // Consult existing item title
    ApiClient::instance()->consultarExistencias(eanStr, tipo, 0, [this, ean, cant, costo, tipo](bool ok, const QJsonValue &data, const QString &) {
        QString titulo = QString("Artículo %1").arg(ean);
        if (ok && data.isArray() && !data.toArray().isEmpty()) {
            QJsonObject obj = data.toArray().first().toObject();
            titulo = obj.value("titulo").toString(titulo);
        }

        // Check if item is already in list
        bool found = false;
        for (auto &item : m_items) {
            if (item.ean == ean) {
                item.cantidad += cant;
                item.costo_unitario = costo;
                item.subtotal = item.cantidad * item.costo_unitario;
                found = true;
                break;
            }
        }

        if (!found) {
            CompraCartItem ci;
            ci.ean = ean;
            ci.tipo = tipo;
            ci.nombre = titulo;
            ci.cantidad = cant;
            ci.costo_unitario = costo;
            ci.subtotal = cant * costo;
            m_items.append(ci);
        }

        m_txtEan->clear();
        m_txtEan->setFocus();
        recalcularTotales();
    });
}

void BodegaComprasView::onQuitarFila(int row) {
    if (row >= 0 && row < m_items.size()) {
        m_items.removeAt(row);
        recalcularTotales();
    }
}

void BodegaComprasView::limpiarCarrito() {
    m_items.clear();
    m_txtObs->clear();
    recalcularTotales();
}

void BodegaComprasView::recalcularTotales() {
    m_tableCart->setRowCount(m_items.size());

    int totalPiezas = 0;
    double totalDinero = 0.0;

    for (int i = 0; i < m_items.size(); ++i) {
        const auto &item = m_items.at(i);
        totalPiezas += item.cantidad;
        totalDinero += item.subtotal;

        m_tableCart->setItem(i, 0, new QTableWidgetItem(QString::number(item.ean)));
        m_tableCart->setItem(i, 1, new QTableWidgetItem(item.tipo.toUpper()));
        m_tableCart->setItem(i, 2, new QTableWidgetItem(item.nombre));
        m_tableCart->setItem(i, 3, new QTableWidgetItem(QString::number(item.cantidad)));
        m_tableCart->setItem(i, 4, new QTableWidgetItem(QString("$ %1").arg(QString::number(item.costo_unitario, 'f', 2))));
        m_tableCart->setItem(i, 5, new QTableWidgetItem(QString("$ %1").arg(QString::number(item.subtotal, 'f', 2))));

        auto *btnQuitar = new QPushButton("✖ Quitar", this);
        btnQuitar->setStyleSheet(Theme::dangerButtonStyle());
        connect(btnQuitar, &QPushButton::clicked, this, [this, i]() {
            onQuitarFila(i);
        });
        m_tableCart->setCellWidget(i, 6, btnQuitar);
    }

    m_lblTotalArticulos->setText(QString("Piezas Recibidas: %1").arg(totalPiezas));
    m_lblTotalPagar->setText(QString("Total Factura: $ %1").arg(QString::number(totalDinero, 'f', 2)));
}

void BodegaComprasView::onRegistrarCompra() {
    if (m_comboProveedor->currentIndex() < 0) {
        Toast::showToast(this, "Selecciona un proveedor válido", Toast::Warning);
        return;
    }
    if (m_items.isEmpty()) {
        Toast::showToast(this, "La orden de compra no tiene artículos", Toast::Warning);
        return;
    }

    int idProveedor = m_comboProveedor->currentData().toInt();
    QJsonObject req;
    req["id_proveedor"] = idProveedor;
    if (!m_txtObs->text().trimmed().isEmpty()) {
        req["observaciones"] = m_txtObs->text().trimmed();
    }

    QJsonArray itemsArr;
    for (const auto &it : m_items) {
        QJsonObject obj;
        obj["codigo_ean"] = it.ean;
        obj["tipo_producto"] = it.tipo;
        obj["cantidad"] = it.cantidad;
        obj["costo_unitario"] = it.costo_unitario;
        itemsArr.append(obj);
    }
    req["items"] = itemsArr;

    ApiClient::instance()->registrarCompra(req, [this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isObject()) {
            Toast::showToast(this, "Error al registrar compra: " + msg, Toast::Error);
            return;
        }

        CompraResponse cr = CompraResponse::fromJson(data.toObject());
        limpiarCarrito();
        Toast::showToast(this, QString("Compra #%1 registrada con éxito. Inventario en bodega actualizado.").arg(cr.id_compra), Toast::Success);
    });
}
