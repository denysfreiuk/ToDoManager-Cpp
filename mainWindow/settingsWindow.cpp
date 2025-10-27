#include "settingsWindow.h"
#include "ui_settingsWindow.h"
#include <QFileDialog>
#include <QDebug>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::SettingsWindow),
      settings("ZooSoft", "ToDoManager")
{
    ui->setupUi(this);
    setWindowTitle("Settings");

    ui->defaultDeadlineBox->addItems({"None", "Today", "Tomorrow", "In 3 days", "In a week"});
    ui->deletePeriodBox->addItems({"Immediately", "After 1 day", "After 1 week", "After deadline passes"});
    ui->themeBox->addItems({"Light", "Dark"});

    loadSettings();

    connect(ui->autoDeleteCheck, &QCheckBox::stateChanged, this, [this](int state) {
        bool enabled = (state == Qt::Checked);
        ui->labelDeletePeriod->setVisible(enabled);
        ui->deletePeriodBox->setVisible(enabled);
    });
    ui->labelDeletePeriod->setVisible(ui->autoDeleteCheck->isChecked());
    ui->deletePeriodBox->setVisible(ui->autoDeleteCheck->isChecked());

    connect(ui->reminderCheck, &QCheckBox::stateChanged, this, [this](int state) {
        bool enabled = (state == Qt::Checked);
        ui->labelReminderTime->setVisible(enabled);
        ui->reminderTimeBox->setVisible(enabled);
        ui->labelReminderSound->setVisible(enabled);
        ui->selectSoundButton->setVisible(enabled);
        ui->labelSoundPath->setVisible(enabled);
    });
    ui->labelReminderTime->setVisible(ui->reminderCheck->isChecked());
    ui->reminderTimeBox->setVisible(ui->reminderCheck->isChecked());
    ui->labelReminderSound->setVisible(ui->reminderCheck->isChecked());
    ui->selectSoundButton->setVisible(ui->reminderCheck->isChecked());
    ui->labelSoundPath->setVisible(ui->reminderCheck->isChecked());

    connect(ui->selectSoundButton, &QPushButton::clicked, this, &SettingsWindow::onSelectSoundClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &SettingsWindow::onSaveClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &SettingsWindow::onCancelClicked);
}

SettingsWindow::~SettingsWindow() {
    delete ui;
}

void SettingsWindow::loadSettings() {
    ui->defaultDeadlineBox->setCurrentText(settings.value("defaultDeadline", "Tomorrow").toString());
    ui->autoDeleteCheck->setChecked(settings.value("autoDelete", false).toBool());
    ui->deletePeriodBox->setCurrentText(settings.value("deletePeriod", "After deadline passes").toString());
    ui->themeBox->setCurrentText(settings.value("theme", "Light").toString());
    ui->reminderCheck->setChecked(settings.value("reminderEnabled", false).toBool());
    ui->reminderTimeBox->setValue(settings.value("reminderTime", 30).toInt());

    QString sound = settings.value("reminderSound", "").toString();
    if (sound.isEmpty())
        ui->labelSoundPath->setText("(none selected)");
    else
        ui->labelSoundPath->setText(sound);
}

void SettingsWindow::saveSettings() {
    settings.setValue("defaultDeadline", ui->defaultDeadlineBox->currentText());
    settings.setValue("autoDelete", ui->autoDeleteCheck->isChecked());
    settings.setValue("deletePeriod", ui->deletePeriodBox->currentText());
    settings.setValue("theme", ui->themeBox->currentText());
    settings.setValue("reminderEnabled", ui->reminderCheck->isChecked());
    settings.setValue("reminderTime", ui->reminderTimeBox->value());
    settings.setValue("reminderSound", ui->labelSoundPath->text());
}

void SettingsWindow::onSaveClicked() {
    saveSettings();
    accept();
}

void SettingsWindow::onCancelClicked() {
    reject();
}

void SettingsWindow::onSelectSoundClicked() {
    QString filePath = QFileDialog::getOpenFileName(this,
        "Select Reminder Sound", "",
        "Audio Files (*.wav *.mp3 *.ogg *.flac)");
    if (!filePath.isEmpty()) {
        ui->labelSoundPath->setText(filePath);
    }
}
