#pragma once

#include <QWidget>

class StatCard;

class VendedorDashboardView : public QWidget {
    Q_OBJECT

public:
    explicit VendedorDashboardView(QWidget *parent = nullptr);
    void refreshData();

private:
    StatCard *m_cardStockTienda;
    StatCard *m_cardCatalogo;
};
