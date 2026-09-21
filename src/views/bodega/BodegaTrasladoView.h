#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

class BodegaTrasladoView : public QWidget {
    Q_OBJECT

public:
    explicit BodegaTrasladoView(QWidget *parent = nullptr);

private slots:
    void onTrasladar();

private:
    QComboBox *m_comboTipo;
    QLineEdit *m_txtEan;
    QSpinBox *m_spnCantidad;
    QLineEdit *m_txtObs;
    QPushButton *m_btnTrasladar;

    // Result Card
    QFrame *m_resCard;
    QLabel *m_lblResMensaje;
    QLabel *m_lblResBodega;
    QLabel *m_lblResTienda;
};
