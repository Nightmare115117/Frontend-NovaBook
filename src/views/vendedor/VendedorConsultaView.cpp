#include "VendedorConsultaView.h"
#include "Theme.h"
#include "ApiClient.h"
#include "Toast.h"
#include "Models.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>

VendedorConsultaView::VendedorConsultaView(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    auto *title = new QLabel("Consulta de Existencias y Disponibilidad", this);
    title->setStyleSheet(QString("font-size: 22px; font-weight: bold; color: %1;").arg(Theme::TextPrimary));
    layout->addWidget(title);

    // Toolbar
    auto *toolbar = new QHBoxLayout();
    toolbar->setSpacing(12);

    m_txtBuscar = new QLineEdit(this);
    m_txtBuscar->setPlaceholderText("Buscar por título, autor, código EAN o SKU...");
    toolbar->addWidget(m_txtBuscar, 2);

    m_comboTipo = new QComboBox(this);
    m_comboTipo->addItem("Todos los Tipos", "");
    m_comboTipo->addItem("Libros", "libro");
    m_comboTipo->addItem("Revistas", "revista");
    toolbar->addWidget(m_comboTipo);

    m_comboUbicacion = new QComboBox(this);
    m_comboUbicacion->addItem("Toda la Sucursal", 0);
    m_comboUbicacion->addItem("Solo Piso de Tienda", 1);
    m_comboUbicacion->addItem("Solo Almacén / Bodega", 2);
    toolbar->addWidget(m_comboUbicacion);

    m_btnBuscar = new QPushButton("Buscar", this);
    m_btnBuscar->setStyleSheet(Theme::buttonStyle(Theme::Role3, Theme::BgDark, "#A5D67D"));
    toolbar->addWidget(m_btnBuscar);

    m_btnLimpiar = new QPushButton("Restablecer", this);
    m_btnLimpiar->setStyleSheet(Theme::secondaryButtonStyle());
    toolbar->addWidget(m_btnLimpiar);

    layout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(9);
    m_table->setHorizontalHeaderLabels({"EAN", "SKU", "Título", "Tipo", "Autor / Editorial", "Géneros", "Stock Tienda", "Stock Bodega", "Precio"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_table);

    connect(m_btnBuscar, &QPushButton::clicked, this, &VendedorConsultaView::buscarExistencias);
    connect(m_txtBuscar, &QLineEdit::returnPressed, this, &VendedorConsultaView::buscarExistencias);
    connect(m_comboTipo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VendedorConsultaView::buscarExistencias);
    connect(m_comboUbicacion, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VendedorConsultaView::buscarExistencias);
    connect(m_btnLimpiar, &QPushButton::clicked, this, [this]() {
        m_txtBuscar->clear();
        m_comboTipo->setCurrentIndex(0);
        m_comboUbicacion->setCurrentIndex(0);
        buscarExistencias();
    });
}

void VendedorConsultaView::buscarExistencias() {
    QString q = m_txtBuscar->text().trimmed();
    QString tipo = m_comboTipo->currentData().toString();
    int ubicacion = m_comboUbicacion->currentData().toInt();

    ApiClient::instance()->consultarExistencias(q, tipo, ubicacion, [this](bool ok, const QJsonValue &data, const QString &msg) {
        if (!ok || !data.isArray()) {
            Toast::showToast(this, "Error al buscar: " + msg, Toast::Error);
            return;
        }

        QJsonArray arr = data.toArray();
        m_table->setRowCount(arr.size());

        for (int i = 0; i < arr.size(); ++i) {
            ExistenciaInventario ex = ExistenciaInventario::fromJson(arr.at(i).toObject());
            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(ex.codigo_ean)));
            m_table->setItem(i, 1, new QTableWidgetItem(ex.sku > 0 ? QString::number(ex.sku) : "-"));
            m_table->setItem(i, 2, new QTableWidgetItem(ex.titulo));
            m_table->setItem(i, 3, new QTableWidgetItem(ex.tipo_producto));
            m_table->setItem(i, 4, new QTableWidgetItem(ex.autor_o_editorial.isEmpty() ? "-" : ex.autor_o_editorial));
            m_table->setItem(i, 5, new QTableWidgetItem(ex.generos.isEmpty() ? "General" : ex.generos));

            auto *itemTienda = new QTableWidgetItem(QString::number(ex.stock_tienda));
            itemTienda->setForeground(QColor(Theme::Role3));
            m_table->setItem(i, 6, itemTienda);

            auto *itemBodega = new QTableWidgetItem(QString::number(ex.stock_bodega));
            itemBodega->setForeground(QColor(Theme::Role2));
            m_table->setItem(i, 7, itemBodega);

            m_table->setItem(i, 8, new QTableWidgetItem(QString("$ %1").arg(QString::number(ex.precio, 'f', 2))));
        }
    });
}
