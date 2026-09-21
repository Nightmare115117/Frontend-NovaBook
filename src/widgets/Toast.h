#pragma once

#include <QWidget>
#include <QLabel>
#include <QTimer>

class Toast : public QWidget {
    Q_OBJECT

public:
    enum Type {
        Success,
        Error,
        Warning,
        Info
    };

    static void showToast(QWidget *parent, const QString &message, Type type = Info, int durationMs = 3500);

private:
    explicit Toast(QWidget *parent, const QString &message, Type type);
    QLabel *m_label;
    QTimer *m_timer;
};
