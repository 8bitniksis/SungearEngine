# ⚙️ INSTALL — развёртывание репозитория

Инструкция для нового участника: как поднять проект с нуля на рабочей станции.

> **Документ живой.** Появляется новая инфраструктурная зависимость или шаг
> сборки — они добавляются сюда сразу же, вместе с кодом, который их использует.
> Порядок описан в [`DEV_RULES.md`](./DEV_RULES.md) и [`USER_RULES.md`](./USER_RULES.md).

## 📋 Оглавление

1. [Что понадобится](#1-что-понадобится)
2. [Клонирование](#2-клонирование)
3. [Bootstrap vcpkg](#3-bootstrap-vcpkg)
4. [Переменная окружения SUNGEAR_SOURCES_ROOT](#4-переменная-окружения-sungear_sources_root)
5. [Конфигурация CMake и пресеты](#5-конфигурация-cmake-и-пресеты)
6. [Сборка ядра](#6-сборка-ядра)
7. [Сборка редактора](#7-сборка-редактора)
8. [Запуск](#8-запуск)
9. [Тесты](#9-тесты)
10. [Диагностика частых проблем](#10-диагностика-частых-проблем)

---

## 1. Что понадобится

| Компонент | Версия | Обязателен | Зачем |
|---|---|---|---|
| CMake | 3.22+ | да | система сборки |
| Компилятор C++23 | MSVC 2022 / Clang / GCC | да | стандарт задан жёстко (`CMAKE_CXX_STANDARD 23`) |
| Git | любая | да | репозиторий + сабмодули (vcpkg, msdf-atlas-gen) |
| Диск | десятки ГБ | да | vcpkg собирает ~30 зависимостей из исходников (в т.ч. skia и boost) |
| Android NDK | — | только для Android | пресет `arm64-android` |

Первая сборка зависимостей vcpkg — **долгая** (часы на слабой машине из-за
skia/boost/assimp). Это разовая цена: результат кешируется в
`vcpkg/vcpkg_installed/`.

---

## 2. Клонирование

```bash
git clone --recurse-submodules -j8 https://github.com/Pixelfield-ru/SungearEngine
cd SungearEngine
```

Если репозиторий уже склонирован без сабмодулей (или скачан архивом):

```bash
git submodule update --init --recursive
```

Сабмодули: `vcpkg/` и `Externals/msdf-atlas-gen` (ветка v1.3). Без них
конфигурация CMake не пройдёт.

---

## 3. Bootstrap vcpkg

```powershell
# Windows
.\vcpkg\bootstrap-vcpkg.bat
```

```bash
# Linux/macOS
./vcpkg/bootstrap-vcpkg.sh
```

Зависимости ставить вручную **не нужно**: проект работает в манифестном
режиме (`vcpkg.json` в корне), установка происходит при конфигурации CMake.
Используются оверлей-триплеты из `cmake/vcpkg-triplets/`.

---

## 4. Переменная окружения SUNGEAR_SOURCES_ROOT

Пресеты CMake и подключение движка из плагинов берут пути из системной
переменной `SUNGEAR_SOURCES_ROOT` — она должна указывать на корень checkout'а:

```powershell
# Windows (PowerShell, от имени пользователя)
[Environment]::SetEnvironmentVariable('SUNGEAR_SOURCES_ROOT', 'E:\Github\SungearEngine', 'User')
```

```bash
# Linux (добавить в ~/.bashrc или ~/.profile)
export SUNGEAR_SOURCES_ROOT=$HOME/dev/SungearEngine
```

⚠️ После установки переменной **перезапустите IDE** (README рекомендует
перезагрузить ПК — важно, чтобы переменную увидел процесс IDE/терминала).
Через неё резолвятся `CMAKE_TOOLCHAIN_FILE`, `VCPKG_INSTALLED_DIR`
и `VCPKG_OVERLAY_TRIPLETS` (см. `cmake/presets/base-presets.json`).

---

## 5. Конфигурация CMake и пресеты

Пресеты подключаются корневым `CMakePresets.json` из `cmake/presets/`:

| Пресет | Платформа | Тип |
|--------|-----------|-----|
| `debug-x64-windows-static-md` | Windows x64 | Debug |
| `release-x64-windows-static-md` | Windows x64 | Release |
| `debug-x64-linux` | Linux x64 | Debug |
| `release-x64-linux` | Linux x64 | Release |
| пресеты из `arm64-android.json` | Android arm64 | — |

⚠️ README упоминает имена `debug-host`/`release-host` — фактические имена
пресетов см. в таблице (источник истины — `cmake/presets/*.json`).

```bash
cmake --preset debug-x64-windows-static-md
```

Первый запуск конфигурации соберёт все зависимости vcpkg — см. §1 про время.

Флаги (сейчас захардкожены в `ON` в корневом `CMakeLists.txt`):

- `SG_BUILD_TESTS` — сборка тестовых таргетов (`Tests/`);
- `SG_BUILD_ENTRY` — сборка исполняемого файла `SGEntry`.

---

## 6. Сборка ядра

```bash
cmake --build cmake-build-debug-x64-windows-static-md --target SGCore
```

Либо в IDE: открыть корневой `CMakeLists.txt`, выбрать пресет и таргет.

⚠️ **Windows**: после сборки скопируйте
`<binary-dir>/Sources/SGCore/SGCore.dll` в каталог с исполняемым файлом,
который собираетесь запускать. Функция `copy_sgcore_dlls()` в
`cmake/utils.cmake` сейчас закомментирована, автоматического копирования нет.

После каждой сборки срабатывает таргет `auto_install` — он выполняет
`cmake --install` в `CMAKE_INSTALL_PREFIX`.

---

## 7. Сборка редактора

Редактор — отдельный CMake-проект в `Plugins/SungearEngineEditor/`.

1. Убедиться, что ядро собрано **тем же пресетом**, каким будет собираться
   редактор — иначе бинарная несовместимость.
2. Открыть `Plugins/SungearEngineEditor/` как CMake-проект.
3. Выбрать пресет и таргет, собрать.

Движок редактор находит через `SUNGEAR_SOURCES_ROOT`
(`cmake/SungearEngineInclude.cmake`).

---

## 8. Запуск

1. Собрать CMake-таргет `SungearEngine` (SGEntry).
2. Запустить `<binary-dir>/Sources/SGEntry/SungearEngine` — точка входа
   загрузит плагин редактора через `PluginsManager`.

На Windows не забыть про `SGCore.dll` рядом с exe (§6).

---

## 9. Тесты

Единственный тестовый таргет — `SGCoroTest` (`Tests/Coro/`, корутины):

```bash
cmake --build <binary-dir> --target SGCoroTest
<binary-dir>/Tests/Coro/SGCoroTest
```

GTest заявлен в `vcpkg.json`, но в `Tests/Coro/CMakeLists.txt` закомментирован —
тест сейчас обычный исполняемый файл. Состояние тестирования — в
[DEV_RULES.md → Тестирование](./DEV_RULES.md#-тестирование).

---

## 10. Диагностика частых проблем

| Симптом | Причина | Что делать |
|---|---|---|
| CMake не находит toolchain / `CMAKE_TOOLCHAIN_FILE` пустой | Не задана `SUNGEAR_SOURCES_ROOT` или IDE её не видит | §4: задать переменную, перезапустить IDE/ПК |
| Конфигурация падает на пустом `vcpkg/` или `Externals/msdf-atlas-gen/` | Не инициализированы сабмодули | `git submodule update --init --recursive` |
| exe мгновенно завершается / «не найдена SGCore.dll» | DLL не скопирована к исполняемому файлу | §6: скопировать `SGCore.dll` вручную |
| Плагин редактора не загружается | Ядро и редактор собраны разными пресетами | Пересобрать оба одним пресетом |
| Первая конфигурация «висит» часами | vcpkg собирает skia/boost/assimp из исходников | Это норма для первого раза; кеш в `vcpkg/vcpkg_installed/` |
| Ошибки про макрос `ERROR` при сборке с ANTLR4 на Windows | Конфликт WinAPI (wingdi.h) с `ParseTreeType::ERROR` | Уже закрыто: `NOGDI`, `NOMINMAX`, `WIN32_LEAN_AND_MEAN` определены в корневом CMake — не удалять |
| MSVC: `fatal error C1128` (too many sections) | Большие объектники | Уже закрыто флагом `/bigobj` в корневом CMake |
| Кириллица в консоли выводится мусором | Кодировка терминала Windows | `[Console]::OutputEncoding=[Text.Encoding]::UTF8` |

---

*Документ дополняется по мере появления новых шагов развёртывания и разобранных проблем.*
