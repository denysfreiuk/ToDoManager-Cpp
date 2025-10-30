#include "framelesswindow.h"
#include <QApplication>

static const int RESIZE_MARGIN = 2;
static const int SNAP_EDGE = 10;

FramelessWindow::FramelessWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setMouseTracking(true);
    snapPreview = new SnapPreviewWindow();
}

FramelessWindow::~FramelessWindow() {
    delete snapPreview;
}


void FramelessWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F4)
        toggleFullscreenMode();
    else
        QMainWindow::keyPressEvent(event);
}

void FramelessWindow::toggleFullscreenMode() {
    static bool isFullscreen = false;
    static QRect savedGeometry;
    static bool titleBarWasVisible = true;

    QWidget *titleBar = findChild<QWidget*>("titleBar");

    QPropertyAnimation *fade = new QPropertyAnimation(this, "windowOpacity");
    fade->setDuration(200);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    fade->setEasingCurve(QEasingCurve::InOutQuad);

    connect(fade, &QPropertyAnimation::finished, this, [=]() mutable {
        if (!isFullscreen) {
            savedGeometry = geometry();

            if (titleBar && titleBar->isVisible()) {
                titleBar->hide();
                titleBarWasVisible = true;
            } else {
                titleBarWasVisible = false;
            }

            showFullScreen();
            isFullscreen = true;
        } else {
            showNormal();
            setGeometry(savedGeometry);

            if (titleBarWasVisible && titleBar)
                titleBar->show();

            isFullscreen = false;
        }

        QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
        fadeIn->setDuration(200);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    });

    fade->start(QAbstractAnimation::DeleteWhenStopped);
}


FramelessWindow::ResizeRegion FramelessWindow::detectResizeRegion(const QPoint &pos) {
    const int x = pos.x(), y = pos.y(), w = width(), h = height();
    const bool left = x <= RESIZE_MARGIN;
    const bool right = x >= w - RESIZE_MARGIN;
    const bool top = y <= RESIZE_MARGIN;
    const bool bottom = y >= h - RESIZE_MARGIN;

    if (top && left) return TopLeft;
    if (top && right) return TopRight;
    if (bottom && left) return BottomLeft;
    if (bottom && right) return BottomRight;
    if (top) return Top;
    if (bottom) return Bottom;
    if (left) return Left;
    if (right) return Right;
    return None;
}

void FramelessWindow::updateCursorShape(const QPoint &pos) {
    if (isMaximized) {
        setCursor(Qt::ArrowCursor);
        return;
    }
    switch (detectResizeRegion(pos)) {
        case TopLeft: setCursor(Qt::SizeFDiagCursor); break;
        case BottomRight: setCursor(Qt::SizeFDiagCursor); break;
        case TopRight: setCursor(Qt::SizeFDiagCursor); break;
        case BottomLeft: setCursor(Qt::SizeBDiagCursor); break;
        case Left: setCursor(Qt::SizeHorCursor); break;
        case Right: setCursor(Qt::SizeHorCursor); break;
        case Top: setCursor(Qt::SizeVerCursor); break;
        case Bottom: setCursor(Qt::SizeVerCursor); break;
        default: setCursor(Qt::ArrowCursor); break;
    }
}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        currentResizeRegion = isMaximized ? None : detectResizeRegion(event->pos());
        if (currentResizeRegion != None) {
            isResizing = true;
            grabMouse();
            event->accept();
            return;
        }

        QWidget *titleBar = findChild<QWidget*>("titleBar");
        if (titleBar && titleBar->rect().contains(titleBar->mapFrom(this, event->pos()))) {
            isDragging = true;
            dragOffset = event->globalPos() - frameGeometry().topLeft();
            grabMouse();
            event->accept();
            return;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint globalPos = event->globalPos();

    if (isResizing && !isMaximized) {
        QRect geom = geometry();
        switch (currentResizeRegion) {
        case Left:        geom.setLeft(globalPos.x()); break;
        case Right:       geom.setRight(globalPos.x()); break;
        case Top:         geom.setTop(globalPos.y()); break;
        case Bottom:      geom.setBottom(globalPos.y()); break;
        case TopLeft:     geom.setTop(globalPos.y()); geom.setLeft(globalPos.x()); break;
        case TopRight:    geom.setTop(globalPos.y()); geom.setRight(globalPos.x()); break;
        case BottomLeft:  geom.setBottom(globalPos.y()); geom.setLeft(globalPos.x()); break;
        case BottomRight: geom.setBottom(globalPos.y()); geom.setRight(globalPos.x()); break;
        default: break;
        }
        setGeometry(geom.normalized());
        event->accept();
        return;
    }

    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        QScreen *screen = QGuiApplication::screenAt(globalPos);
        if (!screen) screen = QGuiApplication::primaryScreen();
        QRect avail = screen->availableGeometry();

        if (isMaximized) {
            double ratio = double(event->pos().x()) / width();
            int newW = avail.width() * 0.8;
            int newH = avail.height() * 0.75;
            int newX = globalPos.x() - int(newW * ratio);
            int newY = globalPos.y() - 20;

            if (newY < avail.y()) newY = avail.y();
            if (newY + newH > avail.bottom()) newY = avail.bottom() - newH;

            savedGeometryBeforeMaximize = QRect(newX, newY, newW, newH);
            setGeometry(savedGeometryBeforeMaximize);
            isMaximized = false;
            emit windowMaximizedChanged(false);

            dragOffset = globalPos - frameGeometry().topLeft();
        }

        QPoint newTopLeft = globalPos - dragOffset;

        if (globalPos.y() >= avail.bottom() - 5)
            return;

        if (newTopLeft.y() < avail.y())
            newTopLeft.setY(avail.y());

        move(newTopLeft);

        const int edge = SNAP_EDGE;
        SnapPreviewWindow::SnapType snap = SnapPreviewWindow::SnapType::None;
        if (globalPos.y() <= avail.y() + edge)
            snap = SnapPreviewWindow::SnapType::Top;
        else if (globalPos.x() <= avail.x() + edge)
            snap = SnapPreviewWindow::SnapType::Left;
        else if (globalPos.x() >= avail.x() + avail.width() - edge)
            snap = SnapPreviewWindow::SnapType::Right;

        snapPreview->showPreview(snap, screen);
        event->accept();
        return;
    }

    updateCursorShape(event->pos());
    QMainWindow::mouseMoveEvent(event);
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        releaseMouse();

        if (snapPreview && snapPreview->currentType() != SnapPreviewWindow::SnapType::None) {
            QScreen *screen = QGuiApplication::screenAt(frameGeometry().center());
            if (!screen) screen = QGuiApplication::primaryScreen();
            QRect avail = screen->availableGeometry();

            switch (snapPreview->currentType()) {
                case SnapPreviewWindow::SnapType::Top:
                    setGeometry(avail);
                    isMaximized = true;
                    emit windowMaximizedChanged(true);
                    break;
                case SnapPreviewWindow::SnapType::Left:
                    setGeometry(avail.x(), avail.y(), avail.width()/2, avail.height());
                    isMaximized = false;
                    emit windowMaximizedChanged(false);
                    break;
                case SnapPreviewWindow::SnapType::Right:
                    setGeometry(avail.x() + avail.width()/2, avail.y(),
                                avail.width()/2, avail.height());
                    isMaximized = false;
                    emit windowMaximizedChanged(false);
                    break;
                default: break;
            }
        }

        isDragging = false;
        isResizing = false;
        currentResizeRegion = None;
        if (snapPreview) snapPreview->hidePreview();
    }
    QMainWindow::mouseReleaseEvent(event);
}

void FramelessWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget *titleBar = findChild<QWidget*>("titleBar");
    if (titleBar && titleBar->rect().contains(titleBar->mapFrom(this, event->pos()))) {
        toggleMaximizeRestore();
        event->accept();
        return;
    }
    QMainWindow::mouseDoubleClickEvent(event);
}

void FramelessWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    QWidget *titleBar = findChild<QWidget*>("titleBar");
    if (titleBar)
        titleBar->setGeometry(0, 0, width(), 36);
}

void FramelessWindow::toggleMaximizeRestore()
{
    if (isMaximized) {
        setGeometry(savedGeometryBeforeMaximize);
        isMaximized = false;
    } else {
        if (!isMaximized)
            savedGeometryBeforeMaximize = geometry();

        QScreen *screen = QGuiApplication::screenAt(frameGeometry().center());
        if (!screen)
            screen = QGuiApplication::primaryScreen();
        QRect available = screen->availableGeometry();
        setGeometry(available);
        isMaximized = true;
    }
    emit windowMaximizedChanged(isMaximized);
}


//=======================================================
//                 FramelessDialog
//=======================================================


FramelessDialog::FramelessDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setMouseTracking(true);
    applyShadowEffect();
}

void FramelessDialog::applyShadowEffect()
{
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 150));
    setGraphicsEffect(shadow);
}

FramelessDialog::ResizeRegion FramelessDialog::detectResizeRegion(const QPoint &pos)
{
    const int x = pos.x(), y = pos.y(), w = width(), h = height();
    const bool left = x <= RESIZE_MARGIN;
    const bool right = x >= w - RESIZE_MARGIN;
    const bool top = y <= RESIZE_MARGIN;
    const bool bottom = y >= h - RESIZE_MARGIN;

    if (top && left) return TopLeft;
    if (top && right) return TopRight;
    if (bottom && left) return BottomLeft;
    if (bottom && right) return BottomRight;
    if (top) return Top;
    if (bottom) return Bottom;
    if (left) return Left;
    if (right) return Right;
    return None;
}

void FramelessDialog::updateCursorShape(const QPoint &pos)
{
    switch (detectResizeRegion(pos)) {
        case TopLeft:     setCursor(Qt::SizeFDiagCursor); break;
        case BottomRight: setCursor(Qt::SizeFDiagCursor); break;
        case TopRight:    setCursor(Qt::SizeBDiagCursor); break;
        case BottomLeft:  setCursor(Qt::SizeBDiagCursor); break;
        case Left:        setCursor(Qt::SizeHorCursor); break;
        case Right:       setCursor(Qt::SizeHorCursor); break;
        case Top:         setCursor(Qt::SizeVerCursor); break;
        case Bottom:      setCursor(Qt::SizeVerCursor); break;
        default:          setCursor(Qt::ArrowCursor); break;
    }
}

void FramelessDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return QDialog::mousePressEvent(event);

    currentResizeRegion = detectResizeRegion(event->pos());
    if (currentResizeRegion != None) {
        isResizing = true;
        grabMouse();
        return;
    }

    QWidget *titleBar = findChild<QWidget*>("titleBar");
    if (titleBar && titleBar->rect().contains(titleBar->mapFrom(this, event->pos()))) {
        isDragging = true;
        dragOffset = event->globalPos() - frameGeometry().topLeft();
        grabMouse();
        return;
    }

    QDialog::mousePressEvent(event);
}

void FramelessDialog::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint globalPos = event->globalPos();

    if (isResizing) {
        QRect geom = geometry();
        switch (currentResizeRegion) {
            case Left:        geom.setLeft(globalPos.x()); break;
            case Right:       geom.setRight(globalPos.x()); break;
            case Top:         geom.setTop(globalPos.y()); break;
            case Bottom:      geom.setBottom(globalPos.y()); break;
            case TopLeft:     geom.setTop(globalPos.y()); geom.setLeft(globalPos.x()); break;
            case TopRight:    geom.setTop(globalPos.y()); geom.setRight(globalPos.x()); break;
            case BottomLeft:  geom.setBottom(globalPos.y()); geom.setLeft(globalPos.x()); break;
            case BottomRight: geom.setBottom(globalPos.y()); geom.setRight(globalPos.x()); break;
            default: break;
        }
        setGeometry(geom.normalized());
        return;
    }

    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        move(globalPos - dragOffset);
        return;
    }

    updateCursorShape(event->pos());
    QDialog::mouseMoveEvent(event);
}

void FramelessDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        releaseMouse();
        isDragging = false;
        isResizing = false;
        currentResizeRegion = None;
    }
    QDialog::mouseReleaseEvent(event);
}

void FramelessDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    QWidget *titleBar = findChild<QWidget*>("titleBar");
    if (titleBar)
        titleBar->setGeometry(0, 0, width(), 36);
}


void FramelessDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    setWindowOpacity(0.0);
    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(250);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
}

void FramelessDialog::closeEvent(QCloseEvent *event)
{
    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "windowOpacity");
    fadeOut->setDuration(200);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InOutQuad);

    connect(fadeOut, &QPropertyAnimation::finished, this, [this, event]() {
        event->accept();
        QDialog::close();
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    event->ignore();
}
