#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QDate>
#include <QString>

class AppSettings {
public:
    enum class DefaultDeadline { None, Today, Tomorrow, In3Days, InWeek };
    enum class AutoDeletePeriod { Immediately, After1Day, After1Week, AfterDeadline };
    enum class Theme { Light, Dark };

    static QSettings& s() {
        static QSettings settings("ToDoSoft", "ToDoManager");
        return settings;
    }

    static DefaultDeadline defaultDeadline() {
        const QString v = s().value("defaultDeadline", "Tomorrow").toString();
        if (v == "Today") return DefaultDeadline::Today;
        if (v == "In 3 days") return DefaultDeadline::In3Days;
        if (v == "In a week") return DefaultDeadline::InWeek;
        if (v == "None") return DefaultDeadline::None;
        return DefaultDeadline::Tomorrow;
    }
    static void setDefaultDeadline(DefaultDeadline d) {
        QString v = "Tomorrow";
        if (d == DefaultDeadline::None) v = "None";
        else if (d == DefaultDeadline::Today) v = "Today";
        else if (d == DefaultDeadline::In3Days) v = "In 3 days";
        else if (d == DefaultDeadline::InWeek) v = "In a week";
        s().setValue("defaultDeadline", v);
    }

    static bool autoDelete() { return s().value("autoDelete", false).toBool(); }
    static void setAutoDelete(bool on) { s().setValue("autoDelete", on); }

    static AutoDeletePeriod deletePeriod() {
        const QString v = s().value("deletePeriod", "After deadline passes").toString();
        if (v == "Immediately") return AutoDeletePeriod::Immediately;
        if (v == "After 1 day") return AutoDeletePeriod::After1Day;
        if (v == "After 1 week") return AutoDeletePeriod::After1Week;
        return AutoDeletePeriod::AfterDeadline;
    }
    static void setDeletePeriod(AutoDeletePeriod p) {
        QString v = "After deadline passes";
        if (p == AutoDeletePeriod::Immediately) v = "Immediately";
        else if (p == AutoDeletePeriod::After1Day) v = "After 1 day";
        else if (p == AutoDeletePeriod::After1Week) v = "After 1 week";
        s().setValue("deletePeriod", v);
    }

    static Theme theme() {
        const QString v = s().value("theme", "Light").toString();
        return (v == "Dark") ? Theme::Dark : Theme::Light;
    }
    static void setTheme(Theme t) { s().setValue("theme", t == Theme::Dark ? "Dark" : "Light"); }

    static bool reminderEnabled() { return s().value("reminderEnabled", false).toBool(); }
    static void setReminderEnabled(bool on) { s().setValue("reminderEnabled", on); }

    static int reminderMinutes() { return s().value("reminderTime", 30).toInt(); }
    static void setReminderMinutes(int m) { s().setValue("reminderTime", m); }

    static QString reminderSound() { return s().value("reminderSound", "").toString(); }
    static void setReminderSound(const QString& path) { s().setValue("reminderSound", path); }

    static QDateTime computeQuickAddDeadline(QDateTime today = QDateTime::currentDateTime()) {
        switch (defaultDeadline()) {
            case DefaultDeadline::None:       return QDateTime();
            case DefaultDeadline::Today:      return today;
            case DefaultDeadline::Tomorrow:   return today.addDays(1);
            case DefaultDeadline::In3Days:    return today.addDays(3);
            case DefaultDeadline::InWeek:     return today.addDays(7);
        }
        return today.addDays(1);
    }
};

#endif //APPSETTINGS_H
