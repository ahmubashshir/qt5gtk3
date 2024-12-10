// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <qpa/qplatformthemeplugin.h>
#include <QLoggingCategory>
#include "qgtk3theme.h"

QT_BEGIN_NAMESPACE

class Qt5Gtk3ThemePlugin : public QPlatformThemePlugin
{
		Q_OBJECT
		Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "gtk3.json")

	public:
		QPlatformTheme *create(const QString &key, const QStringList &params) override;
};

QPlatformTheme *Qt5Gtk3ThemePlugin::create(const QString &key, const QStringList &params)
{
	Q_UNUSED(params);
	if (!key.compare(QLatin1String(Qt5Gtk3Theme::name), Qt::CaseInsensitive))
		return new Qt5Gtk3Theme;

	return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
