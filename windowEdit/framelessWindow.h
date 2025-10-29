#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMouseEvent>
#include <QScreen>
#include <QGuiApplication>
#include "../windowEdit/snapPreviewWindow.h"

class FramelessWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *parent = nullptr);
    ~FramelessWindow() override;

    signals:
        void windowMaximizedChanged(bool maximized);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    enum ResizeRegion {
        None, Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };
    ResizeRegion detectResizeRegion(const QPoint &pos);
    void updateCursorShape(const QPoint &pos);
    void toggleMaximizeRestore();

    bool isMaximized = false;
    bool isDragging = false;
    bool isResizing = false;
    QPoint dragOffset;
    QRect savedGeometryBeforeMaximize;
    ResizeRegion currentResizeRegion = None;

    SnapPreviewWindow *snapPreview = nullptr;
};

class FramelessDialog : public QDialog {
    Q_OBJECT
public:
    explicit FramelessDialog(QWidget *parent = nullptr);
    ~FramelessDialog() override;

    signals:
        void windowMaximizedChanged(bool maximized);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum ResizeRegion {
        None, Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };

    ResizeRegion detectResizeRegion(const QPoint &pos);
    void updateCursorShape(const QPoint &pos);
    void toggleMaximizeRestore();

    bool isMaximized = false;
    bool isDragging = false;
    bool isResizing = false;
    QPoint dragOffset;
    QRect savedGeometryBeforeMaximize;
    ResizeRegion currentResizeRegion = None;

    SnapPreviewWindow *snapPreview = nullptr;
};

#endif // FRAMELESSDIALOG_H