#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include "Models.h"

struct ReturnItemDraft {
    qint64 ean = 0;
    QString tipo = "Libro";
    int cantidad = 1;
    QString motivo;
};

class VendedorDevolucionView : public QWidget {
    Q_OBJECT

public:
    explicit VendedorDevolucionView(QWidget *parent = nullptr);

private slots:
    void onEanChanged(const QString &text);
    void onAgregarArticulo();
    void onQuitarArticulo(int index);
    void onEnviarDevolucion();
    void onDescargarPdf();

private:
    QComboBox *m_comboProveedor;

    QLineEdit *m_txtEan;
    QLabel *m_lblEanStatus;
    QSpinBox *m_spnCantidad;
    QLineEdit *m_txtMotivo;
    QPushButton *m_btnAgregar;

    QTableWidget *m_tableItems;
    QPushButton *m_btnEnviar;

    // Result Card with PDF download
    QFrame *m_resCard;
    QLabel *m_lblResStatus;
    QPushButton *m_btnDescargarPdf;
    qint64 m_lastDevolucionId = 0;
    QString m_lastTipo;

    QList<ReturnItemDraft> m_items;
};
