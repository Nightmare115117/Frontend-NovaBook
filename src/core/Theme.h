#pragma once

#include <QString>
#include <QColor>

namespace Theme {
    // Exact colors requested by user
    inline const QString BgDark       = "#0D0D0D";
    inline const QString BgCard       = "#161616";
    inline const QString BgInput      = "#1F1F1F";
    inline const QString BgSidebar    = "#111111";
    inline const QString BgRowAlt     = "#1A1A1A";
    inline const QString Accent       = "#C8973A";
    inline const QString AccentHover  = "#E0AE52";
    inline const QString AccentDim    = "#7A5A1E";
    inline const QString TextPrimary  = "#F0EDE8";
    inline const QString TextMuted    = "#7A7570";
    inline const QString TextSidebar  = "#A09890";
    inline const QString Border       = "#2A2A2A";
    inline const QString BorderFocus  = "#C8973A";
    inline const QString Error        = "#E05C5C";
    inline const QString ErrorBg      = "#2A1010";
    inline const QString Success      = "#5CBF8A";
    inline const QString SuccessBg    = "#0F2A1A";
    inline const QString Warning      = "#E0B84A";
    inline const QString Role1        = "#C8973A"; // Jefe — dorado
    inline const QString Role2        = "#5C9FBF"; // Bodega — azul
    inline const QString Role3        = "#8ABF5C"; // Vendedor — verde

    QString globalStyleSheet();
    QString buttonStyle(const QString &bg = Accent, const QString &fg = BgDark, const QString &hover = AccentHover);
    QString secondaryButtonStyle();
    QString dangerButtonStyle();
    QString roleBadgeStyle(int roleId);
    QString roleColor(int roleId);
    QString roleName(int roleId);
}
