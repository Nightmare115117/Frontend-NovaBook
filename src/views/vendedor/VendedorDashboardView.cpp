#include "VendedorDashboardView.h"
#include "StatCard.h"
#include "Theme.h"
#include "ApiClient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

VendedorDashboardView::VendedorDashboardView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(24);

    // Header
    auto *title = new QLabel("Punto de Atención — Vendedor de Piso", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    auto *subtitle = new QLabel("Atención a clientes, consultas de existencias, bajas por venta y devoluciones", this);
    subtitle->setStyleSheet(QString("font-size: 13px; color: %1;").arg(Theme::TextMuted));

    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Metrics Row
    auto *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(16);

    m_cardStockTienda = new StatCard("Stock en Piso de Venta", "...", "Unidades listas para venta", Theme::Role3, this);
    m_cardCatalogo = new StatCard("Catálogo Disponible", "...", "Títulos exhibidos", Theme::Role1, this);

    cardsLayout->addWidget(m_cardStockTienda);
    cardsLayout->addWidget(m_cardCatalogo);
    layout->addLayout(cardsLayout);

    // Info Card
    auto *infoCard = new QFrame(this);
    infoCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 20px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setSpacing(12);

    auto *infoTitle = new QLabel("Módulos de Mostrador", infoCard);
    infoTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(Theme::Role3));

    auto *infoDesc = new QLabel(
        "• <b>Consultar Existencias:</b> Localiza ejemplares al instante en bodega o piso con precio y proveedor.<br>"
        "• <b>Baja por Venta (POS):</b> Escanea o añade productos al carrito para procesar ventas de contado.<br>"
        "• <b>Requisición a Bodega:</b> Devuelve sobrantes o producto de piso al almacén.<br>"
        "• <b>Generar Devolución:</b> Levanta actas de devolución a proveedores con descarga de PDF.",
        infoCard
    );
    infoDesc->setStyleSheet(QString("color: %1; font-size: 13px; line-height: 1.6;").arg(Theme::TextPrimary));

    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(infoDesc);
    layout->addWidget(infoCard);

    layout->addStretch();
}

void VendedorDashboardView::refreshData() {
    ApiClient::instance()->consultarExistencias("", "", 0, [this](bool ok, const QJsonValue &data, const QString &) {
        if (ok && data.isArray()) {
            QJsonArray arr = data.toArray();
            int totalTienda = 0;
            for (const auto &v : arr) {
                totalTienda += v.toObject().value("stock_tienda").toInt();
            }
            m_cardStockTienda->setValue(QString::number(totalTienda));
            m_cardCatalogo->setValue(QString::number(arr.size()));
        } else {
            m_cardStockTienda->setValue("0");
            m_cardCatalogo->setValue("0");
        }
    });
}
