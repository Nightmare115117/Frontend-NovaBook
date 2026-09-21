#include "JefeDevolucionesView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QTextEdit>
#include <QMessageBox>

JefeDevolucionesView::JefeDevolucionesView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    // Header
    auto *title = new QLabel("Autorización y Control de Devoluciones", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Toolbar / Filter Row
    auto *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(12);

    auto *lblEstado = new QLabel("Filtrar Estado:", this);
    lblEstado->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    filterLayout->addWidget(lblEstado);

    m_comboEstado = new QComboBox(this);
    m_comboEstado->addItems({"Todos", "Pendiente", "Autorizado", "Rechazado"});
    m_comboEstado->setFixedWidth(150);
    filterLayout->addWidget(m_comboEstado);

    m_btnRecargar = new QPushButton("Actualizar Lista", this);
    m_btnRecargar->setStyleSheet(Theme::secondaryButtonStyle());
    filterLayout->addWidget(m_btnRecargar);

    filterLayout->addStretch();

    m_btnEvaluar = new QPushButton("Evaluar Devolución Seleccionada", this);
    m_btnEvaluar->setStyleSheet(Theme::buttonStyle(Theme::Accent, Theme::BgDark, Theme::AccentHover));
    filterLayout->addWidget(m_btnEvaluar);

    layout->addLayout(filterLayout);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"ID Devolución", "Fecha", "Tipo", "Proveedor", "Total Piezas", "Estado", "Autorizado Por"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    connect(m_comboEstado, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &JefeDevolucionesView::cargarDevoluciones);
    connect(m_btnRecargar, &QPushButton::clicked, this, &JefeDevolucionesView::cargarDevoluciones);
    connect(m_btnEvaluar, &QPushButton::clicked, this, &JefeDevolucionesView::onEvaluarDevolucion);
}

void JefeDevolucionesView::cargarDevoluciones() {
    QString estado = m_comboEstado->currentText();
    ApiClient::instance()->consultarHistorialDevoluciones("", estado, 0, [this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isArray()) {
            Toast::showToast(this, "Error al consultar historial: " + msg, Toast::Error);
            return;
        }

        m_devoluciones.clear();
        QJsonArray arr = data.toArray();
        m_table->setRowCount(arr.size());

        for (int i = 0; i < arr.size(); ++i) {
            Devolucion d = Devolucion::fromJson(arr.at(i).toObject());
            m_devoluciones.append(d);

            m_table->setItem(i, 0, new QTableWidgetItem(QString("#%1").arg(d.id_devolucion)));
            m_table->setItem(i, 1, new QTableWidgetItem(d.fecha));
            m_table->setItem(i, 2, new QTableWidgetItem(d.tipo_producto));
            m_table->setItem(i, 3, new QTableWidgetItem(d.nombre_proveedor.isEmpty() ? QString("Proveedor #%1").arg(d.id_proveedor) : d.nombre_proveedor));
            m_table->setItem(i, 4, new QTableWidgetItem(QString::number(d.total_piezas)));

            auto *estadoItem = new QTableWidgetItem(d.estado);
            if (d.estado.toLower() == "autorizado") {
                estadoItem->setForeground(QColor(Theme::Success));
            } else if (d.estado.toLower() == "rechazado") {
                estadoItem->setForeground(QColor(Theme::Error));
            } else {
                estadoItem->setForeground(QColor(Theme::Warning));
            }
            m_table->setItem(i, 5, estadoItem);

            m_table->setItem(i, 6, new QTableWidgetItem(d.autorizado_por.isEmpty() ? "-" : d.autorizado_por));
        }
    });
}

void JefeDevolucionesView::onEvaluarDevolucion() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_devoluciones.size()) {
        Toast::showToast(this, "Por favor selecciona una devolución de la tabla", Toast::Warning);
        return;
    }

    const Devolucion &dev = m_devoluciones.at(row);

    QDialog dlg(this);
    dlg.setWindowTitle(QString("Evaluar Devolución #%1").arg(dev.id_devolucion));
    dlg.setMinimumWidth(450);

    auto *dlgLayout = new QVBoxLayout(&dlg);
    dlgLayout->setSpacing(14);

    auto *infoLabel = new QLabel(QString(
        "<b>Proveedor:</b> %1<br>"
        "<b>Piezas Totales:</b> %2<br>"
        "<b>Estado Actual:</b> %3<br>"
        "<b>Vendedor solicitante:</b> %4"
    ).arg(dev.nombre_proveedor.isEmpty() ? QString("ID %1").arg(dev.id_proveedor) : dev.nombre_proveedor)
     .arg(dev.total_piezas)
     .arg(dev.estado)
     .arg(dev.vendedor_nombre), &dlg);
    dlgLayout->addWidget(infoLabel);

    auto *lblNotas = new QLabel("Notas de Evaluación / Justificación:", &dlg);
    lblNotas->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    dlgLayout->addWidget(lblNotas);

    auto *txtNotas = new QTextEdit(&dlg);
    txtNotas->setPlaceholderText("Ingresa observaciones adicionales para el vendedor o proveedor...");
    txtNotas->setFixedHeight(80);
    dlgLayout->addWidget(txtNotas);

    auto *btnLayout = new QHBoxLayout();
    auto *btnAprobar = new QPushButton("✔ Autorizar Devolución", &dlg);
    btnAprobar->setStyleSheet(Theme::buttonStyle(Theme::Success, Theme::BgDark, "#72D4A0"));

    auto *btnRechazar = new QPushButton("✖ Rechazar Devolución", &dlg);
    btnRechazar->setStyleSheet(Theme::dangerButtonStyle());

    btnLayout->addWidget(btnAprobar);
    btnLayout->addWidget(btnRechazar);
    dlgLayout->addLayout(btnLayout);

    connect(btnAprobar, &QPushButton::clicked, [&]() {
        ApiClient::instance()->aprobarDevolucion(dev.id_devolucion, true, txtNotas->toPlainText().trimmed(),
            [this, &dlg](bool ok, const QJsonValue &, const QString &msg) {
                if (ok) {
                    Toast::showToast(this, "Devolución aprobada exitosamente", Toast::Success);
                    dlg.accept();
                    cargarDevoluciones();
                } else {
                    Toast::showToast(&dlg, "Error al aprobar: " + msg, Toast::Error);
                }
            });
    });

    connect(btnRechazar, &QPushButton::clicked, [&]() {
        ApiClient::instance()->aprobarDevolucion(dev.id_devolucion, false, txtNotas->toPlainText().trimmed(),
            [this, &dlg](bool ok, const QJsonValue &, const QString &msg) {
                if (ok) {
                    Toast::showToast(this, "Devolución rechazada", Toast::Warning);
                    dlg.accept();
                    cargarDevoluciones();
                } else {
                    Toast::showToast(&dlg, "Error al rechazar: " + msg, Toast::Error);
                }
            });
    });

    dlg.exec();
}
