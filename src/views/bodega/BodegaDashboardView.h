#pragma once

#include <QWidget>

class StatCard;

class BodegaDashboardView : public QWidget {
    Q_OBJECT

public:
    explicit BodegaDashboardView(QWidget *parent = nullptr);
    void refreshData();

private:
    StatCard *m_cardStockBodega;
    StatCard *m_cardTitulos;
};
