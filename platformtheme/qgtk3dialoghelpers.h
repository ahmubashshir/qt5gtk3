// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGTK3DIALOGHELPERS_H
#define QGTK3DIALOGHELPERS_H

#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qstring.h>
#include <qpa/qplatformdialoghelper.h>

typedef struct _GtkWidget GtkWidget;
typedef struct _GtkDialog GtkDialog;
typedef struct _GtkFileFilter GtkFileFilter;

QT_BEGIN_NAMESPACE

class Qt5Gtk3Dialog;
class QColor;

class Qt5Gtk3ColorDialogHelper : public QPlatformColorDialogHelper
{
    Q_OBJECT

public:
    Qt5Gtk3ColorDialogHelper();
    ~Qt5Gtk3ColorDialogHelper();

    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent) override;
    void exec() override;
    void hide() override;

    void setCurrentColor(const QColor &color) override;
    QColor currentColor() const override;

private:
    static void onColorChanged(Qt5Gtk3ColorDialogHelper *helper);
    void applyOptions();

    QScopedPointer<Qt5Gtk3Dialog> d;
};

class Qt5Gtk3FileDialogHelper : public QPlatformFileDialogHelper
{
    Q_OBJECT

public:
    Qt5Gtk3FileDialogHelper();
    ~Qt5Gtk3FileDialogHelper();

    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent) override;
    void exec() override;
    void hide() override;

    bool defaultNameFilterDisables() const override;
    void setDirectory(const QUrl &directory) override;
    QUrl directory() const override;
    void selectFile(const QUrl &filename) override;
    QList<QUrl> selectedFiles() const override;
    void setFilter() override;
    void selectNameFilter(const QString &filter) override;
    QString selectedNameFilter() const override;

private:
    static void onSelectionChanged(GtkDialog *dialog, Qt5Gtk3FileDialogHelper *helper);
    static void onCurrentFolderChanged(Qt5Gtk3FileDialogHelper *helper);
    static void onFilterChanged(Qt5Gtk3FileDialogHelper *helper);
    static void onUpdatePreview(GtkDialog *dialog, Qt5Gtk3FileDialogHelper *helper);
    void applyOptions();
    void setNameFilters(const QStringList &filters);
    void selectFileInternal(const QUrl &filename);
    void setFileChooserAction();

    QUrl _dir;
    QList<QUrl> _selection;
    QHash<QString, GtkFileFilter*> _filters;
    QHash<GtkFileFilter*, QString> _filterNames;
    QScopedPointer<Qt5Gtk3Dialog> d;
    GtkWidget *previewWidget;
};

class Qt5Gtk3FontDialogHelper : public QPlatformFontDialogHelper
{
    Q_OBJECT

public:
    Qt5Gtk3FontDialogHelper();
    ~Qt5Gtk3FontDialogHelper();

    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent) override;
    void exec() override;
    void hide() override;

    void setCurrentFont(const QFont &font) override;
    QFont currentFont() const override;

private:
    static void onFontChanged(Qt5Gtk3FontDialogHelper *helper);
    void applyOptions();

    QScopedPointer<Qt5Gtk3Dialog> d;
};

QT_END_NAMESPACE

#endif // QGTK3DIALOGHELPERS_H
