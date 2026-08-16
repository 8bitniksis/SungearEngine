---
name: sgcore-graphics
description: >-
  Когда использовать: любая работа с графическим слоем SGCore — Graphics/API
  (IRenderer, IFrameBuffer, ITexture2D, IShader, GAPIType, GAPISelector),
  GL-бэкенды (GL4Renderer/GL46Renderer, GL4FrameBuffer, GL4Texture2D), рендер-проходы
  (Render/PBRRP, CSM, Batching), шейдеры SGSL (Resources/sg_shaders, SGSLETranslator),
  readback/скриншоты, смоук-тест SGSmokeTest, выбор бэкенда через SG_GAPI, ECS-компоненты
  рендера (Camera3D, LayeredFrameReceiver, CSMTarget, Controllable3D, Atmosphere, Mesh).
  Проверенные факты и грабли: форматы attachment'ов (не предполагать RGBA8), кто отбрасывает
  тени (только Batch), Controllable3D перезаписывает поворот камеры, размер фреймбуфера
  = размер монитора, GL46Renderer сломан, GAPIType/SGG*-enum'ы глобальные, protected-поля
  IMaterial, компоненты хранятся по значению.
  Triggers: framebuffer, attachment, glReadPixels, readback, screenshot, texture format,
  SGGColorFormat, SGGDataType, IRenderer, GAPIType, SG_GAPI, GL46, shadows, CSM, Batch,
  smoke test, SGSL, sgshader, uniform, sampler, LayeredFrameReceiver, Controllable3D,
  RHI, Vulkan backend, DX12 backend.
---

# SGCore: графический слой — проверенные факты и грабли

Скилл накапливает то, что **проверено кодом, сборкой или запуском** (даты — когда
проверено). Догадка о поведении API здесь стоит дороже одного чтения: если факта
нет — смотреть исходники, а не предполагать. Дизайн целевого RHI —
[`docs/RHI_DESIGN.md`](../../../docs/RHI_DESIGN.md), аудит —
[`docs/RHI_AUDIT.md`](../../../docs/RHI_AUDIT.md).

## 📋 Структура документа
- [Форматы, enum'ы, кастеры](#форматы-enumы-кастеры)
- [Attachment'ы и readback](#attachmentы-и-readback)
- [Выбор бэкенда](#выбор-бэкенда)
- [Состояние бэкендов](#состояние-бэкендов)
- [ECS-компоненты рендера: что где перезаписывается](#ecs-компоненты-рендера-что-где-перезаписывается)
- [Тени и батчинг](#тени-и-батчинг)
- [Шейдеры SGSL](#шейдеры-sgsl)
- [Смоук-тест](#смоук-тест)
- [Мелкие грабли API](#мелкие-грабли-api)

---

## Форматы, enum'ы, кастеры

- `SGGColorFormat`, `SGGColorInternalFormat`, `SGGDataType`, `SGFrameBufferAttachmentType`
  и прочие `SGG*` объявлены **глобально** в `Graphics/API/GraphicsDataTypes.h`, а не в
  `namespace SGCore`. `SGCore::SGGColorFormat` — ошибка компиляции (проверено 2026-08-17).
- `GAPIType` — unscoped enum в `SGCore` (`SG_API_TYPE_GL4/GL46/GLES2/GLES3/VULKAN/DX12`);
  хелперы `isOpenGLAPI()`, `isExplicitAPI()`, `gapiTypeToString()`, `gapiTypeFromString()`
  там же (`Graphics/API/GAPIType.h`). Сравнения диапазонов enum'а (`>= GL4 && <= GLES3`)
  не писать — использовать хелперы.
- Перевод в GL — только через `GLGraphicsTypesCaster` (`Graphics/API/GL/GLGraphicsTypesCaster.h`):
  `sggFormatToGL`, `sggDataTypeToGL`, `sggInternalFormatToGL`. Размеры — хелперы в
  `GraphicsDataTypes.h`: `getSGGFormatChannelsCount(format)`,
  `getSGGDataTypeSizeInBytes(dataType)`, `getSGGInternalFormatChannelsSizeInBytes(internalFormat)`.
- Все 244 прямых `gl*`-вызова живут внутри `Graphics/API/GL/` (аудит 2026-08-16); вне
  бэкенда GL не вызывать.

## Attachment'ы и readback

- **У attachment'а свой формат.** `ITexture2D` хранит `m_format` (R/RG/RGB/RGBA/BGR/BGRA,
  `*_INTEGER`, depth/stencil), `m_internalFormat`, `m_dataType`, `m_channelsCount`.
  Attachment может быть `R`, `RG`, integer (ID пикинга) или float. **Никогда не
  предполагать RGBA8** — замечание Ilya 2026-08-16 к первой версии readback'а.
- Полное чтение attachment'а: `IFrameBuffer::readAttachmentPixels(type, AttachmentReadback&)`
  — читает **в родном формате** (`glReadPixels` с `sggFormatToGL(m_format)` /
  `sggDataTypeToGL(m_dataType)`), возвращает байты + раскладку. Приведение к RGBA8 —
  на вызывающей стороне (пример: `Tests/Smoke/ImageCompare.cpp::toRGBA8`). Для integer
  attachment'ов чтение как `GL_RGBA/GL_UNSIGNED_BYTE` = `GL_INVALID_OPERATION`.
- Однопиксельное чтение под мышью — `readPixelsFromAttachment(mousePos, type)` (пикинг),
  возвращает `glm::vec3` через `GL_FLOAT`.
- Строки в readback'е GL идут **снизу вверх** — для PNG нужен флип
  (`isOpenGLAPI(...) → flipVertically`).
- **Размер фреймбуферов `LayeredFrameReceiver` = разрешение основного монитора**
  (`LayeredFrameReceiver.cpp:47,142`), не окна. Скриншот на 1080p-мониторе — 1920×1080
  независимо от размера окна; менять размер окна ради размера кадра бесполезно.
- Финальный кадр камеры — `LayeredFrameReceiver::m_layersFXFrameBuffer`, attachment
  `SGG_COLOR_ATTACHMENT7` (`BasicApp::m_attachmentToDisplay`).

## Выбор бэкенда

- Бэкенд выбирает `GAPISelector::selectRenderer()` (`Graphics/API/GAPISelector.h`,
  вызывается из `CoreMain::init()`): переменная окружения `SG_GAPI` (`gl4`, `gl46`,
  `vulkan`, `dx12`…) → список предпочтения (`setPreference()` до `init()`, платформенный
  дефолт). Недоступные в сборке бэкенды пропускаются с логом; при откате — предупреждение.
- Рантайм-откат с пересозданием окна пока не реализован (задача 1.3a плана):
  `GL4Renderer::init()` при провале `confirmSupport()` вызывает `setShouldClose(true)`.
- Окно: для `isExplicitAPI` ставится `GLFW_CLIENT_API = GLFW_NO_API` (`Main/Window.cpp`).
- Проверить выбор из теста: `SGSmokeTest --gapi gl46` — использует `setPreference({type})`.

## Состояние бэкендов

- **`GL46Renderer` — основной GL-бэкенд** (решение 2026-08-17): GL4-реализация на
  контексте 4.6, `#version 460 core` + дефайн `SG_GLSL4`; переопределяет только
  `confirmSupport`/`createShader`. `GL4Renderer` (`#version 400 core`) — второй в списке.
  Смоук на gl46 и gl4 совпадает попиксельно (общий эталон `smoke_gl_1920x1080.png`).
- Грабли, из-за которых GL46 был «сломан» до 2026-08-17: (1) `createShader()` без
  `addDefine("SG_GLSL4")` — весь код шейдеров под `#if defined(SG_GLSL4)` исчезает,
  стадии пустые → «No input primitive type» и чёрный кадр; **любой новый GL-бэкенд
  обязан добавлять этот дефайн**; (2) `GL46Texture2D` — заглушка 2023 г.
  (`glTextureParameteri(GL_GENERATE_MIPMAP)` = `GL_INVALID_ENUM`, захардкоженный
  `GL_UNSIGNED_BYTE`, нет buffer/3D/compressed-путей) — удалена, DSA возвращать
  пообъектно под смоук-контролем. `GL46FrameBuffer`/`GL46UniformBuffer` не используются
  (`GL4Renderer` создаёт GL4-варианты).
- Первым симптомом «шейдер пустой» в логе будет ошибка компиляции геометрической стадии
  про примитивы, а не про сам код — проверять дефайны раньше, чем GLSL.
- `Graphics/API/Vulkan/` — нерабочий скелет 2023 г., `vulkan.h` закомментирован,
  зависимости в `vcpkg.json` нет; `VkRenderer::confirmSupport()` возвращает `false`.
  Не достраивать — переписывать (этап 2).
- ImGui-бэкенд захардкожен на `imgui_impl_opengl3` (`ImGuiWrap/ImGuiLayer.cpp`).

## ECS-компоненты рендера: что где перезаписывается

- Компоненты хранятся **по значению** (`reg_t` = сам тип): `registry->get<T>(e)` — ссылка,
  `tryGet<T>(e)` — сырой указатель, `emplace/remove/allOf` — как у EnTT.
  `tryGet(entt::null)` в Debug ассертит — проверять сущность на `entt::null`.
- **`Controllable3D` каждый кадр перезаписывает `Transform::m_localTransform.m_rotation`**
  из своих `m_pitchYawRoll` (`Controllables3DUpdater.cpp:44`), а те скачут от дельт
  курсора на первых кадрах. Для фиксированной позы камеры компонент снять
  (`registry->remove<Controllable3D>(camera)`) либо `m_localTransform.m_blockRotation = true`.
- Камера: `Transform + Camera3D + RenderingBase + LayeredFrameReceiver`
  (+ `Controllable3D` для управления, + `CSMTarget` для теней) — см. `BasicApp::initImpl`.
- Конвенции осей: `MathUtils::forward3 = (0,0,-1)`, `up3 = (0,1,0)`, `right3 = (-1,0,0)`.
- `Atmosphere` — компонент на скайбокс-меше; солнце задаётся `m_sunRotation` (градусы,
  `AtmosphereUpdater` вращает по Z, затем по Y); `{0, 35, 40}` даёт низкое солнце.
- `BasicApp::start(true)` создаёт: пайплайн PBRRP, сцену, скайбокс+`Atmosphere`
  (`m_atmosphereEntity` = `skyboxEntities[2]`, может остаться `entt::null`, если не
  загрузился `cube_model`), камеру с SSAO. Конфиг `SungearEngineConfig.json` — не
  обязателен (ошибка в логе, работает дальше).
- Импорт модели: `ModelAsset::m_rootNode->addOnScene(scene)` возвращает сущности,
  `[0]` — корень (его `Transform` двигать/масштабировать), меши — дочерние с `Mesh`,
  `Transform`, `Pickable`, `EnableMeshPass` и тегом `OpaqueEntityTag`/`TransparentEntityTag`
  **по материалу меша на момент импорта** (`IMeshData.cpp:229`). Сменил материал на
  `MAT_BLEND` — перетегируй руками.

## Тени и батчинг

- **`SunShadowsPass` рендерит в каскады только `view<Batch, ShadowCaster>`**
  (`Render/BaseRenderPasses/SunShadowsPass.cpp`) — обычные меши с `ShadowCaster` теней
  не отбрасывают. `CSMTarget` на камере + `ShadowCaster` на мешах недостаточно; нужен
  `Batch` (`Batch::insertEntities`). Батчинг — отдельная подсистема (атлас, TBO), меши
  в батче продолжают рендериться и обычным проходом, пока не снят `EnableMeshPass`.
  Правильный путь для теней от обычных мешей — открытый вопрос (1.6a плана).
- CSM читается в `PBRRPOpaqueMeshesPass` через `CSMTarget::bindUniformsToShader`.

## Шейдеры SGSL

- SGSL (`Utils/SGSL/SGSLETranslator`) — препроцессор над GLSL: `#include`,
  `#vertex/#fragment/#geometry/#compute/#tess_control/#tess_eval … #end`,
  `#subpass[Name]` (обязателен), `#attribute[name][value]` (становятся `#define`).
  Код вне `#…#end` — общий для всех субшейдеров. Транслятор **не** генерирует
  `#version` и биндинги — это делает GL-шейдер при компиляции
  (`GL46Shader::compileSubShader`: `#version <m_version>` + дефайны + код).
- Корпус (2026-08-17): 24 `.sgshader` + 59 `.glsl` в `Resources/sg_shaders/`;
  **140 свободных не-opaque `uniform`** вне блоков, **100 сэмплеров без `binding`**,
  8 UBO (`layout(std140)`) без `binding`, 9 `samplerBuffer` (батчинг через TBO),
  10 `gl_FragColor`, 8 `gl_VertexID`, 17 файлов с юниформами под `#if`. Всё это
  несовместимо с Vulkan-GLSL напрямую — нужен проход «вулканизации» (задача 1.4).
- Include-корень: `SGSLETranslator::includeDirectory(<root>/Resources)`; общие файлы —
  `sg_shaders/impl/glsl4/{defines,structs_decl,uniform_bufs_decl}.glsl`.
- Runtime-перекомпиляция: `IShader::m_autoRecompile`.
- Каждый `.sgshader` обёрнут в guard варианта `#if defined(SG_GLSL4) || defined(SG_GLES32)`
  (ветка `SG_HLSL` — `#error`), внутри — `#include` реализации из `impl/glsl4/`.
  SGSL-препроцессор разворачивает `#include` **не глядя на `#if`** — условия
  остаются в тексте для GLSL-компилятора.
- `SGSLETranslator::processCode` работает без окна/GPU (инклуды через `AssetManager`
  как `TextFileAsset`), но **всегда** пишет дамп в `SGSLETranslatorOutputDebug/` и
  `logs/` в текущий каталог (флаг `m_useOutputDebug` не проверяется) — запускать
  инструменты из build-dir.
- **Вулканизация** (`Utils/SGSL/SGSLEVulkanizer`, 2026-08-17): переводит стадии
  программы в Vulkan-GLSL — свободные юниформы стадии → std140-блок
  `SGLegacyUniforms_<stage>` с экземпляром `sg_SGLegacyUniforms_<stage>`,
  обращения переписаны в `instance.member`; `set/binding` сэмплерам/UBO/TBO
  (явные сохраняются), `#if`-условия членов сохраняются с вырезанным общим
  guard'ом файла, блок ставится не раньше объявления `struct`-типов членов,
  `gl_FragColor`→`sgFragColor` + `out`, `gl_VertexID`→`gl_VertexIndex`.
  Обрабатывать **всю программу разом** — биндинги должны совпасть между стадиями.
  Дефайны приписывать до вулканизации. Массивы сэмплеров с макро-размером
  (`[SG_SPOT_LIGHTS_MAX_COUNT]`) дают предупреждение и count = 1 — брать из
  рефлексии. Тест: `Tests/Shaders` (`--corpus`, `--spirv`).
- **Почему блок пер-стадийный, а не общий** (грабли 2026-08-17): (1) стадия видит
  только свои `struct`-типы — `screen.glsl`/`div.glsl` инклудят
  `uniform_bufs_decl.glsl` только в вершинной стадии, общий блок с
  `ObjectTransform` не компилировался во фрагментной; (2) члены **анонимных**
  блоков глобальны для программы — glslang не линкует две стадии, где одно имя
  принадлежит разным анонимным блокам («Anonymous member name used for global
  variable or other anonymous member»). Отсюда именованный экземпляр и
  переписывание обращений. Фасад `useX("имя")` пишет во все блоки с таким членом.
- **SPIR-V + рефлексия** (`Graphics/SPIRV/SPIRVCompiler`, `ShaderReflection`):
  glslang 15.1, `EShClientVulkan`/`EShTargetVulkan_1_3`/`EShTargetSpv_1_6`,
  `setAutoMapLocations(true)` + `program.mapIO()` — авто-локации in/out
  согласованы между стадиями (шейдеры движка их не пишут); SPIRV-Reflect даёт
  set/binding, тип дескриптора, count, члены блоков (offset/size/paddedSize),
  вершинные входы, push constants; блоки именуются **именем типа блока**, не
  экземпляра. Проверено: 23/23 программ корпуса компилируются.
  vcpkg-цели: `glslang::glslang`, `glslang::SPIRV`,
  `glslang::glslang-default-resource-limits`, `unofficial::spirv-reflect`
  (не `unofficial::spirv-reflect::spirv-reflect`).
- `features/pbr/instancing.sgshader` — мёртвый: инклудит несуществующий
  `impl/glsl4/pbr/instancing.glsl` (0 стадий при трансляции).

## RHI (новый слой, 2026-08-17)

- Интерфейсы — `Graphics/RHI/` (`IDevice`, `ICommandList`, `ISwapchain`, `IGPUBuffer`,
  `IShaderProgram`, `IPipelineState`/`PipelineStateDesc`, `IDescriptorSet`, `RHITypes.h`);
  доступ — `CoreMain::getRenderer()->getDevice()` (nullptr у бэкендов без RHI; у GL46 —
  после `init()`). Дизайн — `docs/RHI_DESIGN.md`.
- GL46-реализация — `Graphics/API/GL/GL46/RHI/`: команды исполняются немедленно;
  `bindPipeline` применяет `RenderState`/`BlendingState`/`MeshRenderState` через
  кеширующие `GL4Renderer::use*` (общее «текущее состояние» с legacy-проходами),
  `glUseProgram`, `glBindVertexArray`; VAO принадлежит PSO и хранит только формат
  (DSA `glVertexArrayAttribFormat`), буферы — `bindVertexBuffer` → `glVertexArrayVertexBuffer`
  (нужен **после** `bindPipeline`). Дескрипторный набор = список `glBindBufferBase`/
  `glBindTextureUnit`, номер набора игнорируется (плоское пространство GL).
- **Единый шейдерный путь**: `SGSLEVulkanizer::Target::OPENGL` — GLSL с
  `layout(binding=N)` без `set`, `gl_VertexID` сохранён; GL 4.6 принимает как есть.
  `GL46ShaderProgram` компилирует его (`#version 460 core` приписывается, если нет) и
  **рефлектит через program interface query** (`GL_UNIFORM_BLOCK`/`GL_UNIFORM`/
  `GL_PROGRAM_INPUT`) — на GL это источник истины, glslang не нужен. Грабли: члены
  именованного блока GL отдаёт как `Block.member` — префикс срезается; массивы — `name[0]`.
- Push constants на GL не реализованы (предупреждение) — фасад заменит мини-UBO;
  `transition` — no-op; `destroyDeferred` — просто сброс ссылки; `waitIdle` = `glFinish`.
- Проверка — `Tests/RHI` (`SGRHITest --gapi gl46`): точные пиксели после readback.
- **Загрузка шейдера в RHI** — `RHIShaderLoader::load(device, "${enginePath}/…/x.sgshader",
  defines)`: `ShaderAnalyzedFile` через `AssetManager` → дефайны бэкенда (`SG_GLSL4` /
  `SG_GLES32` по `DeviceProperties::m_apiType`) + `#attribute` шейдера → вулканизатор
  под диалект → `createShaderProgram` (для explicit API ещё `SPIRVCompiler`). Обращаться
  к юниформам **только через рефлексию** (`findBinding`, `findMember`), имена блоков —
  `SGLegacyUniforms_<stage>`.
- **Первый RHI-проход — `Graphics/RHI/ScreenBlit`** (вывод текстуры на экран, вместо
  legacy-квада `IRenderer::renderTextureOnScreen`): `GL46Renderer` переопределяет
  `renderTextureOnScreen`, инициализирует `ScreenBlit` лениво при первом вызове (нужен
  `AssetManager`), при провале откатывается на legacy с ошибкой в логе. Legacy-текстуры
  в дескрипторный набор передаются как `Ref` с пустым делитером (не владеем).
  `IRenderer::readScreenPixels()` читает backbuffer (GL: `glReadPixels` с FB 0, RGBA8,
  строки снизу вверх). `AttachmentReadback` — в `Graphics/API/AttachmentReadback.h`.
- **`GL46Shader` в RHI-режиме** (`m_useRHIUniforms`, ставит `GL46Renderer::createShader`): код
  стадий проходит вулканизатор (OPENGL, `m_firstBinding = 8` — точки 1–4 у legacy-`IUniformBuffer`,
  которые перекрывают `layout(binding)` через `glUniformBlockBinding`), после линковки —
  `GL46ShaderProgram::reflectProgram`, UBO на каждый `SGLegacyUniforms_<stage>` (нули +
  дефолты из `Report::m_defaults`), `useX("имя")` → `glNamedBufferSubData` по offset/stride
  (`name[i]` — элемент массива, stride из `BlockMember::m_paddedSize`; юниформ в нескольких
  стадиях пишется во все блоки). Сэмплеры (`useTextureBlock`, `useTexture`) — по-прежнему
  `glUniform1i`: `layout(binding)` у сэмплера — лишь начальное значение, glUniform его перекрывает.
  `bind()` = `glUseProgram` + `glBindBufferBase` legacy-блоков. GL4Renderer этот режим не включает.
- Правило миграции: проход перенесён, только когда `SGSmokeTest --gapi gl46 --reference`
  даёт 0 %; legacy-путь остаётся откатом до конца этапа.

## Смоук-тест

- `Tests/Smoke` → `SGSmokeTest` (`--gapi`, `--frames`, `--output`, `--reference`,
  `--threshold`, `--max-diff`, `--help`; коды выхода 0/1/2). Эталоны —
  `Tests/Smoke/references/smoke_<gapi>_<WxH>.png`, привязаны к разрешению монитора.
- GL4 детерминирован: повторный прогон 0 % расхождений (SSAO и стохастическая
  прозрачность воспроизводимы).
- Запуск — из каталога `Tests/Smoke` внутри build-dir, рядом должна лежать `SGCore.dll`
  (`copy_sgcore_dlls()` в `cmake/utils.cmake` закомментирована — копировать руками).
- Первый кадр после старта — не эталон: захват на 60-м кадре (прогрев шейдеров/CSM).

## Мелкие грабли API

- `IMaterial`: `m_metallicFactor`/`m_roughnessFactor`/цвета — **protected**; использовать
  `setMetallicFactor()`, `setRoughnessFactor()`, `setDiffuseColor()`. Публичные:
  `m_shaders`, `m_meshRenderState`, `m_transparencyType`.
- `WindowConfig` — поля private (`friend struct Window`), менять через сеттеры `Window`.
- `AssetManager`: `getAsset<T, AssetStorageType::BY_ALIAS>("alias")`,
  `loadAssetWithAlias<T>("alias", "${enginePath}/...")`, `getOrAddAssetByAlias<T>("alias")`.
  Пути с `${enginePath}` резолвятся через `PathInterpolationMarkupSpec` (ключ выставляет
  `CoreMain::init` из `SUNGEAR_SOURCES_ROOT`).
- Логгер: `SG_LOG_I/W/E/C("fmt {}", args...)` — `fmt::format_string`, `std::string_view`
  форматируется; без аргументов работает (`##__VA_ARGS__`).
- `Ref<T>` = `std::shared_ptr<T>` из `Main/CoreGlobals.h` (нет `Utils/Ref.h`).
- Сборка тянет ANTLR-регенерацию по mtime (нужен `java`); она меняет только путь в
  заголовке 12 файлов `UI/ANTLR4CSS3Generated/` — откатывать перед коммитом.
