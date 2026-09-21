#include <QApplication>
#include "Theme.h"
#include "LoginWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Application metadata
    QApplication::setApplicationName("NovaBook");
    QApplication::setOrganizationName("Sanborns");
    QApplication::setApplicationVersion("1.0.0");

    // Apply global dark stylesheet with custom colors
    app.setStyleSheet(Theme::globalStyleSheet());

    // Launch Login Window
    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}
