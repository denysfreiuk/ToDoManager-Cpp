#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QPixmap>
#include <QTransform>
#include <QToolBox>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButtonShow, &QPushButton::clicked, this, [=](){
        QString text = ui->lineEditInput->text();
        QMessageBox::information(this, "text: ", text);
    });

    connect(ui->pushButtonClear, &QPushButton::clicked,
            ui->lineEditInput, &QLineEdit::clear);



    connect(ui->toolBoxTasks, &QToolBox::currentChanged, this, [&](int index) {
        static int previousIndex = -1;
        // Відкривається нова сторінка
        QString icon = "../resources/icons/right-arrow.png";
        ui->toolBoxTasks->setItemIcon(index, QIcon(icon));
        animateIconRotation(ui->toolBoxTasks, index, true);

        if (previousIndex != -1 && previousIndex != index) {
            ui->toolBoxTasks->setItemIcon(previousIndex, QIcon(icon));
            animateIconRotation(ui->toolBoxTasks, previousIndex, false);
        }

        previousIndex = index;
    });




    //MyObjects obj1, obj2;
    //connect(&obj1, &MyObjects::mySignal, &obj2, &MyObjects::mySlot);
    //emit obj1.mySignal(); // надсилає сигнал, викликає слот у obj2
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::animateIconRotation(QToolBox *toolBox, int pageIndex, bool opening)
{
    QIcon icon = toolBox->itemIcon(pageIndex);
    QPixmap pix = icon.pixmap(24, 24);

    QVariantAnimation *anim = new QVariantAnimation(this);
    anim->setDuration(300);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->setStartValue(opening ? 0 : 90);
    anim->setEndValue(opening ? 90 : 0);

    connect(anim, &QVariantAnimation::valueChanged, this, [=](const QVariant &value) {
        QTransform transform;
        transform.rotate(value.toReal());
        QPixmap rotated = pix.transformed(transform, Qt::SmoothTransformation);
        toolBox->setItemIcon(pageIndex, QIcon(rotated));
    });

    connect(anim, &QVariantAnimation::finished, anim, &QObject::deleteLater);
    anim->start();
}
//void MyObjects::mySlot() {
//    qDebug() << "Мій слот викликано!";
//}

