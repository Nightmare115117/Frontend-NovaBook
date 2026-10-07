#pragma once

#include <QString>
#include <QLabel>
#include <QComboBox>
#include "Theme.h"

namespace EanClassifier {

enum class Tipo {
    Desconocido,
    Libro,
    Revista
};

struct Resultado {
    Tipo tipo = Tipo::Desconocido;
    bool esValido = false;
    QString mensaje;
    QString colorEstilo;
    QString prefijo;
    int digitoCalculado = -1;
    int digitoReal = -1;
};

inline Resultado clasificar(const QString &rawEan) {
    Resultado res;
    QString ean = rawEan.trimmed();

    if (ean.isEmpty()) {
        res.tipo = Tipo::Desconocido;
        res.esValido = false;
        res.mensaje = "Ingresa el código EAN-13 para clasificar automáticamente como Libro o Revista";
        res.colorEstilo = Theme::TextMuted;
        return res;
    }

    bool todosDigitos = (ean.length() == 13);
    if (todosDigitos) {
        for (const QChar &ch : ean) {
            if (!ch.isDigit()) {
                todosDigitos = false;
                break;
            }
        }
    }

    if (!todosDigitos) {
        res.tipo = Tipo::Desconocido;
        res.esValido = false;
        res.mensaje = "Ingresa 13 dígitos numéricos exactos para clasificación automática";
        res.colorEstilo = Theme::TextMuted;
        return res;
    }

    // Cálculo y verificación del dígito verificador EAN-13 (Módulo 10 ponderado)
    int sumImpar = 0;
    int sumPar = 0;
    for (int i = 0; i < 12; ++i) {
        int d = ean[i].digitValue();
        if (i % 2 == 0) {
            sumImpar += d;
        } else {
            sumPar += d;
        }
    }
    int total = sumImpar + (sumPar * 3);
    int checkDigit = (10 - (total % 10)) % 10;
    int lastDigit = ean[12].digitValue();

    res.digitoCalculado = checkDigit;
    res.digitoReal = lastDigit;

    if (checkDigit != lastDigit) {
        res.tipo = Tipo::Desconocido;
        res.esValido = false;
        res.mensaje = QString("❌ Dígito verificador inválido: calculado %1 pero el código termina en %2")
                          .arg(checkDigit).arg(lastDigit);
        res.colorEstilo = Theme::Error;
        return res;
    }

    res.prefijo = ean.left(3);

    // Autoclasificación por prefijo estándar GS1
    if (ean.startsWith("978") || ean.startsWith("979")) {
        res.tipo = Tipo::Libro;
        res.esValido = true;
        res.mensaje = "✔ DETECTADO AUTOMÁTICAMENTE: LIBRO (ISBN-13 — Prefijo " + res.prefijo + ")";
        res.colorEstilo = Theme::Success;
    } else if (ean.startsWith("977")) {
        res.tipo = Tipo::Revista;
        res.esValido = true;
        res.mensaje = "✔ DETECTADO AUTOMÁTICAMENTE: REVISTA (ISSN-13 — Prefijo 977)";
        res.colorEstilo = "#7BB9D9"; // Azul claro característico
    } else {
        res.tipo = Tipo::Desconocido;
        res.esValido = false;
        res.mensaje = QString("❌ Código '%1' rechazado: Solo se admiten libros (978/979) o revistas (977)")
                          .arg(res.prefijo);
        res.colorEstilo = Theme::Error;
    }

    return res;
}

// Función auxiliar para actualizar automáticamente el label de estado y el combo de tipo
inline void aplicarAControles(const QString &ean, QLabel *lblStatus, QComboBox *comboTipo = nullptr,
                              const QString &valorLibro = "libro", const QString &valorRevista = "revista") {
    Resultado r = clasificar(ean);

    if (lblStatus) {
        lblStatus->setText(r.mensaje);
        lblStatus->setStyleSheet(QString("font-size: 12px; color: %1; font-weight: %2;")
                                     .arg(r.colorEstilo, r.esValido || r.colorEstilo == Theme::Error ? "bold" : "500"));
    }

    if (comboTipo && r.esValido) {
        if (r.tipo == Tipo::Libro) {
            int idx = comboTipo->findData(valorLibro);
            if (idx >= 0) {
                comboTipo->setCurrentIndex(idx);
            } else {
                int idxText = comboTipo->findText("Libro", Qt::MatchContains);
                if (idxText >= 0) comboTipo->setCurrentIndex(idxText);
            }
        } else if (r.tipo == Tipo::Revista) {
            int idx = comboTipo->findData(valorRevista);
            if (idx >= 0) {
                comboTipo->setCurrentIndex(idx);
            } else {
                int idxText = comboTipo->findText("Revista", Qt::MatchContains);
                if (idxText >= 0) comboTipo->setCurrentIndex(idxText);
            }
        }
    }
}

} // namespace EanClassifier
