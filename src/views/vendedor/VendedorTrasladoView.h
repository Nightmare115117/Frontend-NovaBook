#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

class VendedorTrasladoView : public QWidget {
    Q_OBJECT

public:
    explicit VendedorTrasladoView(QWidget *parent = nullptr);

private slots:
    void onTrasladar();

private:
    QComboBox *m_comboTipo;
    QLineEdit *m_txtEan;
    QSpinBox *m_spnCantidad;
    QLineEdit *m_txtObs;
    QPushButton *m_btnTrasladar;

    QFrame *m_resCard;
    QLabel *m_lblResMensaje;
    QLabel *m_lblResTienda;
    QLabel *m_lblResBodega;
};
