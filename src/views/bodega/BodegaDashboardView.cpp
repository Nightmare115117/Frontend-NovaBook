#include "BodegaDashboardView.h"
#include "StatCard.h"
#include "Theme.h"
#include "ApiClient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

BodegaDashboardView::BodegaDashboardView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(24);

    // Header
    auto *title = new QLabel("Panel de Almacén — Personal de Bodega", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    auto *subtitle = new QLabel("Control de entradas, altas de producto y requisiciones hacia piso de venta", this);
    subtitle->setStyleSheet(QString("font-size: 13px; color: %1;").arg(Theme::TextMuted));

    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Metrics Row
    auto *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(16);

    m_cardStockBodega = new StatCard("Stock en Bodega", "...", "Unidades totales resguardadas", Theme::Role2, this);
    m_cardTitulos = new StatCard("Títulos Registrados", "...", "Referencias activas", Theme::Role1, this);

    cardsLayout->addWidget(m_cardStockBodega);
    cardsLayout->addWidget(m_cardTitulos);
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

    auto *infoTitle = new QLabel("Operaciones de Almacén", infoCard);
    infoTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(Theme::Role2));

    auto *infoDesc = new QLabel(
        "• <b>Registrar Libros / Revistas:</b> Da de alta nueva mercancía recibida de proveedores con su EAN-13 y SKU.<br>"
        "• <b>Requisición de Salida:</b> Traslada ejemplares desde la bodega hacia los exhibidores de tienda.<br>"
        "• <b>Consultar Inventario:</b> Visualiza el balance de existencias disponibles en almacén vs piso.",
        infoCard
    );
    infoDesc->setStyleSheet(QString("color: %1; font-size: 13px; line-height: 1.6;").arg(Theme::TextPrimary));

    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(infoDesc);
    layout->addWidget(infoCard);

    layout->addStretch();
}

void BodegaDashboardView::refreshData() {
    ApiClient::instance()->consultarExistencias("", "", 0, [this](bool ok, const QJsonValue &data, const QString &) {
        if (ok && data.isArray()) {
            QJsonArray arr = data.toArray();
            int totalBodega = 0;
            for (const auto &v : arr) {
                totalBodega += v.toObject().value("stock_bodega").toInt();
            }
            m_cardStockBodega->setValue(QString::number(totalBodega));
            m_cardTitulos->setValue(QString::number(arr.size()));
        } else {
            m_cardStockBodega->setValue("0");
            m_cardTitulos->setValue("0");
        }
    });
}
