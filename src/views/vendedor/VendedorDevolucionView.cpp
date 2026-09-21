#include "VendedorDevolucionView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>

VendedorDevolucionView::VendedorDevolucionView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    auto *title = new QLabel("Gestión de Devoluciones a Proveedor", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    auto *subtitle = new QLabel("Registra artículos para devolución por merma, defectos o acuerdo comercial con descarga de acta PDF", this);
    subtitle->setStyleSheet(QString("font-size: 13px; color: %1;").arg(Theme::TextMuted));
    layout->addWidget(title);
    layout->addWidget(subtitle);

    // Form Header Card
    auto *headerCard = new QFrame(this);
    headerCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 14px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setSpacing(16);

    auto *lblProv = new QLabel("Proveedor Destino:", headerCard);
    lblProv->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    headerLayout->addWidget(lblProv);

    m_comboProveedor = new QComboBox(headerCard);
    m_comboProveedor->addItem("Planeta México (1)", 1);
    m_comboProveedor->addItem("Planeta México Infantil (2)", 2);
    m_comboProveedor->addItem("Penguin Random House (3)", 3);
    headerLayout->addWidget(m_comboProveedor, 1);

    auto *lblTipo = new QLabel("Tipo de Material:", headerCard);
    lblTipo->setStyleSheet(QString("font-weight: bold; color: %1;").arg(Theme::TextMuted));
    headerLayout->addWidget(lblTipo);

    m_comboTipo = new QComboBox(headerCard);
    m_comboTipo->addItem("Libro", "Libro");
    m_comboTipo->addItem("Revista", "Revista");
    headerLayout->addWidget(m_comboTipo, 1);

    layout->addWidget(headerCard);

    // Add item card
    auto *addItemCard = new QFrame(this);
    addItemCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
            padding: 14px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *addItemLayout = new QHBoxLayout(addItemCard);
    addItemLayout->setSpacing(10);

    m_txtEan = new QLineEdit(addItemCard);
    m_txtEan->setPlaceholderText("Código EAN (13 dígitos)...");
    addItemLayout->addWidget(m_txtEan, 2);

    auto *lblCant = new QLabel("Cant:", addItemCard);
    addItemLayout->addWidget(lblCant);

    m_spnCantidad = new QSpinBox(addItemCard);
    m_spnCantidad->setRange(1, 999);
    m_spnCantidad->setValue(1);
    addItemLayout->addWidget(m_spnCantidad);

    m_txtMotivo = new QLineEdit(addItemCard);
    m_txtMotivo->setPlaceholderText("Motivo (ej. Portada dañada, defecto editorial)...");
    addItemLayout->addWidget(m_txtMotivo, 3);

    m_btnAgregar = new QPushButton("＋ Añadir Pieza", addItemCard);
    m_btnAgregar->setStyleSheet(Theme::buttonStyle(Theme::Role3, Theme::BgDark, "#A5D67D"));
    addItemLayout->addWidget(m_btnAgregar);

    layout->addWidget(addItemCard);

    // Items table
    m_tableItems = new QTableWidget(this);
    m_tableItems->setColumnCount(4);
    m_tableItems->setHorizontalHeaderLabels({"EAN", "Cantidad", "Motivo de Devolución", "Acción"});
    m_tableItems->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableItems->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableItems->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_tableItems->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableItems->verticalHeader()->setVisible(false);
    m_tableItems->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableItems->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_tableItems);

    m_btnEnviar = new QPushButton("✔ Registrar Solicitud de Devolución", this);
    m_btnEnviar->setStyleSheet(Theme::buttonStyle(Theme::Accent, Theme::BgDark, Theme::AccentHover));
    layout->addWidget(m_btnEnviar);

    // Result Card with PDF download
    m_resCard = new QFrame(this);
    m_resCard->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-left: 4px solid %3;
            border-radius: 8px;
            padding: 16px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border).arg(Theme::Warning));
    m_resCard->hide();

    auto *resLayout = new QHBoxLayout(m_resCard);
    m_lblResStatus = new QLabel(m_resCard);
    m_lblResStatus->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 13px;").arg(Theme::TextPrimary));
    resLayout->addWidget(m_lblResStatus, 1);

    m_btnDescargarPdf = new QPushButton("📄 Descargar Comprobante PDF", m_resCard);
    m_btnDescargarPdf->setStyleSheet(Theme::buttonStyle(Theme::Accent, Theme::BgDark, Theme::AccentHover));
    resLayout->addWidget(m_btnDescargarPdf);

    layout->addWidget(m_resCard);

    connect(m_btnAgregar, &QPushButton::clicked, this, &VendedorDevolucionView::onAgregarArticulo);
    connect(m_btnEnviar, &QPushButton::clicked, this, &VendedorDevolucionView::onEnviarDevolucion);
    connect(m_btnDescargarPdf, &QPushButton::clicked, this, &VendedorDevolucionView::onDescargarPdf);
}

void VendedorDevolucionView::onAgregarArticulo() {
    QString eanStr = m_txtEan->text().trimmed();
    if (eanStr.isEmpty() || !eanStr.toLongLong()) {
        Toast::showToast(this, "Ingresa un código EAN válido", Toast::Warning);
        return;
    }

    ReturnItemDraft item;
    item.ean = eanStr.toLongLong();
    item.cantidad = m_spnCantidad->value();
    item.motivo = m_txtMotivo->text().trimmed();

    m_items.append(item);
    int row = m_items.size() - 1;
    m_tableItems->insertRow(row);

    m_tableItems->setItem(row, 0, new QTableWidgetItem(QString::number(item.ean)));
    m_tableItems->setItem(row, 1, new QTableWidgetItem(QString::number(item.cantidad)));
    m_tableItems->setItem(row, 2, new QTableWidgetItem(item.motivo.isEmpty() ? "Defecto general" : item.motivo));

    auto *btnQuitar = new QPushButton("✖ Quitar", this);
    btnQuitar->setStyleSheet(Theme::dangerButtonStyle());
    connect(btnQuitar, &QPushButton::clicked, this, [this, row]() {
        onQuitarArticulo(row);
    });
    m_tableItems->setCellWidget(row, 3, btnQuitar);

    m_txtEan->clear();
    m_txtMotivo->clear();
    m_spnCantidad->setValue(1);
    m_txtEan->setFocus();
}

void VendedorDevolucionView::onQuitarArticulo(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
        m_tableItems->removeRow(index);
    }
}

void VendedorDevolucionView::onEnviarDevolucion() {
    if (m_items.isEmpty()) {
        Toast::showToast(this, "Agrega al menos un artículo a devolver", Toast::Warning);
        return;
    }

    int provId = m_comboProveedor->currentData().toInt();
    QString tipo = m_comboTipo->currentData().toString();

    QJsonArray itemsArr;
    for (const auto &it : m_items) {
        QJsonObject o;
        o["codigo_ean"] = it.ean;
        o["cantidad"] = it.cantidad;
        if (!it.motivo.isEmpty()) o["motivo"] = it.motivo;
        itemsArr.append(o);
    }

    ApiClient::instance()->crearDevolucion(provId, tipo, itemsArr, [this, tipo](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isObject()) {
            Toast::showToast(this, "Error al registrar devolución: " + msg, Toast::Error);
            return;
        }

        Devolucion dev = Devolucion::fromJson(data.toObject());
        m_lastDevolucionId = dev.id_devolucion;
        m_lastTipo = tipo;

        m_resCard->show();
        m_lblResStatus->setText(QString("✔ Solicitud de Devolución #%1 registrada con éxito.<br>"
                                        "Estado: <b>%2</b> (Requiere autorización del Jefe)").arg(dev.id_devolucion).arg(dev.estado));

        m_items.clear();
        m_tableItems->setRowCount(0);
        Toast::showToast(this, "Devolución registrada exitosamente", Toast::Success);
    });
}

void VendedorDevolucionView::onDescargarPdf() {
    if (m_lastDevolucionId <= 0) return;

    QString defaultName = QString("devolucion_%1_%2.pdf").arg(m_lastDevolucionId).arg(m_lastTipo.toLower());
    QString savePath = QFileDialog::getSaveFileName(this, "Guardar Acta de Devolución en PDF", defaultName, "Archivos PDF (*.pdf)");
    if (savePath.isEmpty()) return;

    ApiClient::instance()->descargarPdfDevolucion(m_lastDevolucionId, m_lastTipo, [this, savePath](bool ok, const QByteArray &bytes, const QString &msg) {
        if (!ok || bytes.isEmpty()) {
            Toast::showToast(this, "Error al descargar PDF: " + msg, Toast::Error);
            return;
        }

        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(bytes);
            file.close();
            Toast::showToast(this, "Comprobante PDF guardado exitosamente", Toast::Success);
        } else {
            Toast::showToast(this, "No se pudo guardar el archivo en disco", Toast::Error);
        }
    });
}
