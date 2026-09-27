#include "LoginWindow.h"
#include "MainWindow.h"
#include "Theme.h"
#include "ApiClient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QIcon>
#include <QGuiApplication>
#include <QScreen>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("NovaBook — Iniciar Sesión");
    setFixedSize(480, 700);
    setAttribute(Qt::WA_QuitOnClose, true);

    // Center on screen
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Accent top bar
    auto *topBar = new QFrame(this);
    topBar->setFixedHeight(4);
    topBar->setStyleSheet(QString("background-color: %1;").arg(Theme::Accent));
    mainLayout->addWidget(topBar);

    auto *contentWidget = new QWidget(this);
    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 32, 40, 32);
    contentLayout->setSpacing(16);

    // Logo NOVA BOOK
    auto *logoLayout = new QHBoxLayout();
    logoLayout->setAlignment(Qt::AlignCenter);

    auto *lblNova = new QLabel("NOVA", contentWidget);
    lblNova->setStyleSheet(QString("font-size: 32px; font-weight: 900; color: %1; letter-spacing: 2px;").arg(Theme::TextPrimary));

    auto *lblBook = new QLabel("BOOK", contentWidget);
    lblBook->setStyleSheet(QString("font-size: 32px; font-weight: 900; color: %1; letter-spacing: 2px;").arg(Theme::Accent));

    logoLayout->addWidget(lblNova);
    logoLayout->addWidget(lblBook);
    contentLayout->addLayout(logoLayout);

    auto *lblSub = new QLabel("Sistema de Gestión de Librería", contentWidget);
    lblSub->setAlignment(Qt::AlignCenter);
    lblSub->setStyleSheet(QString("font-size: 11px; color: %1; letter-spacing: 0.5px;").arg(Theme::TextMuted));
    contentLayout->addWidget(lblSub);

    auto *lblLoc = new QLabel("Sanborns · Saltillo", contentWidget);
    lblLoc->setAlignment(Qt::AlignCenter);
    lblLoc->setStyleSheet(QString("font-size: 10px; color: %1; margin-bottom: 8px;").arg(Theme::AccentDim));
    contentLayout->addWidget(lblLoc);

    // Card Box
    auto *card = new QFrame(contentWidget);
    card->setStyleSheet(QString(R"(
        QFrame {
            background-color: %1;
            border: 1px solid %2;
            border-radius: 8px;
        }
    )").arg(Theme::BgCard).arg(Theme::Border));

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 24, 24, 24);
    cardLayout->setSpacing(12);

    auto *lblCardTitle = new QLabel("INICIAR SESIÓN", card);
    lblCardTitle->setStyleSheet(QString("font-size: 11px; font-weight: bold; color: %1; letter-spacing: 1px;").arg(Theme::TextMuted));
    cardLayout->addWidget(lblCardTitle);

    auto *lblUser = new QLabel("ID USUARIO (NUMÉRICO)", card);
    lblUser->setStyleSheet(QString("font-size: 10px; font-weight: bold; color: %1;").arg(Theme::TextMuted));
    cardLayout->addWidget(lblUser);

    m_txtUsuario = new QLineEdit(card);
    m_txtUsuario->setPlaceholderText("Ej. 350976899");
    cardLayout->addWidget(m_txtUsuario);

    auto *lblPass = new QLabel("CONTRASEÑA", card);
    lblPass->setStyleSheet(QString("font-size: 10px; font-weight: bold; color: %1;").arg(Theme::TextMuted));
    cardLayout->addWidget(lblPass);

    m_txtPassword = new QLineEdit(card);
    m_txtPassword->setEchoMode(QLineEdit::Password);
    m_txtPassword->setPlaceholderText("••••••••");
    cardLayout->addWidget(m_txtPassword);

    auto *lblUrl = new QLabel("SERVIDOR API", card);
    lblUrl->setStyleSheet(QString("font-size: 10px; font-weight: bold; color: %1;").arg(Theme::TextMuted));
    cardLayout->addWidget(lblUrl);

    m_txtServerUrl = new QLineEdit("http://127.0.0.1:3000", card);
    m_txtServerUrl->setStyleSheet("font-size: 11px;");
    cardLayout->addWidget(m_txtServerUrl);

    m_lblError = new QLabel(card);
    m_lblError->setStyleSheet(QString("color: %1; font-size: 11px;").arg(Theme::Error));
    m_lblError->setWordWrap(true);
    m_lblError->hide();
    cardLayout->addWidget(m_lblError);

    m_btnLogin = new QPushButton("INGRESAR AL SISTEMA", card);
    m_btnLogin->setStyleSheet(Theme::buttonStyle());
    m_btnLogin->setCursor(Qt::PointingHandCursor);
    cardLayout->addWidget(m_btnLogin);

    contentLayout->addWidget(card);

    // Quick Test Buttons
    auto *lblQuick = new QLabel("ACCESOS RÁPIDOS DE PRUEBA", contentWidget);
    lblQuick->setStyleSheet(QString("font-size: 10px; font-weight: bold; color: %1; letter-spacing: 0.5px; margin-top: 4px;").arg(Theme::TextMuted));
    lblQuick->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(lblQuick);

    auto *quickLayout = new QGridLayout();
    quickLayout->setSpacing(8);

    auto addQuickBtn = [&](const QString &label, qint64 id, const QString &pass, int row, int col, const QString &color) {
        auto *btn = new QPushButton(label, contentWidget);
        btn->setStyleSheet(QString(R"(
            QPushButton {
                background-color: %1;
                color: %2;
                border: 1px solid %3;
                border-radius: 6px;
                padding: 6px 4px;
                font-size: 11px;
                font-weight: bold;
            }
            QPushButton:hover {
                border-color: %4;
                color: %4;
                background-color: #1A1A1A;
            }
        )").arg(Theme::BgInput).arg(Theme::TextSidebar).arg(Theme::Border).arg(color));
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, id, pass]() {
            m_txtUsuario->setText(QString::number(id));
            m_txtPassword->setText(pass);
            onLoginClicked();
        });
        quickLayout->addWidget(btn, row, col);
    };

    addQuickBtn("👑 Gerente (1001)", 1001, "admin123", 0, 0, Theme::Role1);
    addQuickBtn("👔 Jefe (350976899)", 350976899, "2501", 0, 1, Theme::Role1);
    addQuickBtn("📦 Bodega (628777130)", 628777130, "7777", 1, 0, Theme::Role2);
    addQuickBtn("🏷️ Vendedor (628777129)", 628777129, "8888", 1, 1, Theme::Role3);

    contentLayout->addLayout(quickLayout);

    // Role Indicator Pills
    auto *rolesLayout = new QHBoxLayout();
    rolesLayout->setAlignment(Qt::AlignCenter);
    rolesLayout->setSpacing(12);

    struct RoleInfo { QString color; QString name; };
    QList<RoleInfo> roles = {
        {Theme::Role1, "Gerente"},
        {Theme::Role1, "Jefe"},
        {Theme::Role2, "Bodega"},
        {Theme::Role3, "Vendedor"}
    };

    for (const auto &r : roles) {
        auto *dot = new QLabel("●", contentWidget);
        dot->setStyleSheet(QString("color: %1; font-size: 11px;").arg(r.color));
        auto *text = new QLabel(r.name, contentWidget);
        text->setStyleSheet(QString("color: %1; font-size: 10px;").arg(Theme::TextMuted));

        rolesLayout->addWidget(dot);
        rolesLayout->addWidget(text);
    }
    contentLayout->addLayout(rolesLayout);

    mainLayout->addWidget(contentWidget);

    connect(m_btnLogin, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_txtUsuario, &QLineEdit::returnPressed, this, [this]() { m_txtPassword->setFocus(); });
    connect(m_txtPassword, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
}

LoginWindow::~LoginWindow() {
    delete m_mainWindow;
}

void LoginWindow::onLoginClicked() {
    m_lblError->hide();
    QString idStr = m_txtUsuario->text().trimmed();
    QString pass = m_txtPassword->text();

    bool okNum = false;
    qint64 id = idStr.toLongLong(&okNum);

    if (!okNum || id <= 0) {
        m_lblError->setText("⚠ El ID de usuario debe ser numérico.");
        m_lblError->show();
        return;
    }

    if (pass.isEmpty()) {
        m_lblError->setText("⚠ Ingresa tu contraseña.");
        m_lblError->show();
        return;
    }

    m_btnLogin->setEnabled(false);
    m_btnLogin->setText("CONECTANDO...");

    ApiClient::instance()->setBaseUrl(m_txtServerUrl->text());
    ApiClient::instance()->login(id, pass, [this](bool success, const QJsonValue &, const QString &msg) {
        m_btnLogin->setEnabled(true);
        m_btnLogin->setText("INGRESAR AL SISTEMA");

        if (!success) {
            m_lblError->setText("⚠ " + msg);
            m_lblError->show();
            return;
        }

        // Open MainWindow
        hide();
        if (!m_mainWindow) {
            m_mainWindow = new MainWindow();
            connect(m_mainWindow, &MainWindow::logoutRequested, this, &LoginWindow::onLogoutRequested);
        }
        m_mainWindow->initUser(ApiClient::instance()->currentUser());
        m_mainWindow->show();
    });
}

void LoginWindow::onLogoutRequested() {
    if (m_mainWindow) {
        m_mainWindow->hide();
    }
    m_txtPassword->clear();
    m_lblError->hide();
    show();
    m_txtUsuario->setFocus();
}
