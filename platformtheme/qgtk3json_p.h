// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QGTK3JSON_P_H
#define QGTK3JSON_P_H

#include <QtCore/QCache>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtGui/QGuiApplication>
#include <QtGui/QPalette>

#include <qpa/qplatformtheme.h>
#include "qgtk3interface_p.h"
#include "qgtk3storage_p.h"
#include "qgtk3colorscheme_p.h"

#undef signals // Collides with GTK symbols
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class QGtk3Json
{
    Q_GADGET
private:
    QGtk3Json(){};

public:
    // Convert enums to strings
    static QLatin1String fromPalette(QPlatformTheme::Palette palette);
    static QLatin1String fromGtkState(GtkStateFlags type);
    static QLatin1String fromColor(const QColor &Color);
    static QLatin1String fromColorRole(QPalette::ColorRole role);
    static QLatin1String fromColorGroup(QPalette::ColorGroup group);
    static QLatin1String fromGdkSource(QGtk3Interface::QGtkColorSource source);
    static QLatin1String fromSourceType(QGtk3Storage::SourceType sourceType);
    static QLatin1String fromWidgetType(QGtk3Interface::QGtkWidget widgetType);
    static QLatin1String fromColorScheme(QGtk3::ColorScheme colorScheme);

    // Convert strings to enums
    static QPlatformTheme::Palette toPalette(const QString &palette);
    static GtkStateFlags toGtkState(const QString &type);
    static QColor toColor(const QString &Color);
    static QPalette::ColorRole toColorRole(const QString &role);
    static QPalette::ColorGroup toColorGroup(const QString &group);
    static QGtk3Interface::QGtkColorSource toGdkSource(const QString &source);
    static QGtk3Storage::SourceType toSourceType(const QString &sourceType);
    static QGtk3Interface::QGtkWidget toWidgetType(const QString &widgetType);
    static QGtk3::ColorScheme toColorScheme(const QString &colorScheme);

    // Json keys
    static constexpr QLatin1String cePalettes  = QLatin1String("QtGtk3Palettes"); // View
    static constexpr QLatin1String cePalette  = QLatin1String("PaletteType"); // View
    static constexpr QLatin1String ceGtkState  = QLatin1String("GtkStateType"); // View
    static constexpr QLatin1String ceGtkWidget  = QLatin1String("GtkWidgetType"); // View
    static constexpr QLatin1String ceColor  = QLatin1String("Color"); // View
    static constexpr QLatin1String ceColorRole  = QLatin1String("ColorRole"); // View
    static constexpr QLatin1String ceColorGroup  = QLatin1String("ColorGroup"); // View
    static constexpr QLatin1String ceGdkSource  = QLatin1String("GdkSource"); // View
    static constexpr QLatin1String ceSourceType  = QLatin1String("SourceType"); // View
    static constexpr QLatin1String ceLighter  = QLatin1String("Lighter"); // View
    static constexpr QLatin1String ceRed  = QLatin1String("DeltaRed"); // View
    static constexpr QLatin1String ceGreen  = QLatin1String("DeltaGreen"); // View
    static constexpr QLatin1String ceBlue  = QLatin1String( "DeltaBlue"); // View
    static constexpr QLatin1String ceWidth  = QLatin1String("Width"); // View
    static constexpr QLatin1String ceHeight  = QLatin1String("Height"); // View
    static constexpr QLatin1String ceBrush  = QLatin1String("FixedBrush"); // View
    static constexpr QLatin1String ceData  = QLatin1String("SourceData"); // View
    static constexpr QLatin1String ceBrushes  = QLatin1String("Brushes"); // View
    static constexpr QLatin1String ceColorScheme  = QLatin1String("ColorScheme"); // View

    // Save to a file
    static bool save(const QGtk3Storage::PaletteMap &map, const QString &fileName,
              QJsonDocument::JsonFormat format = QJsonDocument::Indented);

    // Save to a Json document
    static const QJsonDocument save(const QGtk3Storage::PaletteMap &map);

    // Load from a file
    static bool load(QGtk3Storage::PaletteMap &map, const QString &fileName);

    // Load from a Json document
    static bool load(QGtk3Storage::PaletteMap &map, const QJsonDocument &doc);
};

QT_END_NAMESPACE
#endif // QGTK3JSON_P_H
