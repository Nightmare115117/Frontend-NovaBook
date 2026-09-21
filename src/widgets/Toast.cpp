#include "Toast.h"
#include "Theme.h"
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

Toast::Toast(QWidget *parent, const QString &message, Type type)
    : QWidget(parent)
{
    QString bgColor, textColor, borderColor, iconPrefix;
    switch (type) {
        case Success:
            bgColor = Theme::SuccessBg;
            textColor = Theme::Success;
            borderColor = Theme::Success;
            iconPrefix = "✔ ";
            break;
        case Error:
            bgColor = Theme::ErrorBg;
            textColor = Theme::Error;
            borderColor = Theme::Error;
            iconPrefix = "✖ ";
            break;
        case Warning:
            bgColor = "#2A2510";
            textColor = Theme::Warning;
            borderColor = Theme::Warning;
            iconPrefix = "⚠ ";
            break;
        case Info:
        default:
            bgColor = Theme::BgCard;
            textColor = Theme::Accent;
            borderColor = Theme::Accent;
            iconPrefix = "ℹ ";
            break;
    }

    setObjectName("toastWidget");
    setStyleSheet(QString(R"(
        #toastWidget {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
        }
        QLabel {
            color: %3;
            font-weight: 600;
            font-size: 12px;
        }
    )").arg(bgColor).arg(borderColor).arg(textColor));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 10, 16, 10);
    m_label = new QLabel(iconPrefix + message, this);
    m_label->setWordWrap(true);
    layout->addWidget(m_label);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        auto *effect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(effect);
        auto *anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(300);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, this, &QWidget::deleteLater);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void Toast::showToast(QWidget *parent, const QString &message, Type type, int durationMs) {
    if (!parent) return;

    auto *toast = new Toast(parent, message, type);
    toast->adjustSize();

    // Position at top center of parent widget
    int x = (parent->width() - toast->width()) / 2;
    int y = 20;
    toast->move(qMax(10, x), y);
    toast->show();
    toast->raise();

    toast->m_timer->start(durationMs);
}
