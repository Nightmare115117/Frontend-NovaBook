#include "Theme.h"

namespace Theme {

QString globalStyleSheet() {
    return QString(R"(
        /* Global Reset */
        QWidget {
            background-color: %1;
            color: %2;
            font-family: 'Segoe UI', 'DejaVu Sans', sans-serif;
            font-size: 13px;
        }

        /* QMainWindow and Containers */
        QMainWindow, QDialog {
            background-color: %1;
        }

        /* Labels */
        QLabel {
            background-color: transparent;
            color: %2;
        }

        /* Text Inputs */
        QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            border-radius: 6px;
            padding: 8px 12px;
            selection-background-color: %5;
            selection-color: %1;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, 
        QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {
            border: 1px solid %6;
        }

        QLineEdit:disabled, QSpinBox:disabled, QDoubleSpinBox:disabled, QComboBox:disabled {
            background-color: #141414;
            color: %7;
            border: 1px solid #222222;
        }

        /* Combo Box */
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }

        QComboBox QAbstractItemView {
            background-color: %8;
            color: %2;
            border: 1px solid %4;
            selection-background-color: %5;
            selection-color: %1;
            outline: none;
            padding: 4px;
        }

        /* Push Buttons (Default) */
        QPushButton {
            background-color: %5;
            color: %1;
            font-weight: bold;
            border: none;
            border-radius: 6px;
            padding: 9px 18px;
            min-height: 20px;
        }

        QPushButton:hover {
            background-color: %9;
        }

        QPushButton:pressed {
            background-color: %10;
        }

        QPushButton:disabled {
            background-color: #262626;
            color: %7;
        }

        /* Tables & Item Views */
        QTableWidget, QTableView, QListWidget, QListView, QTreeWidget, QTreeView {
            background-color: %8;
            alternate-background-color: %11;
            color: %2;
            border: 1px solid %4;
            border-radius: 6px;
            gridline-color: %4;
            selection-background-color: #2A2415;
            selection-color: %2;
            outline: none;
        }

        QTableWidget::item, QTableView::item {
            padding: 8px 10px;
            border-bottom: 1px solid #1F1F1F;
        }

        QTableWidget::item:selected, QTableView::item:selected {
            background-color: #2D2513;
            color: %2;
            border-left: 2px solid %5;
        }

        QHeaderView::section {
            background-color: %12;
            color: %7;
            font-weight: bold;
            font-size: 11px;
            padding: 10px 8px;
            border: none;
            border-bottom: 2px solid %4;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            background: %1;
            width: 10px;
            margin: 0px;
        }

        QScrollBar::handle:vertical {
            background: %4;
            min-height: 20px;
            border-radius: 5px;
        }

        QScrollBar::handle:vertical:hover {
            background: %10;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background: %1;
            height: 10px;
            margin: 0px;
        }

        QScrollBar::handle:horizontal {
            background: %4;
            min-width: 20px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal:hover {
            background: %10;
        }

        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }

        /* Tabs / Stacked */
        QTabWidget::pane {
            border: 1px solid %4;
            background-color: %8;
            border-radius: 6px;
        }

        QTabBar::tab {
            background-color: %12;
            color: %7;
            padding: 8px 16px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            margin-right: 2px;
        }

        QTabBar::tab:selected {
            background-color: %8;
            color: %5;
            border-bottom: 2px solid %5;
            font-weight: bold;
        }

        /* GroupBox */
        QGroupBox {
            background-color: %8;
            border: 1px solid %4;
            border-radius: 6px;
            margin-top: 14px;
            padding: 14px;
            font-weight: bold;
            color: %2;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 6px;
            color: %5;
        }

        /* Tooltip */
        QToolTip {
            background-color: %8;
            color: %2;
            border: 1px solid %4;
            padding: 4px 8px;
            border-radius: 4px;
        }
    )")
    .arg(BgDark)        // %1
    .arg(TextPrimary)   // %2
    .arg(BgInput)       // %3
    .arg(Border)        // %4
    .arg(Accent)        // %5
    .arg(BorderFocus)   // %6
    .arg(TextMuted)     // %7
    .arg(BgCard)        // %8
    .arg(AccentHover)   // %9
    .arg(AccentDim)     // %10
    .arg(BgRowAlt)      // %11
    .arg(BgSidebar);    // %12
}

QString buttonStyle(const QString &bg, const QString &fg, const QString &hover) {
    return QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            font-weight: bold;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: %3;
        }
    )").arg(bg).arg(fg).arg(hover);
}

QString secondaryButtonStyle() {
    return QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            font-weight: 500;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #252525;
            border-color: %4;
            color: %5;
        }
    )").arg(BgCard).arg(TextPrimary).arg(Border).arg(Accent).arg(TextPrimary);
}

QString dangerButtonStyle() {
    return QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            font-weight: bold;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: %3;
            color: #FFFFFF;
        }
    )").arg(ErrorBg).arg(Error).arg(Error);
}

QString roleColor(int roleId) {
    switch (roleId) {
        case 1: case 4: return Role1; // Jefe / Gerente -> Dorado
        case 2: return Role2;         // Bodega -> Azul
        case 3: return Role3;         // Vendedor -> Verde
        default: return Accent;
    }
}

QString roleName(int roleId) {
    switch (roleId) {
        case 1: return "Jefe de Departamento";
        case 4: return "Gerente General";
        case 2: return "Personal de Bodega";
        case 3: return "Vendedor";
        default: return "Usuario";
    }
}

QString roleBadgeStyle(int roleId) {
    QString color = roleColor(roleId);
    return QString(R"(
        QLabel {
            background-color: rgba(0, 0, 0, 0.4);
            color: %1;
            border: 1px solid %1;
            border-radius: 10px;
            padding: 3px 10px;
            font-size: 11px;
            font-weight: bold;
        }
    )").arg(color);
}

} // namespace Theme
