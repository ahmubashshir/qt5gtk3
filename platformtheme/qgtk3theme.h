// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGTK3THEME_H
#define QGTK3THEME_H

#include <private/qtguiglobal_p.h>
#include <private/qgenericunixthemes_p.h>
#include "qgtk3storage_p.h"
#include "qgtk3colorscheme_p.h"

QT_BEGIN_NAMESPACE

class Qt5Gtk3Theme : public QGnomeTheme
{
public:
    Qt5Gtk3Theme();

    virtual QVariant themeHint(ThemeHint hint) const override;
    virtual QString gtkFontName() const override;

    Qt5Gtk3::ColorScheme colorScheme() const;

    bool usePlatformNativeDialog(DialogType type) const override;
    QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const override;

    const QPalette *palette(Palette type = SystemPalette) const override;
    const QFont *font(Font type = SystemFont) const override;
    QPixmap standardPixmap(StandardPixmap sp, const QSizeF &size) const override;
    QIcon fileIcon(const QFileInfo &fileInfo,
                           QPlatformTheme::IconOptions iconOptions = { }) const override;

    static const char *name;
private:
    static bool useNativeFileDialog();
    std::unique_ptr<Qt5Gtk3Storage> m_storage;
};

QT_END_NAMESPACE

#endif // QGTK3THEME_H
