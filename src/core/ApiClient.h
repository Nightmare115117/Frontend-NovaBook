#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <functional>
#include "Models.h"

class ApiClient : public QObject {
    Q_OBJECT

public:
    using ApiCallback = std::function<void(bool success, const QJsonValue &data, const QString &message)>;
    using PdfCallback = std::function<void(bool success, const QByteArray &pdfBytes, const QString &filename)>;

    static ApiClient* instance();

    void setBaseUrl(const QString &url);
    QString baseUrl() const;

    void setToken(const QString &token);
    QString token() const;
    bool isAuthenticated() const;
    void logout();

    void setCurrentUser(const UsuarioDto &user);
    UsuarioDto currentUser() const;

    // --- Autenticación ---
    void login(qint64 id_usuario, const QString &contrasena, ApiCallback cb);
    void getPerfil(ApiCallback cb);

    // --- CRUD Usuarios (Jefe / Gerente) ---
    void listarUsuarios(ApiCallback cb);
    void obtenerUsuario(qint64 id, ApiCallback cb);
    void crearUsuario(const QJsonObject &userObj, ApiCallback cb);
    void actualizarUsuario(qint64 id, const QJsonObject &userObj, ApiCallback cb);
    void eliminarUsuario(qint64 id, ApiCallback cb);

    // --- Personal de Bodega ---
    void registrarLibroBodega(const QJsonObject &libroObj, ApiCallback cb);
    void registrarRevistaBodega(const QJsonObject &revistaObj, ApiCallback cb);
    void trasladoBodegaLibros(qint64 ean, int cant, const QString &obs, ApiCallback cb);
    void trasladoBodegaRevistas(qint64 ean, int cant, const QString &obs, ApiCallback cb);

    // --- Vendedor ---
    void consultarExistencias(const QString &q, const QString &tipo, int ubicacion, ApiCallback cb);
    void registrarVenta(const QJsonArray &items, ApiCallback cb);
    void trasladoTiendaLibros(qint64 ean, int cant, const QString &obs, ApiCallback cb);
    void trasladoTiendaRevistas(qint64 ean, int cant, const QString &obs, ApiCallback cb);
    void crearDevolucion(int idProveedor, const QString &tipo, const QJsonArray &items, ApiCallback cb);
    void descargarPdfDevolucion(qint64 idDevolucion, const QString &tipo, PdfCallback cb);

    // --- Jefe de Departamento ---
    void consultarHistorialDevoluciones(const QString &fecha, const QString &estado, int idProveedor, ApiCallback cb);
    void consultarMovimientoDiario(const QString &fecha, ApiCallback cb);
    void aprobarDevolucion(qint64 idDevolucion, bool aprobar, const QString &notas, ApiCallback cb);

signals:
    void sessionExpired();
    void userLoggedIn(const UsuarioDto &user);
    void userLoggedOut();

private:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient() override = default;

    void sendRequest(const QString &verb, const QString &endpoint, const QUrlQuery &query,
                     const QByteArray &body, ApiCallback cb);

    QNetworkAccessManager *m_nam;
    QString m_baseUrl;
    QString m_token;
    UsuarioDto m_currentUser;
};
