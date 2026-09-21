#pragma once

#include <QWidget>
#include "Models.h"

class StatCard;

class JefeDashboardView : public QWidget {
    Q_OBJECT

public:
    explicit JefeDashboardView(QWidget *parent = nullptr);
    void refreshData();

private:
    StatCard *m_cardMovimientos;
    StatCard *m_cardDevoluciones;
    StatCard *m_cardUsuarios;
};
