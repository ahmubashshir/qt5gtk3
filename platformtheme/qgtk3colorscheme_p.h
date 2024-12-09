#ifndef __QGTK3COLORSCHEME_H__
#define __QGTK3COLORSCHEME_H__

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
namespace QGtk3 {
	Q_NAMESPACE

	enum class ColorScheme {
		Dark,
		Light,
		Unknown
	};
	Q_ENUM_NS(ColorScheme)
}
QT_END_NAMESPACE


#endif /* __QGTK3COLORSCHEME_H__ */
