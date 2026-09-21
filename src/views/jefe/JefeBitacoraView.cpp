#include "JefeBitacoraView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDate>

JefeBitacoraView::JefeBitacoraView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    // Header
    auto *title = new QLabel("Historial de Movimientos y Bitácora", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Toolbar / Filter Row
    auto *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(12);

    auto *lblFecha = new QLabel("Seleccionar Fecha:", this);
    lblFecha->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    filterLayout->addWidget(lblFecha);

    m_dateEdit = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    m_dateEdit->setFixedWidth(140);
    filterLayout->addWidget(m_dateEdit);

    m_btnBuscar = new QPushButton("Consultar", this);
    m_btnBuscar->setStyleSheet(Theme::buttonStyle());
    filterLayout->addWidget(m_btnBuscar);

    m_btnHoy = new QPushButton("Ver Hoy", this);
    m_btnHoy->setStyleSheet(Theme::secondaryButtonStyle());
    filterLayout->addWidget(m_btnHoy);

    filterLayout->addStretch();

    m_lblTotal = new QLabel("Total de eventos: 0", this);
    m_lblTotal->setStyleSheet(QString("color: %1; font-weight: bold;").arg(Theme::Accent));
    filterLayout->addWidget(m_lblTotal);

    layout->addLayout(filterLayout);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"Fecha y Hora", "Usuario", "Rol", "Acción", "Detalle"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    connect(m_btnBuscar, &QPushButton::clicked, this, &JefeBitacoraView::cargarMovimientos);
    connect(m_btnHoy, &QPushButton::clicked, this, [this]() {
        m_dateEdit->setDate(QDate::currentDate());
        cargarMovimientos();
    });
}

void JefeBitacoraView::cargarMovimientos() {
    QString fechaStr = m_dateEdit->date().toString("yyyy-MM-dd");
    ApiClient::instance()->consultarMovimientoDiario(fechaStr, [this, fechaStr](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isObject()) {
            Toast::showToast(this, "Error al cargar movimientos: " + msg, Toast::Error);
            return;
        }

        ResumenMovimientoDiario resumen = ResumenMovimientoDiario::fromJson(data.toObject());
        m_table->setRowCount(resumen.movimientos.size());
        m_lblTotal->setText(QString("Total de eventos (%1): %2").arg(fechaStr).arg(resumen.total_eventos));

        for (int i = 0; i < resumen.movimientos.size(); ++i) {
            const auto &item = resumen.movimientos.at(i);
            m_table->setItem(i, 0, new QTableWidgetItem(item.fecha_hora));
            m_table->setItem(i, 1, new QTableWidgetItem(item.usuario));
            m_table->setItem(i, 2, new QTableWidgetItem(item.rol));
            m_table->setItem(i, 3, new QTableWidgetItem(item.accion));
            m_table->setItem(i, 4, new QTableWidgetItem(item.detalle));
        }
    });
}
