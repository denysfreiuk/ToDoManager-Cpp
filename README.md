# ToDoManager-Cpp

**ToDoManager-Cpp** — це кросплатформовий десктопний застосунок, розроблений мовою **C++** із використанням **Qt 6**, який дозволяє ефективно керувати щоденними завданнями.  
Програма має підтримку **облікових записів користувачів**, **безпечної автентифікації**, **системи налаштувань**, а також **збереження даних у базі SQLite**.

---

## Основні можливості

-  Реєстрація та вхід користувача з валідацією введених даних
-  Повноцінна система завдань (створення, редагування, видалення, фільтрація)
-  Збереження усіх даних у **SQLite** локальній базі
-  Повна підтримка **Qt UI / QSS тем** (світла та темна)
-  Власна реалізація **frameless window** з анімаціями та snap preview
-  Вікно налаштувань із збереженням конфігурації
-  Інтеграція **GoogleTest** для юніт-тестування
-  Система логування подій у файл і консоль

---

##  Структура проєкту

```
ToDoManager-Cpp/
│
├── main.cpp
├── CMakeLists.txt
│
├── accounts/               # Класи користувачів і менеджер авторизації
│   ├── account.h / .cpp
│   ├── authManager.h / .cpp
│
├── authWindow/             # Вікна логіну / реєстрації
│   ├── loginWindow.ui / .h / .cpp
│   ├── registerWindow.ui / .h / .cpp
│
├── mainWindow/             # Головне вікно та UI елементи
│   ├── mainwindow.ui / .h / .cpp
│   ├── taskEditorWindow.ui / .h / .cpp
│   ├── taskItemWidget.ui / .h / .cpp
│
├── databaseManager/        # Класи для роботи з базою даних SQLite
│   ├── databaseManager.h / .cpp
│   ├── accountRepository.h / .cpp
│   ├── TaskRepository.h / .cpp
│   ├── SQLUtilities/
│       ├── SQLUtils.h / .cpp
│
├── tasks/                  # Менеджер завдань
│   ├── task.h / .cpp
│   ├── TaskManager.h / .cpp
│
├── settings/               # Вікно та логіка налаштувань
│   ├── settingsWindow.ui / .h / .cpp
│   ├── appSettings.h / .cpp
│
├── windowEdit/             # Frameless window та snap preview
│   ├── framelessWindow.h / .cpp
│   ├── snapPreviewWindow.h / .cpp
│
├── logger/                 # Логер
│   ├── logger.h / .cpp
│   ├── globalLogger.h / .cpp
│
├── tests/                  # GoogleTest тести
│   ├── CMakeLists.txt
│   ├── test_login_register_qt.cpp
│   ├── test_task_repository.cpp
│   ├── ...
│
├── resources.qrc           # Іконки, звуки, стилі
└── README.md
```

---

##  Використані технології

| Компонент | Опис |
|------------|------|
| **C++17** | Основна мова програмування |
| **Qt 6 (Widgets, Multimedia)** | UI, події, інтерфейс, ресурси |
| **SQLite** | Зберігання даних користувачів і завдань |
| **GoogleTest (v1.14)** | Юніт-тестування |
| **CMake** | Система збірки |
| **QSS** | Стилізація інтерфейсу (Light/Dark теми) |

---

## Як зібрати застосунок

###  Вимоги
- **Qt 6.10.0+** (MinGW toolset)
- **CMake 3.16+**
- **SQLite3 (DLL)** у каталозі `C:/libs/sqlite`

###  Кроки
1. Клонуйте репозиторій або розпакуйте архів
2. Відкрийте проєкт у **Qt Creator** або **CLion**
3. Додайте файл `CMakeLists.txt` (якщо не існує)
4. Виконайте збірку:
   ```bash
   cmake -B build -S .
   cmake --build build
   ```
5. Після збірки автоматично запуститься **windeployqt** для копіювання залежностей Qt.

---

##  Запуск тестів

Проєкт має інтегровану систему **GoogleTest**.  
Для збірки тестів необхідно ввімкнути опцію `BUILD_TESTS` у `CMakeLists.txt`:

```cmake
option(BUILD_TESTS "Build unit tests" ON)
```

Після компіляції:
```bash
cd build/tests
ctest --output-on-failure
```

або безпосередньо запустити виконуваний файл тестів:
```bash
./QtEx_tests
```

---

##  Основні класи

| Клас | Призначення |
|------|--------------|
| **AuthManager** | Реєстрація, авторизація користувачів |
| **TaskManager** | Керування завданнями |
| **DatabaseManager** | Ініціалізація та робота з SQLite |
| **Logger** | Запис логів (файл + консоль) |
| **SettingsWindow** | Графічне вікно налаштувань |
| **FramelessWindow** | Вікно без рамки з кастомним title bar |
| **TaskItemWidget** | Віджет для відображення одного завдання |

---

##  Теми інтерфейсу

Програма підтримує дві візуальні теми:
-  **Light Theme** — мінімалістичний білий дизайн
-  **Dark Theme** — темна стильна тема з акцентами синього кольору

Перемикання здійснюється через кнопку у головному меню або через налаштування.

---

## Логування

Всі події (авторизація, операції із завданнями, помилки) записуються у файл `log.txt`.  
Також логер підтримує кольоровий вивід у консоль із рівнями: `INFO`, `DEBUG`, `WARN`, `ERROR`.

---

##  Автор

**Денис Фреюк**  
 GitHub: [denysfreiuk](https://github.com/denysfreiuk)

---

##  Ліцензія

Цей проєкт є відкритим та доступний для навчальних і демонстраційних цілей.
