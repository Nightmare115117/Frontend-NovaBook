#include "StatCard.h"
#include "Theme.h"

StatCard::StatCard(const QString &title, const QString &value, const QString &subtext,
                   const QString &accentColor, QWidget *parent)
    : QFrame(parent)
{
    setObjectName("statCard");
    setStyleSheet(QString(R"(
        #statCard {
            background-color: %1;
            border: 1px solid %2;
            border-top: 3px solid %3;
            border-radius: 8px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border).arg(accentColor));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(6);

    m_lblTitle = new QLabel(title.toUpper(), this);
    m_lblTitle->setStyleSheet(QString("color: %1; font-size: 11px; font-weight: bold; letter-spacing: 0.5px;")
                               .arg(Theme::TextMuted));

    m_lblValue = new QLabel(value, this);
    m_lblValue->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: bold;")
                               .arg(Theme::TextPrimary));

    m_lblSubtext = new QLabel(subtext, this);
    m_lblSubtext->setStyleSheet(QString("color: %1; font-size: 11px;")
                                 .arg(Theme::TextMuted));

    layout->addWidget(m_lblTitle);
    layout->addWidget(m_lblValue);
    if (!subtext.isEmpty()) {
        layout->addWidget(m_lblSubtext);
    } else {
        m_lblSubtext->hide();
    }
}

void StatCard::setValue(const QString &value) {
    m_lblValue->setText(value);
}

void StatCard::setSubtext(const QString &subtext) {
    m_lblSubtext->setText(subtext);
    m_lblSubtext->setVisible(!subtext.isEmpty());
}
