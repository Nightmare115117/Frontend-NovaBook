#include "ApiClient.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QDebug>

using namespace std;

static string loadApiUrl() {
    const char* envUrl = getenv("API_URL");
    if (envUrl && *envUrl != '\0') {
        return string(envUrl);
    }

    const QStringList envFileCandidates = {
        QDir::current().absoluteFilePath(".env"),
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../.env")
    };

    for (const QString &envFilePath : envFileCandidates) {
        QFile envFile(envFilePath);
        if (!envFile.open(QIODevice::ReadOnly)) {
            continue;
        }

        QTextStream stream(&envFile);
        while (!stream.atEnd()) {
            QString line = stream.readLine().trimmed();
            if (line.isEmpty() || line.startsWith('#')) {
                continue;
            }

            const int separator = line.indexOf('=');
            if (separator <= 0) {
                continue;
            }

            const QString key = line.left(separator).trimmed();
            QString value = line.mid(separator + 1).trimmed();
            if (value.startsWith('"') && value.endsWith('"')) {
                value = value.mid(1, value.length() - 2);
            }

            if (key == "API_URL" && !value.isEmpty()) {
                return value.toStdString();
            }
        }
    }

    return string("http://localhost:3000");
}

ApiClient* ApiClient::instance() {
    static ApiClient s_instance;
    return &s_instance;
}

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
    , m_baseUrl(QString::fromStdString(loadApiUrl()))
{
}

void ApiClient::setBaseUrl(const QString &url) {
    m_baseUrl = url.trimmed();
    while (m_baseUrl.endsWith('/')) {
        m_baseUrl.chop(1);
    }
}

QString ApiClient::baseUrl() const {
    return m_baseUrl;
}

void ApiClient::setToken(const QString &token) {
    m_token = token;
}

QString ApiClient::token() const {
    return m_token;
}

bool ApiClient::isAuthenticated() const {
    return !m_token.isEmpty();
}

void ApiClient::logout() {
    m_token.clear();
    m_currentUser = UsuarioDto();
    emit userLoggedOut();
}

void ApiClient::setCurrentUser(const UsuarioDto &user) {
    m_currentUser = user;
    emit userLoggedIn(user);
}

UsuarioDto ApiClient::currentUser() const {
    return m_currentUser;
}

void ApiClient::sendRequest(const QString &verb, const QString &endpoint, const QUrlQuery &query,
                            const QByteArray &body, ApiCallback cb) {
    QUrl url(m_baseUrl + endpoint);
    if (!query.isEmpty()) {
        url.setQuery(query);
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_token).toUtf8());
    }

    QNetworkReply *reply = nullptr;
    if (verb == "GET") {
        reply = m_nam->get(request);
    } else if (verb == "POST") {
        reply = m_nam->post(request, body);
    } else if (verb == "PUT") {
        reply = m_nam->put(request, body);
    } else if (verb == "DELETE") {
        reply = m_nam->deleteResource(request);
    } else {
        if (cb) cb(false, QJsonValue(), "Verbo HTTP no soportado");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [this, reply, cb]() {
        reply->deleteLater();
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray respData = reply->readAll();

        if (reply->error() != QNetworkReply::NoError && respData.isEmpty()) {
            if (cb) cb(false, QJsonValue(), QString("Error de red (%1): %2").arg(httpStatus).arg(reply->errorString()));
            return;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(respData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            if (cb) cb(false, QJsonValue(), QString("Respuesta no válida del servidor (%1)").arg(httpStatus));
            return;
        }

        QJsonObject root = doc.object();
        bool success = root.value("success").toBool(false);
        QString message = root.value("message").toString();

        if (!success) {
            if (root.contains("error") && root.value("error").isObject()) {
                QJsonObject errObj = root.value("error").toObject();
                message = errObj.value("message").toString(message);
            }
            if (httpStatus == 401) {
                emit sessionExpired();
            }
            if (message.isEmpty()) {
                message = QString("Error en la solicitud (Código HTTP %1)").arg(httpStatus);
            }
            if (cb) cb(false, root.value("data"), message);
            return;
        }

        if (cb) cb(true, root.value("data"), message);
    });
}

// --- Autenticación ---

void ApiClient::login(qint64 id_usuario, const QString &contrasena, ApiCallback cb) {
    QJsonObject req;
    req["id_usuario"] = id_usuario;
    req["contrasena"] = contrasena;

    sendRequest("POST", "/api/auth/login", QUrlQuery(), QJsonDocument(req).toJson(),
        [this, cb](bool success, const QJsonValue &data, const QString &message) {
            if (success && data.isObject()) {
                LoginResponse resp = LoginResponse::fromJson(data.toObject());
                this->setToken(resp.token);
                this->setCurrentUser(resp.usuario);
            }
            if (cb) cb(success, data, message);
        }
    );
}

void ApiClient::getPerfil(ApiCallback cb) {
    sendRequest("GET", "/api/auth/perfil", QUrlQuery(), QByteArray(), cb);
}

// --- CRUD Usuarios ---

void ApiClient::listarUsuarios(ApiCallback cb) {
    sendRequest("GET", "/api/usuarios", QUrlQuery(), QByteArray(), cb);
}

void ApiClient::obtenerUsuario(qint64 id, ApiCallback cb) {
    sendRequest("GET", QString("/api/usuarios/%1").arg(id), QUrlQuery(), QByteArray(), cb);
}

void ApiClient::crearUsuario(const QJsonObject &userObj, ApiCallback cb) {
    sendRequest("POST", "/api/usuarios", QUrlQuery(), QJsonDocument(userObj).toJson(), cb);
}

void ApiClient::actualizarUsuario(qint64 id, const QJsonObject &userObj, ApiCallback cb) {
    sendRequest("PUT", QString("/api/usuarios/%1").arg(id), QUrlQuery(), QJsonDocument(userObj).toJson(), cb);
}

void ApiClient::eliminarUsuario(qint64 id, ApiCallback cb) {
    sendRequest("DELETE", QString("/api/usuarios/%1").arg(id), QUrlQuery(), QByteArray(), cb);
}

// --- Catálogo de Géneros ---

void ApiClient::listarGeneros(ApiCallback cb) {
    sendRequest("GET", "/api/generos", QUrlQuery(), QByteArray(), cb);
}

// --- CRUD Autores ---

void ApiClient::listarAutores(ApiCallback cb) {
    sendRequest("GET", "/api/autores", QUrlQuery(), QByteArray(), cb);
}

void ApiClient::obtenerAutor(int id, ApiCallback cb) {
    sendRequest("GET", QString("/api/autores/%1").arg(id), QUrlQuery(), QByteArray(), cb);
}

void ApiClient::crearAutor(const QJsonObject &autorObj, ApiCallback cb) {
    sendRequest("POST", "/api/autores", QUrlQuery(), QJsonDocument(autorObj).toJson(), cb);
}

void ApiClient::actualizarAutor(int id, const QJsonObject &autorObj, ApiCallback cb) {
    sendRequest("PUT", QString("/api/autores/%1").arg(id), QUrlQuery(), QJsonDocument(autorObj).toJson(), cb);
}

void ApiClient::eliminarAutor(int id, ApiCallback cb) {
    sendRequest("DELETE", QString("/api/autores/%1").arg(id), QUrlQuery(), QByteArray(), cb);
}

// --- CRUD Proveedores ---

void ApiClient::listarProveedores(ApiCallback cb) {
    sendRequest("GET", "/api/proveedores", QUrlQuery(), QByteArray(), cb);
}

void ApiClient::obtenerProveedor(int id, ApiCallback cb) {
    sendRequest("GET", QString("/api/proveedores/%1").arg(id), QUrlQuery(), QByteArray(), cb);
}

void ApiClient::crearProveedor(const QJsonObject &provObj, ApiCallback cb) {
    sendRequest("POST", "/api/proveedores", QUrlQuery(), QJsonDocument(provObj).toJson(), cb);
}

void ApiClient::actualizarProveedor(int id, const QJsonObject &provObj, ApiCallback cb) {
    sendRequest("PUT", QString("/api/proveedores/%1").arg(id), QUrlQuery(), QJsonDocument(provObj).toJson(), cb);
}

void ApiClient::eliminarProveedor(int id, ApiCallback cb) {
    sendRequest("DELETE", QString("/api/proveedores/%1").arg(id), QUrlQuery(), QByteArray(), cb);
}

// --- Personal de Bodega ---

void ApiClient::registrarLibroBodega(const QJsonObject &libroObj, ApiCallback cb) {
    sendRequest("POST", "/api/bodega/libros", QUrlQuery(), QJsonDocument(libroObj).toJson(), cb);
}

void ApiClient::registrarRevistaBodega(const QJsonObject &revistaObj, ApiCallback cb) {
    sendRequest("POST", "/api/bodega/revistas", QUrlQuery(), QJsonDocument(revistaObj).toJson(), cb);
}

void ApiClient::trasladoBodegaLibros(qint64 ean, int cant, const QString &obs, ApiCallback cb) {
    QJsonObject req;
    req["codigo_ean"] = ean;
    req["cantidad"] = cant;
    if (!obs.isEmpty()) req["observaciones"] = obs;
    sendRequest("POST", "/api/bodega/movimientos/libros", QUrlQuery(), QJsonDocument(req).toJson(), cb);
}

void ApiClient::trasladoBodegaRevistas(qint64 ean, int cant, const QString &obs, ApiCallback cb) {
    QJsonObject req;
    req["codigo_ean"] = ean;
    req["cantidad"] = cant;
    if (!obs.isEmpty()) req["observaciones"] = obs;
    sendRequest("POST", "/api/bodega/movimientos/revistas", QUrlQuery(), QJsonDocument(req).toJson(), cb);
}

void ApiClient::registrarCompra(const QJsonObject &compraObj, ApiCallback cb) {
    sendRequest("POST", "/api/bodega/compras", QUrlQuery(), QJsonDocument(compraObj).toJson(), cb);
}

void ApiClient::listarCompras(ApiCallback cb) {
    sendRequest("GET", "/api/bodega/compras", QUrlQuery(), QByteArray(), cb);
}

// --- Vendedor ---

void ApiClient::consultarExistencias(const QString &q, const QString &tipo, int ubicacion, ApiCallback cb) {
    QUrlQuery query;
    if (!q.isEmpty()) query.addQueryItem("q", q);
    if (!tipo.isEmpty()) query.addQueryItem("tipo", tipo);
    if (ubicacion > 0) query.addQueryItem("ubicacion", QString::number(ubicacion));

    sendRequest("GET", "/api/vendedor/existencias", query, QByteArray(), cb);
}

void ApiClient::registrarVenta(const QString &cliente, const QJsonArray &items, ApiCallback cb) {
    QJsonObject req;
    req["cliente"] = cliente;
    req["items"] = items;
    sendRequest("POST", "/api/vendedor/ventas", QUrlQuery(), QJsonDocument(req).toJson(), cb);
}

void ApiClient::registrarVenta(const QJsonArray &items, ApiCallback cb) {
    registrarVenta("Público en General", items, cb);
}

void ApiClient::trasladoTiendaLibros(qint64 ean, int cant, const QString &obs, ApiCallback cb) {
    QJsonObject req;
    req["codigo_ean"] = ean;
    req["cantidad"] = cant;
    if (!obs.isEmpty()) req["observaciones"] = obs;
    sendRequest("POST", "/api/vendedor/movimientos/libros", QUrlQuery(), QJsonDocument(req).toJson(), cb);
}

void ApiClient::trasladoTiendaRevistas(qint64 ean, int cant, const QString &obs, ApiCallback cb) {
    QJsonObject req;
    req["codigo_ean"] = ean;
    req["cantidad"] = cant;
    if (!obs.isEmpty()) req["observaciones"] = obs;
    sendRequest("POST", "/api/vendedor/movimientos/revistas", QUrlQuery(), QJsonDocument(req).toJson(), cb);
}

void ApiClient::crearDevolucion(int idProveedor, const QString &tipo, const QJsonArray &items, ApiCallback cb) {
    QJsonObject req;
    req["id_proveedor"] = idProveedor;
    req["tipo_producto"] = tipo;
    req["items"] = items;
    sendRequest("POST", "/api/vendedor/devoluciones", QUrlQuery(), QJsonDocument(req).toJson(), cb);
}

void ApiClient::descargarPdfDevolucion(qint64 idDevolucion, const QString &tipo, PdfCallback cb) {
    QString subpath = (tipo.toLower() == "revista") ? "revistas" : "libros";
    QUrl url(QString("%1/api/vendedor/devoluciones/%2/pdf/%3").arg(m_baseUrl).arg(idDevolucion).arg(subpath));

    QNetworkRequest request(url);
    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_token).toUtf8());
    }

    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, cb]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            if (cb) cb(false, QByteArray(), reply->errorString());
            return;
        }

        QString disposition = reply->rawHeader("Content-Disposition");
        QString filename = "devolucion.pdf";
        if (disposition.contains("filename=")) {
            filename = disposition.section("filename=", 1, 1).replace("\"", "").trimmed();
        }

        QByteArray data = reply->readAll();
        if (cb) cb(true, data, filename);
    });
}

// --- Jefe de Departamento ---

void ApiClient::consultarHistorialDevoluciones(const QString &fecha, const QString &estado, int idProveedor, ApiCallback cb) {
    QUrlQuery query;
    if (!fecha.isEmpty()) query.addQueryItem("fecha", fecha);
    if (!estado.isEmpty() && estado != "Todos") query.addQueryItem("estado", estado);
    if (idProveedor > 0) query.addQueryItem("id_proveedor", QString::number(idProveedor));

    sendRequest("GET", "/api/jefe/devoluciones/historial", query, QByteArray(), cb);
}

void ApiClient::consultarMovimientoDiario(const QString &fecha, ApiCallback cb) {
    QUrlQuery query;
    if (!fecha.isEmpty()) query.addQueryItem("fecha", fecha);

    sendRequest("GET", "/api/jefe/movimientos/diarios", query, QByteArray(), cb);
}

void ApiClient::aprobarDevolucion(qint64 idDevolucion, bool aprobar, const QString &notas, ApiCallback cb) {
    QJsonObject req;
    req["aprobar"] = aprobar;
    if (!notas.isEmpty()) req["notas"] = notas;

    sendRequest("PUT", QString("/api/jefe/devoluciones/%1/aprobar").arg(idDevolucion),
                QUrlQuery(), QJsonDocument(req).toJson(), cb);
}
