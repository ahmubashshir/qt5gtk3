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


#include "qgtk3interface_p.h"
#include "qgtk3storage_p.h"
#include <QtCore/QMetaEnum>
#include <QtCore/QFileInfo>
#include <QtCore/QScopeGuard>
#include <QtGui/QFontDatabase>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE
Q_LOGGING_CATEGORY(lcQt5Gtk3Interface, "qt.qpa.qt5gtk3.interface");


// Callback for gnome event loop has to be static
static Qt5Gtk3Storage *m_storage = nullptr;

Qt5Gtk3Interface::Qt5Gtk3Interface(Qt5Gtk3Storage *s)
{
	initColorMap();

	if (!s) {
		qCDebug(lcQt5Gtk3Interface) << "Qt5Gtk3Interface instantiated without Qt5Gtk3Storage."
		                            << "No reaction to runtime theme changes.";
		return;
	}

	// Connect to the GTK settings changed signal
	auto handleThemeChange = [] {
		if (m_storage)
			m_storage->handleThemeChange();
	};

	GtkSettings *settings = gtk_settings_get_default();
	const gboolean success = g_signal_connect(settings, "notify::gtk-theme-name",
	                         G_CALLBACK(handleThemeChange), nullptr);
	if (success == FALSE) {
		qCDebug(lcQt5Gtk3Interface) << "Connection to theme change signal failed."
		                            << "No reaction to runtime theme changes.";
	} else {
		m_storage = s;
	}
}

Qt5Gtk3Interface::~Qt5Gtk3Interface()
{
	// Ignore theme changes when destructor is reached
	m_storage = nullptr;

	// QGtkWidgets have to be destroyed manually
	for (auto v : cache)
		gtk_widget_destroy(v);
}

/*!
    \internal
    \brief Converts a string into the GtkStateFlags enum.

    Converts a string formatted GTK color \param state into an enum value.
    Returns an integer corresponding to GtkStateFlags.
    Returns -1 if \param state does not correspond to a valid enum key.
 */
int Qt5Gtk3Interface::toGtkState(const QString &state)
{
#define CASE(x) \
    if (QLatin1String(QByteArray(state.toLatin1())) == QLatin1String(#x)) \
        return GTK_STATE_FLAG_ ##x

#define CONVERT\
    CASE(NORMAL);\
    CASE(ACTIVE);\
    CASE(PRELIGHT);\
    CASE(SELECTED);\
    CASE(INSENSITIVE);\
    CASE(INCONSISTENT);\
    CASE(FOCUSED);\
    CASE(BACKDROP);\
    CASE(DIR_LTR);\
    CASE(DIR_RTL);\
    CASE(LINK);\
    CASE(VISITED);\
    CASE(CHECKED);\
    CASE(DROP_ACTIVE)

	CONVERT;
	return -1;
#undef CASE
}

/*!
    \internal
    \brief Returns \param state converted into a string.
 */
const QLatin1String Qt5Gtk3Interface::fromGtkState(GtkStateFlags state)
{
#define CASE(x) case GTK_STATE_FLAG_ ##x: return QLatin1String(#x)
	switch (state) {
		CONVERT;
	}
	Q_UNREACHABLE();
#undef CASE
#undef CONVERT
}

/*!
    \internal
    \brief Populates the internal map used to find a GTK color's source and fallback generic color.
 */
void Qt5Gtk3Interface::initColorMap()
{
#define SAVE(src, state, prop, def)\
    {ColorKey({QGtkColorSource::src, GTK_STATE_FLAG_ ##state}), ColorValue({QLatin1String(#prop), QGtkColorDefault::def})}

	gtkColorMap = ColorMap {
		SAVE(Foreground, NORMAL, theme_fg_color, Foreground),
		SAVE(Foreground, BACKDROP, theme_unfocused_selected_fg_color, Foreground),
		SAVE(Foreground, INSENSITIVE, insensitive_fg_color, Foreground),
		SAVE(Foreground, SELECTED, theme_selected_fg_color, Foreground),
		SAVE(Foreground, ACTIVE, theme_unfocused_fg_color, Foreground),
		SAVE(Text, NORMAL, theme_text_color, Foreground),
		SAVE(Text, ACTIVE, theme_unfocused_text_color, Foreground),
		SAVE(Base, NORMAL, theme_base_color, Background),
		SAVE(Base, INSENSITIVE, insensitive_base_color, Background),
		SAVE(Background, NORMAL, theme_bg_color, Background),
		SAVE(Background, SELECTED, theme_selected_bg_color, Background),
		SAVE(Background, INSENSITIVE, insensitive_bg_color, Background),
		SAVE(Background, ACTIVE, theme_unfocused_bg_color, Background),
		SAVE(Background, BACKDROP, theme_unfocused_selected_bg_color, Background),
		SAVE(Border, NORMAL, borders, Border),
		SAVE(Border, ACTIVE, unfocused_borders, Border)
	};
#undef SAVE

	qCDebug(lcQt5Gtk3Interface) << "Color map populated from defaults.";
}

/*!
    \internal
    \brief Returns a QImage corresponding to \param standardPixmap.

    A QImage (not a QPixmap) is returned so it can be cached and re-scaled in case the pixmap is
    requested multiple times with different resolutions.

    \note Rather than defaulting to a QImage(), all QPlatformTheme::StandardPixmap enum values have
    been mentioned explicitly.
    That way they can be covered more easily in case additional icons are provided by GTK.
 */
QImage Qt5Gtk3Interface::standardPixmap(QPlatformTheme::StandardPixmap standardPixmap) const
{
	switch (standardPixmap) {
	case QPlatformTheme::DialogDiscardButton:
		return qt_gtk_get_icon(GTK_STOCK_DELETE);
	case QPlatformTheme::DialogOkButton:
		return qt_gtk_get_icon(GTK_STOCK_OK);
	case QPlatformTheme::DialogCancelButton:
		return qt_gtk_get_icon(GTK_STOCK_CANCEL);
	case QPlatformTheme::DialogYesButton:
		return qt_gtk_get_icon(GTK_STOCK_YES);
	case QPlatformTheme::DialogNoButton:
		return qt_gtk_get_icon(GTK_STOCK_NO);
	case QPlatformTheme::DialogOpenButton:
		return qt_gtk_get_icon(GTK_STOCK_OPEN);
	case QPlatformTheme::DialogCloseButton:
		return qt_gtk_get_icon(GTK_STOCK_CLOSE);
	case QPlatformTheme::DialogApplyButton:
		return qt_gtk_get_icon(GTK_STOCK_APPLY);
	case QPlatformTheme::DialogSaveButton:
		return qt_gtk_get_icon(GTK_STOCK_SAVE);
	case QPlatformTheme::MessageBoxWarning:
		return qt_gtk_get_icon(GTK_STOCK_DIALOG_WARNING);
	case QPlatformTheme::MessageBoxQuestion:
		return qt_gtk_get_icon(GTK_STOCK_DIALOG_QUESTION);
	case QPlatformTheme::MessageBoxInformation:
		return qt_gtk_get_icon(GTK_STOCK_DIALOG_INFO);
	case QPlatformTheme::MessageBoxCritical:
		return qt_gtk_get_icon(GTK_STOCK_DIALOG_ERROR);
	case QPlatformTheme::CustomBase:
	case QPlatformTheme::TitleBarMenuButton:
	case QPlatformTheme::TitleBarMinButton:
	case QPlatformTheme::TitleBarMaxButton:
	case QPlatformTheme::TitleBarCloseButton:
	case QPlatformTheme::TitleBarNormalButton:
	case QPlatformTheme::TitleBarShadeButton:
	case QPlatformTheme::TitleBarUnshadeButton:
	case QPlatformTheme::TitleBarContextHelpButton:
	case QPlatformTheme::DockWidgetCloseButton:
	case QPlatformTheme::DesktopIcon:
	case QPlatformTheme::TrashIcon:
	case QPlatformTheme::ComputerIcon:
	case QPlatformTheme::DriveFDIcon:
	case QPlatformTheme::DriveHDIcon:
	case QPlatformTheme::DriveCDIcon:
	case QPlatformTheme::DriveDVDIcon:
	case QPlatformTheme::DriveNetIcon:
	case QPlatformTheme::DirOpenIcon:
	case QPlatformTheme::DirClosedIcon:
	case QPlatformTheme::DirLinkIcon:
	case QPlatformTheme::DirLinkOpenIcon:
	case QPlatformTheme::FileIcon:
	case QPlatformTheme::FileLinkIcon:
	case QPlatformTheme::ToolBarHorizontalExtensionButton:
	case QPlatformTheme::ToolBarVerticalExtensionButton:
	case QPlatformTheme::FileDialogStart:
	case QPlatformTheme::FileDialogEnd:
	case QPlatformTheme::FileDialogToParent:
	case QPlatformTheme::FileDialogNewFolder:
	case QPlatformTheme::FileDialogDetailedView:
	case QPlatformTheme::FileDialogInfoView:
	case QPlatformTheme::FileDialogContentsView:
	case QPlatformTheme::FileDialogListView:
	case QPlatformTheme::FileDialogBack:
	case QPlatformTheme::DirIcon:
	case QPlatformTheme::DialogHelpButton:
	case QPlatformTheme::DialogResetButton:
	case QPlatformTheme::ArrowUp:
	case QPlatformTheme::ArrowDown:
	case QPlatformTheme::ArrowLeft:
	case QPlatformTheme::ArrowRight:
	case QPlatformTheme::ArrowBack:
	case QPlatformTheme::ArrowForward:
	case QPlatformTheme::DirHomeIcon:
	case QPlatformTheme::CommandLink:
	case QPlatformTheme::VistaShield:
	case QPlatformTheme::BrowserReload:
	case QPlatformTheme::BrowserStop:
	case QPlatformTheme::MediaPlay:
	case QPlatformTheme::MediaStop:
	case QPlatformTheme::MediaPause:
	case QPlatformTheme::MediaSkipForward:
	case QPlatformTheme::MediaSkipBackward:
	case QPlatformTheme::MediaSeekForward:
	case QPlatformTheme::MediaSeekBackward:
	case QPlatformTheme::MediaVolume:
	case QPlatformTheme::MediaVolumeMuted:
	case QPlatformTheme::LineEditClearButton:
#if 0 // unsupported in qt5
	case QPlatformTheme::DialogYesToAllButton:
	case QPlatformTheme::DialogNoToAllButton:
	case QPlatformTheme::DialogSaveAllButton:
	case QPlatformTheme::DialogAbortButton:
	case QPlatformTheme::DialogRetryButton:
	case QPlatformTheme::DialogIgnoreButton:
	case QPlatformTheme::RestoreDefaultsButton:
	case QPlatformTheme::TabCloseButton:
	case QPlatformTheme::NStandardPixmap:
#endif
		return QImage();
	}
	Q_UNREACHABLE();
}

/*!
    \internal
    \brief Returns a QImage for a given GTK \param iconName.
 */
QImage Qt5Gtk3Interface::qt_gtk_get_icon(const char* iconName) const
{
	GError *error = NULL;
	GtkIconTheme *theme = gtk_icon_theme_get_default();
	GdkPixbuf* icon = gtk_icon_theme_load_icon(theme, iconName, GTK_ICON_SIZE_DIALOG, (GtkIconLookupFlags) 0, &error);

	if (!icon) {
		g_warning ("Couldn’t load icon: %s", error->message);
		g_error_free (error);
		return QImage();

	}

	QImage i = qt_convert_gdk_pixbuf(icon);
	g_object_unref (icon);
	return i;
}

/*!
    \internal
    \brief Returns a QImage converted from the GDK pixel buffer \param buf.

    The ability to convert GdkPixbuf to QImage relies on the following assumptions:
    \list
    \li QImage uses uchar as a data container (unasserted)
    \li the types guint8 and uchar are identical (statically asserted)
    \li GDK pixel buffer uses 8 bits per sample (assumed at runtime)
    \li GDK pixel buffer has 4 channels (assumed at runtime)
    \endlist
 */
QImage Qt5Gtk3Interface::qt_convert_gdk_pixbuf(GdkPixbuf *buf) const
{
	if (!buf)
		return QImage();

	const guint8 *gdata = gdk_pixbuf_read_pixels(buf);
	static_assert(std::is_same<decltype(gdata), const uchar *>::value,
	              "guint8 has diverted from uchar. Code needs fixing.");
	Q_ASSERT(gdk_pixbuf_get_bits_per_sample(buf) == 8);
	Q_ASSERT(gdk_pixbuf_get_n_channels(buf) == 4);
	const uchar *data = static_cast<const uchar *>(gdata);

	const int width = gdk_pixbuf_get_width(buf);
	const int height = gdk_pixbuf_get_height(buf);
	const int bpl = gdk_pixbuf_get_rowstride(buf);
	QImage converted(data, width, height, bpl, QImage::Format_RGBA8888);

	// convert to more optimal format and detach to survive lifetime of buf
	return converted.convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

/*!
    \internal
    \brief Instantiate a new GTK widget.

    Returns a pointer to a new GTK widget of \param type, allocated on the heap.
    Returns nullptr of gtk_Default has is passed.
 */
GtkWidget *Qt5Gtk3Interface::qt_new_gtkWidget(QGtkWidget type) const
{
#define CASE(Type)\
    case QGtkWidget::Type: return Type ##_new();
#define CASEN(Type)\
    case QGtkWidget::Type: return Type ##_new(nullptr);

	switch (type) {
		CASE(gtk_menu_bar)
		CASE(gtk_menu)
		CASE(gtk_button)
	case QGtkWidget::gtk_button_box:
		return gtk_button_box_new(GtkOrientation::GTK_ORIENTATION_HORIZONTAL);
		CASE(gtk_check_button)
		CASEN(gtk_radio_button)
		CASEN(gtk_frame)
		CASE(gtk_statusbar)
		CASE(gtk_entry)
	case QGtkWidget::gtk_popup:
		return gtk_window_new(GTK_WINDOW_POPUP);
		CASE(gtk_notebook)
		CASE(gtk_toolbar)
		CASE(gtk_tree_view)
		CASE(gtk_combo_box)
		CASE(gtk_combo_box_text)
		CASE(gtk_progress_bar)
		CASE(gtk_fixed)
		CASE(gtk_separator_menu_item)
		CASE(gtk_offscreen_window)
	case QGtkWidget::gtk_Default:
		return nullptr;
	}
#undef CASE
#undef CASEN
	Q_UNREACHABLE();
}

/*!
    \internal
    \brief Read a GTK widget's color from a generic color getter.

    This method returns a generic color of \param con, a given GTK style context.
    The requested color is defined by \param def and the GTK color-state \param state.
    The return type is GDK color in RGBA format.
 */
GdkRGBA Qt5Gtk3Interface::genericColor(GtkStyleContext *con, GtkStateFlags state, QGtkColorDefault def) const
{
	GdkRGBA color;

#define CASE(def, call)\
    case QGtkColorDefault::def:\
        gtk_style_context_get(con, state, GTK_STYLE_PROPERTY_##call, &color, NULL);\
        break;

	switch (def) {
		CASE(Foreground, COLOR)
		CASE(Background, BACKGROUND_COLOR)
		CASE(Border, BORDER_COLOR)
	}
	return color;
#undef CASE
}

/*!
    \internal
    \brief Read a GTK widget's color from a property.

    Returns a color of GTK-widget \param widget, defined by \param source and \param state.
    The return type is GDK color in RGBA format.

    \note If no corresponding property can be found for \param source, the method falls back to a
    suitable generic color.
 */
QColor Qt5Gtk3Interface::color(GtkWidget *widget, QGtkColorSource source, GtkStateFlags state) const
{
	GdkRGBA col;
	GtkStyleContext *con = context(widget);

#define CASE(src, def)\
    case QGtkColorSource::src: {\
        const ColorKey key = ColorKey({QGtkColorSource::src, state});\
        if (gtkColorMap.contains(key)) {\
            const ColorValue val = gtkColorMap.value(key);\
            if (!gtk_style_context_lookup_color(con, val.propertyName.toUtf8().constData(), &col)) {\
                col = genericColor(con, state, val.genericSource);\
                qCDebug(lcQt5Gtk3Interface) << "Property name" << val.propertyName << "not found.\n"\
                                          << "Falling back to " << val.genericSource;\
            }\
         } else {\
           col = genericColor(con, state, QGtkColorDefault::def);\
           qCDebug(lcQt5Gtk3Interface) << "No color source found for" << QGtkColorSource::src\
                                     << fromGtkState(state) << "\n Falling back to"\
                                     << QGtkColorDefault::def;\
         }\
    }\
         break;

	switch (source) {
		CASE(Foreground, Foreground)
		CASE(Background, Background)
		CASE(Text, Foreground)
		CASE(Base, Background)
		CASE(Border, Border)
	}

	return fromGdkColor(col);
#undef CASE
}

/*!
    \internal
    \brief Get pointer to a GTK widget by \param type.

    Returns the pointer to a GTK widget, specified by \param type.
    GTK widgets are cached, so that only one instance of each type is created.
    \note
    The method returns nullptr for the enum value gtk_Default.
 */
GtkWidget *Qt5Gtk3Interface::widget(QGtkWidget type) const
{
	if (type == QGtkWidget::gtk_Default)
		return nullptr;

	// Return from cache
	if (GtkWidget *w = cache.value(type))
		return w;

	// Create new item and cache it
	GtkWidget *w = qt_new_gtkWidget(type);
	cache.insert(type, w);
	return w;
}

/*!
    \internal
    \brief Access a GTK widget's style context.

    Returns the pointer to the style context of GTK widget \param w.

    \note If \param w is nullptr, the GTK default style context (entry style) is returned.
 */
GtkStyleContext *Qt5Gtk3Interface::context(GtkWidget *w) const
{
	if (w)
		return gtk_widget_get_style_context(w);

	return gtk_widget_get_style_context(widget(QGtkWidget::gtk_entry));
}

/*!
    \internal
    \brief Create a QBrush from a GTK widget.

    Returns a QBrush corresponding to GTK widget type \param wtype, \param source and \param state.

    Brush height and width is ignored in GTK3, because brush assets (e.g. 9-patches)
    can't be accessed by the GTK3 API. It's therefore unknown, if the brush relates only to colors,
    or to a pixmap based style.

 */
QBrush Qt5Gtk3Interface::brush(QGtkWidget wtype, QGtkColorSource source, GtkStateFlags state) const
{
	// FIXME: When a color's pixmap can be accessed via the GTK API,
	// read it and set it in the brush.
	return QBrush(color(widget(wtype), source, state));
}

/*!
    \internal
    \brief Returns the name of the current GTK theme.
 */
QString Qt5Gtk3Interface::themeName() const
{
	QString name;

	if (GtkSettings *settings = gtk_settings_get_default()) {
		gchar *theme_name;
		g_object_get(settings, "gtk-theme-name", &theme_name, nullptr);
		name = QLatin1String(theme_name);
		g_free(theme_name);
	}

	return name;
}

/*!
    \internal
    \brief Determine color scheme by colors.

    Returns the color scheme of the current GTK theme, heuristically determined by the
    lightness difference between default background and foreground colors.

    \note Returns Unknown in the unlikely case that both colors have the same lightness.
 */
Qt5Gtk3::ColorScheme Qt5Gtk3Interface::colorSchemeByColors() const
{
	const QColor background = color(widget(QGtkWidget::gtk_Default),
	                                QGtkColorSource::Background,
	                                GTK_STATE_FLAG_ACTIVE);
	const QColor foreground = color(widget(QGtkWidget::gtk_Default),
	                                QGtkColorSource::Foreground,
	                                GTK_STATE_FLAG_ACTIVE);

	if (foreground.lightness() > background.lightness())
		return Qt5Gtk3::ColorScheme::Dark;
	if (foreground.lightness() < background.lightness())
		return Qt5Gtk3::ColorScheme::Light;
	return Qt5Gtk3::ColorScheme::Unknown;
}

/*!
    \internal
    \brief Map font type to GTK widget type.

    Returns the GTK widget type corresponding to the given QPlatformTheme::Font \param type.
 */
inline constexpr Qt5Gtk3Interface::QGtkWidget Qt5Gtk3Interface::toWidgetType(QPlatformTheme::Font type)
{
	switch (type) {
	case QPlatformTheme::SystemFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::MenuFont:
		return QGtkWidget::gtk_menu;
	case QPlatformTheme::MenuBarFont:
		return QGtkWidget::gtk_menu_bar;
	case QPlatformTheme::MenuItemFont:
		return QGtkWidget::gtk_menu;
	case QPlatformTheme::MessageBoxFont:
		return QGtkWidget::gtk_popup;
	case QPlatformTheme::LabelFont:
		return QGtkWidget::gtk_popup;
	case QPlatformTheme::TipLabelFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::StatusBarFont:
		return QGtkWidget::gtk_statusbar;
	case QPlatformTheme::TitleBarFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::MdiSubWindowTitleFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::DockWidgetTitleFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::PushButtonFont:
		return QGtkWidget::gtk_button;
	case QPlatformTheme::CheckBoxFont:
		return QGtkWidget::gtk_check_button;
	case QPlatformTheme::RadioButtonFont:
		return QGtkWidget::gtk_radio_button;
	case QPlatformTheme::ToolButtonFont:
		return QGtkWidget::gtk_button;
	case QPlatformTheme::ItemViewFont:
		return QGtkWidget::gtk_entry;
	case QPlatformTheme::ListViewFont:
		return QGtkWidget::gtk_tree_view;
	case QPlatformTheme::HeaderViewFont:
		return QGtkWidget::gtk_combo_box;
	case QPlatformTheme::ListBoxFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::ComboMenuItemFont:
		return QGtkWidget::gtk_combo_box;
	case QPlatformTheme::ComboLineEditFont:
		return QGtkWidget::gtk_combo_box_text;
	case QPlatformTheme::SmallFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::MiniFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::FixedFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::GroupBoxTitleFont:
		return QGtkWidget::gtk_Default;
	case QPlatformTheme::TabButtonFont:
		return QGtkWidget::gtk_button;
	case QPlatformTheme::EditorFont:
		return QGtkWidget::gtk_entry;
	case QPlatformTheme::NFonts:
		return QGtkWidget::gtk_Default;
	}
	Q_UNREACHABLE();
}

/*!
    \internal
    \brief Convert pango \param style to QFont::Style.
 */
inline constexpr QFont::Style Qt5Gtk3Interface::toFontStyle(PangoStyle style)
{
	switch (style) {
	case PANGO_STYLE_ITALIC:
		return QFont::StyleItalic;
	case PANGO_STYLE_OBLIQUE:
		return QFont::StyleOblique;
	case PANGO_STYLE_NORMAL:
		return QFont::StyleNormal;
	}
	// This is reached when GTK has introduced a new font style
	Q_UNREACHABLE();
}

/*!
    \internal
    \brief Convert pango font \param weight to an int, representing font weight in Qt.

    Compatibility of PangoWeight is statically asserted.
    The minimum (1) and maximum (1000) weight in Qt is respeced.
 */
inline constexpr int Qt5Gtk3Interface::toFontWeight(PangoWeight weight)
{
	// GTK PangoWeight can be directly converted to QFont::Weight
	// unless one of the enums changes.
#define IF
#define OR else
#define choose(MIN, RET) if(weight >= MIN) return QFont::RET
	IF choose(PANGO_WEIGHT_HEAVY, Black);
	OR choose(PANGO_WEIGHT_ULTRABOLD, ExtraBold);
	OR choose(PANGO_WEIGHT_BOLD, Bold);
	OR choose(PANGO_WEIGHT_SEMIBOLD, DemiBold);
	OR choose(PANGO_WEIGHT_MEDIUM, Medium);
	OR choose(PANGO_WEIGHT_NORMAL, Normal);
	OR choose(PANGO_WEIGHT_LIGHT, Light);
	OR choose(PANGO_WEIGHT_ULTRALIGHT, ExtraLight);
	OR return QFont::Thin;
#undef IF
#undef OR
#undef choose
}

/*!
    \internal
    \brief Return a GTK styled font.

    Returns the QFont corresponding to \param type by reading the corresponding
    GTK widget type's font.

    \note GTK allows to specify a non fixed font as the system's fixed font.
    If a fixed font is requested, the method fixes the pitch and falls back to monospace,
    unless a suitable fixed pitch font is found.
 */
QFont Qt5Gtk3Interface::font(QPlatformTheme::Font type) const
{
	GtkStyleContext *con = context(widget(toWidgetType(type)));
	if (!con)
		return QFont();

	// explicitly add provider for fixed font
	GtkCssProvider *cssProvider = nullptr;
	if (type == QPlatformTheme::FixedFont) {
		cssProvider = gtk_css_provider_new();
		gtk_style_context_add_class (con, GTK_STYLE_CLASS_MONOSPACE);
		const char *fontSpec = "* {font-family: monospace;}";
		gtk_css_provider_load_from_data(cssProvider, fontSpec, -1, NULL);
		gtk_style_context_add_provider(con, GTK_STYLE_PROVIDER(cssProvider),
		                               GTK_STYLE_PROVIDER_PRIORITY_USER);
	}

	// remove monospace provider from style context and unref it
	QScopeGuard guard([&]() {
		if (cssProvider) {
			gtk_style_context_remove_provider(con, GTK_STYLE_PROVIDER(cssProvider));
			g_object_unref(cssProvider);
		}
	});

	PangoFontDescription *gtkFont = NULL;
	gtk_style_context_get(con, GTK_STATE_FLAG_NORMAL,
	                      GTK_STYLE_PROPERTY_FONT, &gtkFont, NULL);
	if (!gtkFont)
		return QFont();

	const QString family = QString::fromLatin1(pango_font_description_get_family(gtkFont));
	if (family.isEmpty()) {
		pango_font_description_free(gtkFont);
		return QFont();
	}

	const int weight = toFontWeight(pango_font_description_get_weight(gtkFont));

	// Creating a QFont() creates a futex lockup on a theme change
	// QFont doesn't have a constructor with float point size
	// => create a dummy point size and set it later.
	QFont font(family, 1, weight);
	font.setPointSizeF(static_cast<float>(pango_font_description_get_size(gtkFont)/PANGO_SCALE));
	font.setStyle(toFontStyle(pango_font_description_get_style(gtkFont)));

	if (type == QPlatformTheme::FixedFont) {
		font.setFixedPitch(true);
		if (!QFontInfo(font).fixedPitch()) {
			qCDebug(lcQt5Gtk3Interface) << "No fixed pitch font found in font family"
			                            << font.family() << ". falling back to a default"
			                            << "fixed pitch font";
			font.setFamily(QLatin1String("monospace"));
			font.setFamily(QLatin1String("monospace"));
		}
	}

	pango_font_description_free(gtkFont);
	return font;
}

/*!
    \internal
    \brief Returns a GTK styled file icon for \param fileInfo.
 */
QIcon Qt5Gtk3Interface::fileIcon(const QFileInfo &fileInfo) const
{
	GFile *file = g_file_new_for_path(fileInfo.absoluteFilePath().toLatin1().constData());
	if (!file)
		return QIcon();

	GFileInfo *info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_ICON,
	                                     G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
	if (!info) {
		g_object_unref(file);
		return QIcon();
	}

	GIcon *icon = g_file_info_get_icon(info);
	if (!icon) {
		g_object_unref(file);
		g_object_unref(info);
		return QIcon();
	}

	GtkIconTheme *theme = gtk_icon_theme_get_default();
	GtkIconInfo *iconInfo = gtk_icon_theme_lookup_by_gicon(theme, icon, 16,
	                        GTK_ICON_LOOKUP_FORCE_SIZE);
	if (!iconInfo) {
		g_object_unref(file);
		g_object_unref(info);
		return QIcon();
	}

	GdkPixbuf *buf = gtk_icon_info_load_icon(iconInfo, nullptr);
	QImage image = qt_convert_gdk_pixbuf(buf);
	g_object_unref(file);
	g_object_unref(info);
	g_object_unref(buf);
	return QIcon(QPixmap::fromImage(image));
}

QT_END_NAMESPACE
