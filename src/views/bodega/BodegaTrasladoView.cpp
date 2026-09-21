#include "BodegaTrasladoView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

BodegaTrasladoView::BodegaTrasladoView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    auto *title = new QLabel("Requisición de Salida: Bodega ➔ Piso de Venta", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    auto *subtitle = new QLabel("Transfiere ejemplares desde el almacén hacia los estantes de la librería", this);
    subtitle->setStyleSheet(QString("font-size: 13px; color: %1;").arg(Theme::TextMuted));
    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Form Card
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
    m_comboTipo->addItem("Libros", "libro");
    m_comboTipo->addItem("Revistas", "revista");
    form->addRow("Tipo de Artículo:", m_comboTipo);

    m_txtEan = new QLineEdit(card);
    m_txtEan->setPlaceholderText("Código EAN del producto (13 dígitos)");
    form->addRow("Código EAN:", m_txtEan);

    m_spnCantidad = new QSpinBox(card);
    m_spnCantidad->setRange(1, 9999);
    m_spnCantidad->setValue(5);
    form->addRow("Cantidad a Trasladar:", m_spnCantidad);

    m_txtObs = new QLineEdit(card);
    m_txtObs->setPlaceholderText("Notas u observaciones (opcional)");
    form->addRow("Observaciones:", m_txtObs);

    cardLayout->addLayout(form);

    m_btnTrasladar = new QPushButton("➔ Ejecutar Traslado a Tienda", card);
    m_btnTrasladar->setStyleSheet(Theme::buttonStyle(Theme::Role2, Theme::BgDark, "#7BB9D9"));
    cardLayout->addWidget(m_btnTrasladar);

    layout->addWidget(card);

    // Result Card
    m_resCard = new QFrame(this);
    m_resCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-left: 4px solid %3;
            border-radius: 8px;
            padding: 16px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border).arg(Theme::Success));
    m_resCard->hide();

    auto *resLayout = new QVBoxLayout(m_resCard);
    resLayout->setSpacing(8);

    m_lblResMensaje = new QLabel(m_resCard);
    m_lblResMensaje->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px;").arg(Theme::Success));

    m_lblResBodega = new QLabel(m_resCard);
    m_lblResBodega->setStyleSheet(QString("color: %1;").arg(Theme::TextPrimary));

    m_lblResTienda = new QLabel(m_resCard);
    m_lblResTienda->setStyleSheet(QString("color: %1;").arg(Theme::TextPrimary));

    resLayout->addWidget(m_lblResMensaje);
    resLayout->addWidget(m_lblResBodega);
    resLayout->addWidget(m_lblResTienda);

    layout->addWidget(m_resCard);
    layout->addStretch();

    connect(m_btnTrasladar, &QPushButton::clicked, this, &BodegaTrasladoView::onTrasladar);
}

void BodegaTrasladoView::onTrasladar() {
    QString eanStr = m_txtEan->text().trimmed();
    if (eanStr.isEmpty() || !eanStr.toLongLong()) {
        Toast::showToast(this, "Ingresa un código EAN válido", Toast::Warning);
        return;
    }

    qint64 ean = eanStr.toLongLong();
    int cant = m_spnCantidad->value();
    QString obs = m_txtObs->text().trimmed();
    bool isLibro = (m_comboTipo->currentIndex() == 0);

    auto handleResponse = [this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isObject()) {
            Toast::showToast(this, "Error al trasladar: " + msg, Toast::Error);
            m_resCard->hide();
            return;
        }

        TrasladoResponse tr = TrasladoResponse::fromJson(data.toObject());
        Toast::showToast(this, tr.mensaje, Toast::Success);

        m_resCard->show();
        m_lblResMensaje->setText("✔ " + tr.mensaje);
        m_lblResBodega->setText(QString("• Stock restante en Bodega: <b>%1 piezas</b>").arg(tr.stock_origen_restante));
        m_lblResTienda->setText(QString("• Nuevo stock disponible en Piso de Tienda: <b>%1 piezas</b>").arg(tr.stock_destino_nuevo));

        m_txtEan->clear();
        m_txtObs->clear();
    };

    if (isLibro) {
        ApiClient::instance()->trasladoBodegaLibros(ean, cant, obs, handleResponse);
    } else {
        ApiClient::instance()->trasladoBodegaRevistas(ean, cant, obs, handleResponse);
    }
}
