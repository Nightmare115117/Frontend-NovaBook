#include "VendedorVentasView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDialog>
#include <QMessageBox>
#include <QFrame>

VendedorVentasView::VendedorVentasView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    auto *title = new QLabel("Punto de Venta — Baja por Venta", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Scan / Add item bar
    auto *addCard = new QFrame(this);
    addCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 14px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *addLayout = new QHBoxLayout(addCard);
    addLayout->setSpacing(12);

    m_txtEan = new QLineEdit(addCard);
    m_txtEan->setPlaceholderText("Escanear o ingresar EAN (13 dígitos)...");
    addLayout->addWidget(m_txtEan, 2);

    m_comboTipo = new QComboBox(addCard);
    m_comboTipo->addItem("Libro", "libro");
    m_comboTipo->addItem("Revista", "revista");
    addLayout->addWidget(m_comboTipo);

    auto *lblCant = new QLabel("Cant:", addCard);
    lblCant->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    addLayout->addWidget(lblCant);

    m_spnCantidad = new QSpinBox(addCard);
    m_spnCantidad->setRange(1, 999);
    m_spnCantidad->setValue(1);
    m_spnCantidad->setFixedWidth(70);
    addLayout->addWidget(m_spnCantidad);

    m_btnAgregar = new QPushButton("＋ Añadir al Carrito", addCard);
    m_btnAgregar->setStyleSheet(Theme::buttonStyle(Theme::Role3, Theme::BgDark, "#A5D67D"));
    addLayout->addWidget(m_btnAgregar);

    layout->addWidget(addCard);

    // Cart Table
    m_tableCart = new QTableWidget(this);
    m_tableCart->setColumnCount(6);
    m_tableCart->setHorizontalHeaderLabels({"EAN", "Tipo", "Descripción", "Cantidad", "Precio Unit.", "Acción"});
    m_tableCart->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_tableCart->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_tableCart->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_tableCart->verticalHeader()->setVisible(false);
    m_tableCart->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableCart->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_tableCart);

    // Bottom Summary and Checkout
    auto *bottomCard = new QFrame(this);
    bottomCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 16px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *bottomLayout = new QHBoxLayout(bottomCard);
    bottomLayout->setSpacing(20);

    m_btnVaciar = new QPushButton("Vaciar Carrito", bottomCard);
    m_btnVaciar->setStyleSheet(Theme::secondaryButtonStyle());
    bottomLayout->addWidget(m_btnVaciar);

    bottomLayout->addStretch();

    m_lblTotalArticulos = new QLabel("Artículos: 0", bottomCard);
    m_lblTotalArticulos->setStyleSheet(QString("font-size: 14px; font-weight: bold; color: %1;").arg(Theme::TextMuted));
    bottomLayout->addWidget(m_lblTotalArticulos);

    m_lblTotalPagar = new QLabel("Total a Cobrar: $ 0.00", bottomCard);
    m_lblTotalPagar->setStyleSheet(QString("font-size: 18px; font-weight: bold; color: %1;").arg(Theme::Accent));
    bottomLayout->addWidget(m_lblTotalPagar);

    m_btnProcesar = new QPushButton("✔ Procesar y Cobrar Venta", bottomCard);
    m_btnProcesar->setStyleSheet(Theme::buttonStyle(Theme::Success, Theme::BgDark, "#72D4A0"));
    bottomLayout->addWidget(m_btnProcesar);

    layout->addWidget(bottomCard);

    connect(m_btnAgregar, &QPushButton::clicked, this, &VendedorVentasView::onAgregarAlCarrito);
    connect(m_txtEan, &QLineEdit::returnPressed, this, &VendedorVentasView::onAgregarAlCarrito);
    connect(m_btnVaciar, &QPushButton::clicked, this, &VendedorVentasView::limpiarCarrito);
    connect(m_btnProcesar, &QPushButton::clicked, this, &VendedorVentasView::onProcesarVenta);
}

void VendedorVentasView::onAgregarAlCarrito() {
    QString eanStr = m_txtEan->text().trimmed();
    if (eanStr.isEmpty() || !eanStr.toLongLong()) {
        Toast::showToast(this, "Ingresa un código EAN numérico", Toast::Warning);
        return;
    }

    qint64 ean = eanStr.toLongLong();
    int cant = m_spnCantidad->value();
    QString tipo = m_comboTipo->currentData().toString();

    // Consult existences to get title and price
    ApiClient::instance()->consultarExistencias(eanStr, tipo, 0, [this, ean, cant, tipo](bool ok, const QJsonValue &data, const QString &) {
        QString titulo = QString("Artículo %1").arg(ean);
        double precio = 0.0;

        if (ok && data.isArray() && !data.toArray().isEmpty()) {
            QJsonObject obj = data.toArray().first().toObject();
            titulo = obj.value("titulo").toString(titulo);
            precio = obj.value("precio").toDouble(0.0);
        }

        // Check if already in cart
        bool found = false;
        for (auto &item : m_cart) {
            if (item.ean == ean) {
                item.cantidad += cant;
                item.subtotal = item.cantidad * item.precio;
                found = true;
                break;
            }
        }

        if (!found) {
            CartItem ci;
            ci.ean = ean;
            ci.tipo = tipo;
            ci.nombre = titulo;
            ci.cantidad = cant;
            ci.precio = precio;
            ci.subtotal = cant * precio;
            m_cart.append(ci);
        }

        m_txtEan->clear();
        m_spnCantidad->setValue(1);
        m_txtEan->setFocus();
        recalcularTotales();
    });
}

void VendedorVentasView::onQuitarFila(int row) {
    if (row >= 0 && row < m_cart.size()) {
        m_cart.removeAt(row);
        recalcularTotales();
    }
}

void VendedorVentasView::limpiarCarrito() {
    m_cart.clear();
    recalcularTotales();
}

void VendedorVentasView::recalcularTotales() {
    m_tableCart->setRowCount(m_cart.size());

    int totalPiezas = 0;
    double totalDinero = 0.0;

    for (int i = 0; i < m_cart.size(); ++i) {
        const auto &item = m_cart.at(i);
        totalPiezas += item.cantidad;
        totalDinero += item.subtotal;

        m_tableCart->setItem(i, 0, new QTableWidgetItem(QString::number(item.ean)));
        m_tableCart->setItem(i, 1, new QTableWidgetItem(item.tipo.toUpper()));
        m_tableCart->setItem(i, 2, new QTableWidgetItem(item.nombre));
        m_tableCart->setItem(i, 3, new QTableWidgetItem(QString::number(item.cantidad)));
        m_tableCart->setItem(i, 4, new QTableWidgetItem(item.precio > 0 ? QString("$ %1").arg(QString::number(item.precio, 'f', 2)) : "Al cobrar"));

        auto *btnQuitar = new QPushButton("✖ Quitar", this);
        btnQuitar->setStyleSheet(Theme::dangerButtonStyle());
        connect(btnQuitar, &QPushButton::clicked, this, [this, i]() {
            onQuitarFila(i);
        });
        m_tableCart->setCellWidget(i, 5, btnQuitar);
    }

    m_lblTotalArticulos->setText(QString("Artículos: %1").arg(totalPiezas));
    m_lblTotalPagar->setText(QString("Total a Cobrar: $ %1").arg(QString::number(totalDinero, 'f', 2)));
}

void VendedorVentasView::onProcesarVenta() {
    if (m_cart.isEmpty()) {
        Toast::showToast(this, "El carrito de compra está vacío", Toast::Warning);
        return;
    }

    QJsonArray itemsArr;
    for (const auto &item : m_cart) {
        QJsonObject it;
        it["codigo_ean"] = item.ean;
        it["tipo_producto"] = item.tipo;
        it["cantidad"] = item.cantidad;
        itemsArr.append(it);
    }

    ApiClient::instance()->registrarVenta(itemsArr, [this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isObject()) {
            Toast::showToast(this, "Error al procesar venta: " + msg, Toast::Error);
            return;
        }

        VentaResponse vr = VentaResponse::fromJson(data.toObject());

        // Display Receipt Dialog
        QDialog dlg(this);
        dlg.setWindowTitle("Comprobante de Venta Exitosa");
        dlg.setFixedWidth(420);

        auto *dlgLayout = new QVBoxLayout(&dlg);
        dlgLayout->setSpacing(14);

        auto *lblHeader = new QLabel(QString(
            "<div style='text-align: center;'>"
            "<h2 style='color: %1; margin:0;'>LIBRERÍA NOVABOOK</h2>"
            "<p style='color: %2; margin:2px;'>Ticket de Venta #%3</p>"
            "<p style='color: %2; margin:2px;'>Fecha: %4</p>"
            "</div><hr style='border: 1px solid #2A2A2A;'>"
        ).arg(Theme::Accent).arg(Theme::TextMuted).arg(vr.id_venta).arg(vr.fecha_hora), &dlg);
        dlgLayout->addWidget(lblHeader);

        QString detailHtml = "<table width='100%' style='color: #F0EDE8; font-size: 12px;'>";
        detailHtml += "<tr><th align='left'>Cant</th><th align='left'>Producto</th><th align='right'>Subtotal</th></tr>";
        for (const auto &it : vr.items) {
            detailHtml += QString("<tr><td>%1x</td><td>%2</td><td align='right'>$ %3</td></tr>")
                .arg(it.cantidad)
                .arg(it.nombre_producto)
                .arg(QString::number(it.subtotal, 'f', 2));
        }
        detailHtml += "</table><hr style='border: 1px solid #2A2A2A;'>";
        detailHtml += QString("<h3 align='right' style='color: %1; margin: 4px;'>TOTAL PAGADO: $ %2</h3>")
            .arg(Theme::Success)
            .arg(QString::number(vr.total, 'f', 2));

        auto *lblDetails = new QLabel(detailHtml, &dlg);
        dlgLayout->addWidget(lblDetails);

        auto *btnCerrar = new QPushButton("Aceptar y Nueva Venta", &dlg);
        btnCerrar->setStyleSheet(Theme::buttonStyle(Theme::Success, Theme::BgDark, "#72D4A0"));
        connect(btnCerrar, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlgLayout->addWidget(btnCerrar);

        dlg.exec();

        limpiarCarrito();
        Toast::showToast(this, "Venta completada con éxito", Toast::Success);
    });
}
