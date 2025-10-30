#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMouseEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include "snapPreviewWindow.h"

/* VOID
 void MainWindow::setupTitleBar() {
    ui->titleBar->setMinimumHeight(36);
    ui->titleBar->setMaximumHeight(36);

    connect(ui->btnClose,    &QPushButton::clicked, this, &QWidget::close);
    connect(ui->btnMinimize, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->btnMaximize, &QPushButton::clicked, this, &MainWindow::toggleMaximizeRestore);

    if (auto *v = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout())) {
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(0);
    }
}

CONSTRUCTOR:
setupTitleBar();
    snapPreview = new SnapPreviewWindow(this);
connect(this, &FramelessWindow::windowMaximizedChanged, this, &MainWindow::updateMaximizeIcon);

DESTRUCTOR:
if (snapPreview) {
        snapPreview->hidePreview();
        snapPreview->deleteLater();
    }

IN UI -> QWidget#titleBar:  QLabels: IconName, titleLabel;  horizontalLayout: (QPushButtons: btnMinimize, btnMaximize, btnClose);
 */


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
    void keyPressEvent(QKeyEvent *event) override;

    enum ResizeRegion {
        None, Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };
    ResizeRegion detectResizeRegion(const QPoint &pos);
    void updateCursorShape(const QPoint &pos);
    void toggleMaximizeRestore();
    void toggleFullscreenMode();


    bool isMaximized = false;
    bool isDragging = false;
    bool isResizing = false;
    QPoint dragOffset;
    QRect savedGeometryBeforeMaximize;
    ResizeRegion currentResizeRegion = None;

    SnapPreviewWindow *snapPreview = nullptr;
};


/*
.h
#include "../windowEdit/framelessWindow.h"

class YOURCLASS : public FramelessDialog

private:
    void setupTitleBar();

.cpp

void YOURCLASS::setupTitleBar() {
    ui->titleBar->setMinimumHeight(36);
    ui->titleBar->setMaximumHeight(36);
    connect(ui->btnClose,    &QPushButton::clicked, this, &QWidget::close);
}

CONSTRUCTOR:

YOURCLASS::YOURCLASS(QWidget *parent)
    : FramelessDialog(parent),     // prev: QDialog
      ui(new Ui::YOURCLASS)

setupTitleBar();
IN UI -> QWidget#titleBar:  QLabels: IconName, titleLabel;  horizontalLayout: (QPushButtons: btnClose);

 */



class FramelessDialog : public QDialog {
    Q_OBJECT
public:
    explicit FramelessDialog(QWidget *parent = nullptr);
    ~FramelessDialog() override = default;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum ResizeRegion {
        None, Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };

    ResizeRegion detectResizeRegion(const QPoint &pos);
    void updateCursorShape(const QPoint &pos);
    void applyShadowEffect();

    bool isDragging = false;
    bool isResizing = false;
    QPoint dragOffset;
    ResizeRegion currentResizeRegion = None;

    static constexpr int RESIZE_MARGIN = 4;
};

#endif // FRAMELESSDIALOG_H