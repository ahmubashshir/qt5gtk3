// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#include "qgtk3json_p.h"
#include <QtCore/QFile>
#include <QMetaEnum>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

QLatin1String Qt5Gtk3Json::fromPalette(QPlatformTheme::Palette palette)
{
	return QLatin1String(QByteArray(QString::number(palette).toLatin1()));
}

QLatin1String Qt5Gtk3Json::fromGtkState(GtkStateFlags state)
{
	return Qt5Gtk3Interface::fromGtkState(state);
}

QLatin1String fromColor(const QColor &color)
{
	return QLatin1String(QByteArray(color.name(QColor::HexRgb).toLatin1()));
}

QLatin1String Qt5Gtk3Json::fromColorRole(QPalette::ColorRole role)
{
	return QLatin1String(QMetaEnum::fromType<QPalette::ColorRole>().valueToKey(static_cast<int>(role)));
}

QLatin1String Qt5Gtk3Json::fromColorGroup(QPalette::ColorGroup group)
{
	return QLatin1String(QMetaEnum::fromType<QPalette::ColorGroup>().valueToKey(static_cast<int>(group)));
}

QLatin1String Qt5Gtk3Json::fromGdkSource(Qt5Gtk3Interface::QGtkColorSource source)
{
	return QLatin1String(QMetaEnum::fromType<Qt5Gtk3Interface::QGtkColorSource>().valueToKey(static_cast<int>(source)));
}

QLatin1String Qt5Gtk3Json::fromWidgetType(Qt5Gtk3Interface::QGtkWidget widgetType)
{
	return QLatin1String(QMetaEnum::fromType<Qt5Gtk3Interface::QGtkWidget>().valueToKey(static_cast<int>(widgetType)));
}

QLatin1String Qt5Gtk3Json::fromColorScheme(Qt5Gtk3::ColorScheme app)
{
	return QLatin1String(QMetaEnum::fromType<Qt5Gtk3::ColorScheme>().valueToKey(static_cast<int>(app)));
}

#define CONVERT(type, key, def)\
    bool ok;\
    const int intVal = QMetaEnum::fromType<type>().keyToValue(key.toLatin1().constData(), &ok);\
    return ok ? static_cast<type>(intVal) : type::def

Qt5Gtk3::ColorScheme Qt5Gtk3Json::toColorScheme(const QString &colorScheme)
{
	CONVERT(Qt5Gtk3::ColorScheme, colorScheme, Unknown);
}

QPlatformTheme::Palette Qt5Gtk3Json::toPalette(const QString &palette)
{
	using qp = QPlatformTheme::Palette;
	return qBound(qp::SystemPalette, static_cast<qp>(palette.toInt()), qp::NPalettes);
}

GtkStateFlags Qt5Gtk3Json::toGtkState(const QString &type)
{
	int i = Qt5Gtk3Interface::toGtkState(type);
	if (i < 0)
		return GTK_STATE_FLAG_NORMAL;
	return static_cast<GtkStateFlags>(i);
}

QColor toColor(const QStringView &color)
{
	return QColor(color);
}

QPalette::ColorRole Qt5Gtk3Json::toColorRole(const QString &role)
{
	CONVERT(QPalette::ColorRole, role, NColorRoles);
}

QPalette::ColorGroup Qt5Gtk3Json::toColorGroup(const QString &group)
{
	CONVERT(QPalette::ColorGroup, group, NColorGroups);
}

Qt5Gtk3Interface::QGtkColorSource Qt5Gtk3Json::toGdkSource(const QString &source)
{
	CONVERT(Qt5Gtk3Interface::QGtkColorSource, source, Background);
}

QLatin1String Qt5Gtk3Json::fromSourceType(Qt5Gtk3Storage::SourceType sourceType)
{
	return QLatin1String(QMetaEnum::fromType<Qt5Gtk3Storage::SourceType>().valueToKey(static_cast<int>(sourceType)));
}

Qt5Gtk3Storage::SourceType Qt5Gtk3Json::toSourceType(const QString &sourceType)
{
	CONVERT(Qt5Gtk3Storage::SourceType, sourceType, Invalid);
}

Qt5Gtk3Interface::QGtkWidget Qt5Gtk3Json::toWidgetType(const QString &widgetType)
{
	CONVERT(Qt5Gtk3Interface::QGtkWidget, widgetType, gtk_offscreen_window);
}

#undef CONVERT

bool Qt5Gtk3Json::save(const Qt5Gtk3Storage::PaletteMap &map, const QString &fileName,
                       QJsonDocument::JsonFormat format)
{
	QJsonDocument doc = save(map);
	if (doc.isEmpty()) {
		qWarning() << "Nothing to save to" << fileName;
		return false;
	}

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning() << "Unable to open file" << fileName << "for writing.";
		return false;
	}

	if (!file.write(doc.toJson(format))) {
		qWarning() << "Unable to serialize Json document.";
		return false;
	}

	file.close();
	qInfo() << "Saved mapping data to" << fileName;
	return true;
}

const QJsonDocument Qt5Gtk3Json::save(const Qt5Gtk3Storage::PaletteMap &map)
{
	QJsonObject paletteObject;
	for (auto paletteIterator = map.constBegin(); paletteIterator != map.constEnd();
	     ++paletteIterator) {
		const Qt5Gtk3Storage::BrushMap &bm = paletteIterator.value();
		QFlatMap<QPalette::ColorRole, Qt5Gtk3Storage::BrushMap> brushMaps;
		for (auto brushIterator = bm.constBegin(); brushIterator != bm.constEnd();
		     ++brushIterator) {
			const QPalette::ColorRole role = brushIterator.key().colorRole;
			if (brushMaps.contains(role)) {
				brushMaps[role].insert(brushIterator.key(), brushIterator.value());
			} else {
				Qt5Gtk3Storage::BrushMap newMap;
				newMap.insert(brushIterator.key(), brushIterator.value());
				brushMaps.insert(role, newMap);
			}
		}

		QJsonObject brushArrayObject;
		for (auto brushMapIterator = brushMaps.constBegin();
		     brushMapIterator != brushMaps.constEnd(); ++brushMapIterator) {

			QJsonArray brushArray;
			int brushIndex = 0;
			const Qt5Gtk3Storage::BrushMap &bm = brushMapIterator.value();
			for (auto brushIterator = bm.constBegin(); brushIterator != bm.constEnd();
			     ++brushIterator) {
				QJsonObject brushObject;
				const Qt5Gtk3Storage::TargetBrush tb = brushIterator.key();
				Qt5Gtk3Storage::Source s = brushIterator.value();
				brushObject.insert(ceColorGroup, fromColorGroup(tb.colorGroup));
				brushObject.insert(ceColorScheme, fromColorScheme(tb.colorScheme));
				brushObject.insert(ceSourceType, fromSourceType(s.sourceType));

				QJsonObject sourceObject;
				switch (s.sourceType) {
				case Qt5Gtk3Storage::SourceType::Gtk: {
					sourceObject.insert(ceGtkWidget, fromWidgetType(s.gtk3.gtkWidgetType));
					sourceObject.insert(ceGdkSource, fromGdkSource(s.gtk3.source));
					sourceObject.insert(ceGtkState, fromGtkState(s.gtk3.state));
					sourceObject.insert(ceWidth, s.gtk3.width);
					sourceObject.insert(ceHeight, s.gtk3.height);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Fixed: {
					QJsonObject fixedObject;
					fixedObject.insert(ceColor, s.fix.fixedBrush.color().name());
					fixedObject.insert(ceWidth, s.fix.fixedBrush.texture().width());
					fixedObject.insert(ceHeight, s.fix.fixedBrush.texture().height());
					sourceObject.insert(ceBrush, fixedObject);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Modified: {
					sourceObject.insert(ceColorGroup, fromColorGroup(s.rec.colorGroup));
					sourceObject.insert(ceColorRole, fromColorRole(s.rec.colorRole));
					sourceObject.insert(ceColorScheme, fromColorScheme(s.rec.colorScheme));
					sourceObject.insert(ceRed, s.rec.deltaRed);
					sourceObject.insert(ceGreen, s.rec.deltaGreen);
					sourceObject.insert(ceBlue, s.rec.deltaBlue);
					sourceObject.insert(ceWidth, s.rec.width);
					sourceObject.insert(ceHeight, s.rec.height);
					sourceObject.insert(ceLighter, s.rec.lighter);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Mixed: {
					sourceObject.insert(ceColorGroup, fromColorGroup(s.mix.sourceGroup));
					QJsonArray colorRoles;
					colorRoles << fromColorRole(s.mix.colorRole1)
					           << fromColorRole(s.mix.colorRole2);
					sourceObject.insert(ceColorRole, colorRoles);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Invalid:
					break;
				}

				brushObject.insert(ceData, sourceObject);
				brushArray.insert(brushIndex, brushObject);
				++brushIndex;
			}
			brushArrayObject.insert(fromColorRole(brushMapIterator.key()), brushArray);
		}
		paletteObject.insert(fromPalette(paletteIterator.key()), brushArrayObject);
	}

	QJsonObject top;
	top.insert(cePalettes, paletteObject);
	return paletteObject.keys().isEmpty() ? QJsonDocument() : QJsonDocument(top);
}

bool Qt5Gtk3Json::load(Qt5Gtk3Storage::PaletteMap &map, const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		qCWarning(lcQt5Gtk3Interface) << "Unable to open file:" << fileName;
		return false;
	}

	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
	if (err.error != QJsonParseError::NoError) {
		qCWarning(lcQt5Gtk3Interface) << "Unable to parse Json document from" << fileName
		                              << err.error << err.errorString();
		return false;
	}

	if (Q_LIKELY(load(map, doc))) {
		qInfo() << "GTK mapping successfully imported from" << fileName;
		return true;
	}

	qWarning() << "File" << fileName << "could not be loaded.";
	return false;
}

bool Qt5Gtk3Json::load(Qt5Gtk3Storage::PaletteMap &map, const QJsonDocument &doc)
{
#define GETSTR(obj, key)\
    if (!obj.contains(key)) {\
        qCInfo(lcQt5Gtk3Interface) << key << "missing for palette" << paletteName\
                                  << ", Brush" << colorRoleName;\
        return false;\
    }\
    value = obj[key].toString()

#define GETINT(obj, key, var) GETSTR(obj, key);\
    if (!obj[key].isDouble()) {\
        qCInfo(lcQt5Gtk3Interface) << key << "type mismatch" << value\
                                  << "is not an integer!"\
                                  << "(Palette" << paletteName << "), Brush" << colorRoleName;\
        return false;\
    }\
    const int var = obj[key].toInt()

	map.clear();
	const QJsonObject top(doc.object());
	if (doc.isEmpty() || top.isEmpty() || !top.contains(cePalettes)) {
		qCInfo(lcQt5Gtk3Interface) << "Document does not contain Palettes.";
		return false;
	}

	const QStringList &paletteList = top[cePalettes].toObject().keys();
	for (const QString &paletteName : paletteList) {
		bool ok;
		using qp = QPlatformTheme::Palette;
		const qp intVal = qBound(qp::SystemPalette, static_cast<qp>(paletteName.toInt()), qp::NPalettes);
		if (intVal == qp::NPalettes) {
			qCInfo(lcQt5Gtk3Interface) << "Invalid Palette name:" << paletteName;
			return false;
		}
		const QJsonObject &paletteObject = top[cePalettes][paletteName].toObject();
		const QStringList &brushList = paletteObject.keys();
		if (brushList.isEmpty()) {
			qCInfo(lcQt5Gtk3Interface) << "Palette" << paletteName << "does not contain brushes";
			return false;
		}

		const QPlatformTheme::Palette paletteType = static_cast<QPlatformTheme::Palette>(intVal);
		Qt5Gtk3Storage::BrushMap brushes;
		const QStringList &colorRoles = paletteObject.keys();
		for (const QString &colorRoleName : colorRoles) {
			const int intVal = QMetaEnum::fromType<QPalette::ColorRole>().keyToValue(colorRoleName
			                   .toLatin1().constData(), &ok);
			if (!ok) {
				qCInfo(lcQt5Gtk3Interface) << "Palette" << paletteName
				                           << "contains invalid color role" << colorRoleName;
				return false;
			}
			const QPalette::ColorRole colorRole = static_cast<QPalette::ColorRole>(intVal);
			const QJsonArray &brushArray = paletteObject[colorRoleName].toArray();
			for (int brushIndex = 0; brushIndex < brushArray.size(); ++brushIndex) {
				const QJsonObject brushObject = brushArray.at(brushIndex).toObject();
				if (brushObject.isEmpty()) {
					qCInfo(lcQt5Gtk3Interface) << "Brush specification missing at for palette"
					                           << paletteName << ", Brush" << colorRoleName;
					return false;
				}

				QString value;
				GETSTR(brushObject, ceSourceType);
				const Qt5Gtk3Storage::SourceType sourceType = toSourceType(value);
				GETSTR(brushObject, ceColorGroup);
				const QPalette::ColorGroup colorGroup = toColorGroup(value);
				GETSTR(brushObject, ceColorScheme);
				const Qt5Gtk3::ColorScheme colorScheme = toColorScheme(value);
				Qt5Gtk3Storage::TargetBrush tb(colorGroup, colorRole, colorScheme);
				Qt5Gtk3Storage::Source s;

				if (!brushObject.contains(ceData) || !brushObject[ceData].isObject()) {
					qCInfo(lcQt5Gtk3Interface) << "Source specification missing for palette" << paletteName
					                           << "Brush" << colorRoleName;
					return false;
				}
				const QJsonObject &sourceObject = brushObject[ceData].toObject();

				switch (sourceType) {
				case Qt5Gtk3Storage::SourceType::Gtk: {
					GETSTR(sourceObject, ceGdkSource);
					const Qt5Gtk3Interface::QGtkColorSource gtkSource = toGdkSource(value);
					GETSTR(sourceObject, ceGtkState);
					const GtkStateFlags gtkState = toGtkState(value);
					GETSTR(sourceObject, ceGtkWidget);
					const Qt5Gtk3Interface::QGtkWidget widgetType = toWidgetType(value);
					GETINT(sourceObject, ceHeight, height);
					GETINT(sourceObject, ceWidth, width);
					s = Qt5Gtk3Storage::Source(widgetType, gtkSource, gtkState, width, height);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Fixed: {
					if (!sourceObject.contains(ceBrush)) {
						qCInfo(lcQt5Gtk3Interface) << "Fixed brush specification missing for palette" << paletteName
						                           << "Brush" << colorRoleName;
						return false;
					}
					const QJsonObject &fixedSource = sourceObject[ceBrush].toObject();
					GETINT(fixedSource, ceWidth, width);
					GETINT(fixedSource, ceHeight, height);
					GETSTR(fixedSource, ceColor);
					const QColor color(value);
					if (!color.isValid()) {
						qCInfo(lcQt5Gtk3Interface) << "Color" << value << "can't be parsed for:" << paletteName
						                           << "Brush" << colorRoleName;
						return false;
					}
					const QBrush fixedBrush = (width < 0 && height < 0)
					                          ? QBrush(color, QPixmap(width, height))
					                          : QBrush(color);
					s = Qt5Gtk3Storage::Source(fixedBrush);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Modified: {
					GETSTR(sourceObject, ceColorGroup);
					const QPalette::ColorGroup colorGroup = toColorGroup(value);
					GETSTR(sourceObject, ceColorRole);
					const QPalette::ColorRole colorRole = toColorRole(value);
					GETSTR(sourceObject, ceColorScheme);
					const Qt5Gtk3::ColorScheme colorScheme = toColorScheme(value);
					GETINT(sourceObject, ceLighter, lighter);
					GETINT(sourceObject, ceRed, red);
					GETINT(sourceObject, ceBlue, blue);
					GETINT(sourceObject, ceGreen, green);
					s = Qt5Gtk3Storage::Source(colorGroup, colorRole, colorScheme,
					                           lighter, red, green, blue);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Mixed: {
					if (!sourceObject[ceColorRole].isArray()) {
						qCInfo(lcQt5Gtk3Interface) << "Mixed brush missing the array of color roles for palette:" << paletteName
						                           << "Brush" << colorRoleName;
						return false;
					}
					QJsonArray colorRoles = sourceObject[ceColorRole].toArray();
					if (colorRoles.size() < 2) {
						qCInfo(lcQt5Gtk3Interface) << "Mixed brush missing enough color roles for palette" << paletteName
						                           << "Brush" << colorRoleName;
						return false;
					}
					const QPalette::ColorRole colorRole1 = toColorRole(colorRoles[0].toString());
					const QPalette::ColorRole colorRole2 = toColorRole(colorRoles[1].toString());
					GETSTR(sourceObject, ceColorGroup);
					const QPalette::ColorGroup sourceGroup = toColorGroup(value);
					s = Qt5Gtk3Storage::Source(sourceGroup, colorRole1, colorRole2);
				}
				break;

				case Qt5Gtk3Storage::SourceType::Invalid:
					qCInfo(lcQt5Gtk3Interface) << "Invalid source type for palette" << paletteName
					                           << "Brush." << colorRoleName;
					return false;
				}
				brushes.insert(tb, s);
			}
		}
		map.insert(paletteType, brushes);
	}
	return true;
}

QT_END_NAMESPACE
