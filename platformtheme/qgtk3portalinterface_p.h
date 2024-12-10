// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGTK3PORTALINTERFACE_H
#define QGTK3PORTALINTERFACE_H

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

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>
#include "qgtk3colorscheme_p.h"

QT_BEGIN_NAMESPACE

class QDBusVariant;
class Qt5Gtk3Storage;

Q_DECLARE_LOGGING_CATEGORY(lcQt5Gtk3PortalInterface);

class Qt5Gtk3PortalInterface : public QObject
{
    Q_OBJECT
public:
    Qt5Gtk3PortalInterface(Qt5Gtk3Storage *s);
    ~Qt5Gtk3PortalInterface() = default;

    Qt5Gtk3::ColorScheme colorScheme() const;

private Q_SLOTS:
    void settingChanged(const QString &group, const QString &key,
                        const QDBusVariant &value);
private:
    void queryColorScheme();

    Qt5Gtk3::ColorScheme m_colorScheme = Qt5Gtk3::ColorScheme::Unknown;
    Qt5Gtk3Storage *m_storage = nullptr;
};

QT_END_NAMESPACE

#endif // QGTK3PORTALINTERFACE_H
