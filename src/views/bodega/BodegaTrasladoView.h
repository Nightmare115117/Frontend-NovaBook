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
    void onEanChanged(const QString &text);
    void onTrasladar();

private:
    QLineEdit *m_txtEan;
    QLabel *m_lblEanStatus;
    QSpinBox *m_spnCantidad;
    QLineEdit *m_txtObs;
    QPushButton *m_btnTrasladar;

    // Result Card
    QFrame *m_resCard;
    QLabel *m_lblResMensaje;
    QLabel *m_lblResBodega;
    QLabel *m_lblResTienda;
};
