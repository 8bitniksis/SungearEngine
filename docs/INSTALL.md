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
| Java **JDK** (не только JRE) | 8+, проверено с Temurin 21 | да | Две причины: (1) корневой CMake делает `find_package(JNI REQUIRED)` — нужны заголовки `jni.h`, `JAVA_HOME` должен указывать на JDK; (2) `Sources/SGCore/CMakeLists.txt` генерирует CSS-парсер командой `java -jar Externals/antlr4/antlr-4.13.1-complete.jar` — нужен `java` в PATH |
| Python | 3.x | да | требуется vcpkg-портами (skia и др.) при сборке зависимостей |
| Ninja | любая | рекомендуется | генератор для пресетов; входит в workload C++ Visual Studio |
| Диск | десятки ГБ | да | vcpkg собирает ~30 зависимостей из исходников (в т.ч. skia и boost) |
| Android NDK | — | только для Android | пресет `arm64-android` |

Первая сборка зависимостей vcpkg — **долгая** (часы на слабой машине из-за
skia/boost/assimp). Это разовая цена: результат кешируется в
`vcpkg/vcpkg_installed/`.

### Установка инструментов одной командой

**Windows** (PowerShell, winget):

```powershell
winget install Microsoft.VisualStudio.2022.Community --override "--add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended --passive"
winget install EclipseAdoptium.Temurin.21.JDK
winget install Python.Python.3.12
winget install Git.Git
```

Workload `NativeDesktop` включает MSVC (C++23), Windows SDK, CMake и Ninja.

**Linux** (Debian/Ubuntu):

```bash
sudo apt install build-essential g++-13 cmake ninja-build git curl zip unzip tar pkg-config \
     python3 default-jdk \
     libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
     libasound2-dev libpulse-dev autoconf automake libtool
```

`g++-13`+ или `clang-17`+ (нужен C++23); `default-jdk` закрывает Java и JNI;
X11/GL/ALSA-dev — системные требования glfw3 и openal-soft из vcpkg.

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

### Выбор графического API

Бэкенд выбирается на старте по списку предпочтения (`GAPISelector`,
см. [RHI_DESIGN.md](./RHI_DESIGN.md#командные-списки-и-кадр)); недоступные
в этой сборке пропускаются с записью в лог. Принудительно задать бэкенд —
переменной окружения `SG_GAPI` (для диагностики драйверных проблем):

```powershell
$env:SG_GAPI = 'gl4'      # Windows, текущая сессия
```

```bash
SG_GAPI=gl46 ./SungearEngine   # Linux
```

Допустимые значения: `gl4`, `gl46`, `gles2`, `gles3`, `vulkan`, `dx12`.
Реально доступны на 2026-08-16: `gl4`, `gl46`; остальные появятся по
этапам плана. Если принудительный бэкенд недоступен, движок откатывается
к списку предпочтения и пишет предупреждение в лог.

---

## 9. Тесты

Тестовые таргеты (`Tests/`, собираются при `SG_BUILD_TESTS`):

```bash
cmake --build <binary-dir> --target SGCoroTest
<binary-dir>/Tests/Coro/SGCoroTest
```

### Смоук-сцена рендера (`SGSmokeTest`)

Эталонная сцена для сравнения графических бэкендов: PBR-тела, CSM-тени,
атмосфера, прозрачный объект, SSAO. Рендерит N кадров, читает кадр обратно
и пишет PNG; с `--reference` сравнивает и возвращает код выхода.

```bash
cmake --build <binary-dir> --target SGSmokeTest
cd <binary-dir>/Tests/Smoke          # рядом должна лежать SGCore.dll (§6)

SGSmokeTest --gapi gl4                          # снять кадр → smoke_gl4.png
SGSmokeTest --gapi gl46 --reference smoke_gl4.png   # сравнить другой бэкенд с эталоном
SGSmokeTest --help
```

Коды выхода: `0` — кадр снят (и совпал с эталоном), `1` — расхождение
(рядом пишется `*_diff.png`), `2` — снять не удалось. Пороги: `--threshold`
(разница по каналу, по умолчанию 8/255) и `--max-diff` (доля отличающихся
пикселей, по умолчанию 1 %). Эталоны обновляются осознанно, при
намеренном изменении картинки — и коммитятся рядом с тестом.

GTest заявлен в `vcpkg.json`, но в `Tests/Coro/CMakeLists.txt` закомментирован —
тест сейчас обычный исполняемый файл. Состояние тестирования — в
[DEV_RULES.md → Тестирование](./DEV_RULES.md#-тестирование).

---

## 10. Диагностика частых проблем

| Симптом | Причина | Что делать |
|---|---|---|
| CMake не находит toolchain / `CMAKE_TOOLCHAIN_FILE` пустой | Не задана `SUNGEAR_SOURCES_ROOT` или IDE её не видит | §4: задать переменную, перезапустить IDE/ПК |
| Конфигурация падает на пустом `vcpkg/` или `Externals/msdf-atlas-gen/` | Не инициализированы сабмодули | `git submodule update --init --recursive` |
| Сборка SGCore падает на шаге «Generating ANTLR4 parser and lexer» (`java: command not found`) | Нет Java в PATH | Установить JRE 8+ и перезапустить IDE (§1) |
| В логе `No graphics API from the preference list is available` | Ни один бэкенд не создался (напр. `SG_GAPI` указывает на нереализованный, а список предпочтения пуст) | Снять `SG_GAPI` или указать `gl4`; см. §8 |
| exe мгновенно завершается / «не найдена SGCore.dll» | DLL не скопирована к исполняемому файлу | §6: скопировать `SGCore.dll` вручную |
| Плагин редактора не загружается | Ядро и редактор собраны разными пресетами | Пересобрать оба одним пресетом |
| Первая конфигурация «висит» часами | vcpkg собирает skia/boost/assimp из исходников | Это норма для первого раза; кеш в `vcpkg/vcpkg_installed/` |
| Ошибки про макрос `ERROR` при сборке с ANTLR4 на Windows | Конфликт WinAPI (wingdi.h) с `ParseTreeType::ERROR` | Уже закрыто: `NOGDI`, `NOMINMAX`, `WIN32_LEAN_AND_MEAN` определены в корневом CMake — не удалять |
| MSVC: `fatal error C1128` (too many sections) | Большие объектники | Уже закрыто флагом `/bigobj` в корневом CMake |
| Кириллица в консоли выводится мусором | Кодировка терминала Windows | `[Console]::OutputEncoding=[Text.Encoding]::UTF8` |

---

*Документ дополняется по мере появления новых шагов развёртывания и разобранных проблем.*
