#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

struct UsuarioDto {
    qint64 id_usuarios = 0;
    int id_roles = 0;
    QString rol_nombre;
    QString nombre;
    QString apellido_paterno;
    QString apellido_materno;
    qint64 telefono = 0;

    QString nombreCompleto() const {
        QString full = nombre;
        if (!apellido_paterno.isEmpty()) full += " " + apellido_paterno;
        if (!apellido_materno.isEmpty()) full += " " + apellido_materno;
        return full.trimmed();
    }

    static UsuarioDto fromJson(const QJsonObject &obj) {
        UsuarioDto u;
        u.id_usuarios = obj.value("id_usuarios").toVariant().toLongLong();
        u.id_roles = obj.value("id_roles").toInt();
        u.rol_nombre = obj.value("rol_nombre").toString();
        u.nombre = obj.value("nombre").toString();
        u.apellido_paterno = obj.value("apellido_paterno").toString();
        u.apellido_materno = obj.value("apellido_materno").toString();
        u.telefono = obj.value("telefono").toVariant().toLongLong();
        return u;
    }
};

struct LoginResponse {
    QString token;
    QString token_type;
    qint64 expires_in_hours = 0;
    UsuarioDto usuario;

    static LoginResponse fromJson(const QJsonObject &obj) {
        LoginResponse r;
        r.token = obj.value("token").toString();
        r.token_type = obj.value("token_type").toString();
        r.expires_in_hours = obj.value("expires_in_hours").toVariant().toLongLong();
        if (obj.contains("usuario") && obj.value("usuario").isObject()) {
            r.usuario = UsuarioDto::fromJson(obj.value("usuario").toObject());
        }
        return r;
    }
};

struct ExistenciaInventario {
    qint64 codigo_ean = 0;
    qint64 sku = 0;
    QString titulo;
    QString tipo_producto;
    int stock_tienda = 0;
    int stock_bodega = 0;
    int stock_total = 0;
    double precio = 0.0;
    QString proveedor;
    QString autor_o_editorial;

    static ExistenciaInventario fromJson(const QJsonObject &obj) {
        ExistenciaInventario e;
        e.codigo_ean = obj.value("codigo_ean").toVariant().toLongLong();
        e.sku = obj.value("sku").toVariant().toLongLong();
        e.titulo = obj.value("titulo").toString();
        e.tipo_producto = obj.value("tipo_producto").toString();
        e.stock_tienda = obj.value("stock_tienda").toInt();
        e.stock_bodega = obj.value("stock_bodega").toInt();
        e.stock_total = obj.value("stock_total").toInt();
        e.precio = obj.value("precio").toDouble();
        e.proveedor = obj.value("proveedor").toString();
        e.autor_o_editorial = obj.value("autor_o_editorial").toString();
        return e;
    }
};

struct ItemVenta {
    qint64 codigo_ean = 0;
    QString tipo_producto;
    QString nombre_producto;
    int cantidad = 0;
    double precio_unitario = 0.0;
    double subtotal = 0.0;

    static ItemVenta fromJson(const QJsonObject &obj) {
        ItemVenta iv;
        iv.codigo_ean = obj.value("codigo_ean").toVariant().toLongLong();
        iv.tipo_producto = obj.value("tipo_producto").toString();
        iv.nombre_producto = obj.value("nombre_producto").toString();
        iv.cantidad = obj.value("cantidad").toInt();
        iv.precio_unitario = obj.value("precio_unitario").toDouble();
        iv.subtotal = obj.value("subtotal").toDouble();
        return iv;
    }
};

struct VentaResponse {
    qint64 id_venta = 0;
    qint64 id_vendedor = 0;
    QString fecha_hora;
    double total = 0.0;
    int total_articulos = 0;
    QList<ItemVenta> items;
    QString mensaje;

    static VentaResponse fromJson(const QJsonObject &obj) {
        VentaResponse vr;
        vr.id_venta = obj.value("id_venta").toVariant().toLongLong();
        vr.id_vendedor = obj.value("id_vendedor").toVariant().toLongLong();
        vr.fecha_hora = obj.value("fecha_hora").toString();
        vr.total = obj.value("total").toDouble();
        vr.total_articulos = obj.value("total_articulos").toInt();
        vr.mensaje = obj.value("mensaje").toString();

        QJsonArray arr = obj.value("items").toArray();
        for (const auto &val : arr) {
            vr.items.append(ItemVenta::fromJson(val.toObject()));
        }
        return vr;
    }
};

struct TrasladoResponse {
    qint64 codigo_ean = 0;
    QString tipo_producto;
    QString tipo_movimiento;
    int cantidad_trasladada = 0;
    int stock_origen_restante = 0;
    int stock_destino_nuevo = 0;
    QString mensaje;

    static TrasladoResponse fromJson(const QJsonObject &obj) {
        TrasladoResponse tr;
        tr.codigo_ean = obj.value("codigo_ean").toVariant().toLongLong();
        tr.tipo_producto = obj.value("tipo_producto").toString();
        tr.tipo_movimiento = obj.value("tipo_movimiento").toString();
        tr.cantidad_trasladada = obj.value("cantidad_trasladada").toInt();
        tr.stock_origen_restante = obj.value("stock_origen_restante").toInt();
        tr.stock_destino_nuevo = obj.value("stock_destino_nuevo").toInt();
        tr.mensaje = obj.value("mensaje").toString();
        return tr;
    }
};

struct ItemDevolucion {
    qint64 codigo_ean = 0;
    qint64 sku = 0;
    QString titulo;
    int cantidad = 0;
    QString motivo;

    static ItemDevolucion fromJson(const QJsonObject &obj) {
        ItemDevolucion id;
        id.codigo_ean = obj.value("codigo_ean").toVariant().toLongLong();
        id.sku = obj.value("sku").toVariant().toLongLong();
        id.titulo = obj.value("titulo").toString();
        id.cantidad = obj.value("cantidad").toInt();
        id.motivo = obj.value("motivo").toString();
        return id;
    }
};

struct Devolucion {
    qint64 id_devolucion = 0;
    QString fecha;
    int total_piezas = 0;
    QString estado;
    QString autorizado_por;
    qint64 id_usuarios = 0;
    QString vendedor_nombre;
    int id_proveedor = 0;
    QString nombre_proveedor;
    QString tipo_producto;
    QList<ItemDevolucion> items;

    static Devolucion fromJson(const QJsonObject &obj) {
        Devolucion d;
        d.id_devolucion = obj.value("id_devolucion").toVariant().toLongLong();
        d.fecha = obj.value("fecha").toString();
        d.total_piezas = obj.value("total_piezas").toInt();
        d.estado = obj.value("estado").toString();
        d.autorizado_por = obj.value("autorizado_por").toString();
        d.id_usuarios = obj.value("id_usuarios").toVariant().toLongLong();
        d.vendedor_nombre = obj.value("vendedor_nombre").toString();
        d.id_proveedor = obj.value("id_proveedor").toInt();
        d.nombre_proveedor = obj.value("nombre_proveedor").toString();
        d.tipo_producto = obj.value("tipo_producto").toString();

        QJsonArray arr = obj.value("items").toArray();
        for (const auto &val : arr) {
            d.items.append(ItemDevolucion::fromJson(val.toObject()));
        }
        return d;
    }
};

struct MovimientoDiarioItem {
    QString fecha_hora;
    QString usuario;
    QString rol;
    QString accion;
    QString detalle;

    static MovimientoDiarioItem fromJson(const QJsonObject &obj) {
        MovimientoDiarioItem m;
        m.fecha_hora = obj.value("fecha_hora").toString();
        m.usuario = obj.value("usuario").toString();
        m.rol = obj.value("rol").toString();
        m.accion = obj.value("accion").toString();
        m.detalle = obj.value("detalle").toString();
        return m;
    }
};

struct ResumenMovimientoDiario {
    QString fecha;
    int total_eventos = 0;
    QList<MovimientoDiarioItem> movimientos;

    static ResumenMovimientoDiario fromJson(const QJsonObject &obj) {
        ResumenMovimientoDiario r;
        r.fecha = obj.value("fecha").toString();
        r.total_eventos = obj.value("total_eventos").toInt();

        QJsonArray arr = obj.value("movimientos").toArray();
        for (const auto &val : arr) {
            r.movimientos.append(MovimientoDiarioItem::fromJson(val.toObject()));
        }
        return r;
    }
};
