# Фреймворк GStreamer. Руководство разработчика приложений. Шина сообщений [h1-bus]

Оригинал: GStreamer Application Development Manual  
Авторы: Wim Taymans, Steve Baker, Andy Wingo, Ronald S. Bultje, Stefan Kost  
Дата публикации: 21 мая 2014 г.  
Перевод: А.Панин  
Дата перевода: 19 июня 2014 г.  
Оригинал перевода: [Фреймворк GStreamer. Руководство разработчика приложений. Шина сообщений](http://rus-linux.net/MyLDP/BOOKS/gstreamer/07-bus.html)

## Глава 7. Шина сообщений [bus]

Шина сообщения является простой системой, ответственной за передачу сообщений от программных потоков, предназначенных для обработки мультимедийных потоков, в контекст программного потока приложения. Преимущество использования шины сообщений заключается в отказе от обязательного создания многопоточных приложений для работы с фреймворком GStreamer, несмотря на то, что сам фреймворк GStreamer активно использует программные потоки.

Каждый конвейер по умолчанию содержит шину сообщений, поэтому приложения не должны создавать шины сообщений или другие примитивы взаимодействия самостоятельно. Единственная задача, которую должно выполнить приложение, заключается в установке обработчика для передаваемых посредством шины сообщений, причем этот обработчик аналогичен обработчику сигналов для объекта. В процессе работы главного цикла обработки событий шина сообщений будет периодически проверяться на наличие новых сообщений и функция обратного вызова будет активироваться каждый раз, когда будет доступно новое сообщение.

### 7.1. Как использовать шину сообщений [how-to-use-a-bus]

Существует два различных подхода к использованию шины сообщений:

- Запуск главного цикла обработки событий GLib/Gtk+ (или самостоятельный постоянный итерационный обход событий стандартного главного контекста GLib) с последующей установкой механизма какого-либо типа для наблюдения за состоянием шины сообщений. Это метод, который главный цикл обработки событий GLib будет использовать для проверки наличия новых сообщений на шине сообщений и уведомления вас в случае наличии таких сообщений.

    Обычно в подобных случаях приходится использовать функцию `gst_bus_add_watch()` или `gst_bus_add_signal_watch()`.

    Для использования шины сообщений следует соединить обработчик событий с объектом шины сообщений конвейера с помощью функции `gst_bus_add_watch()`. Этот обработчик будет вызываться тогда, когда конвейер будет генерировать сообщение и передавать его посредством шины сообщений. В рамках данного обработчика следует проверить тип сигнала (об этом будет сказано в следующем разделе) и выполнить какие-либо действия в зависимости от типа сообщения. Обработчик сигнала должен возвращать логическое значение `TRUE` для продолжения обработки событий шины или логическое значение `FALSE` для отсоединения обработчика.

- Самостоятельная проверка наличия сообщений на шине. При выполнении этой операции должны использоваться функции gst_bus_peek () и/или gst_bus_poll ().


```
#include <gst/gst.h>

static GMainLoop *loop;

static gboolean
my_bus_callback (GstBus     *bus,
		 GstMessage *message,
		 gpointer    data)
{
  g_print ("Получено сообщение типа %s\n", GST_MESSAGE_TYPE_NAME (message));

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("Ошибка: %s\n", err->message);
      g_error_free (err);
      g_free (debug);

      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* Окончание мультимедийного потока */
      g_main_loop_quit (loop);
      break;
    default:
      /* Необработанное сообщение */
      break;
  }

  /* мы хотим получать уведомление при поступлении следующего сообщения посредством
   * шины сообщений, поэтому возвращаем TRUE (FALSE возвращается для прекращения
   * отслеживания сообщений на шине, при этом наша функция обратного вызова более
   * не должна вызываться)
   */
  return TRUE;
}

gint
main (gint   argc,
      gchar *argv[])
{
  GstElement *pipeline;
  GstBus *bus;
  guint bus_watch_id;

  /* Инициализация */
  gst_init (&argc, &argv);

  /* Создание конвейера, добавление обработчика */
  pipeline = gst_pipeline_new ("my_pipeline");

  /* добавление функции отслеживания новых сообщений для шины сообщений нашего
   * конвейера в стандартный главный контекст GLib, который является главным
   * контекстом, к которому впоследствии будет присоединяться главный цикл
   * обработки событий GLib
   */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, my_bus_callback, NULL);
  gst_object_unref (bus);

[..]

  /* создание главного цикла обработки событий, который функционирует в рамках
   * стандартного главного контекста GLib (контекста NULL), другими словами:
   * в рамках контекста будет проводиться проверка наступления какого-либо события,
   * за которым осуществляется наблюдение.
   * При передаче сообщения посредством шины стандартный главный контекст
   * автоматически вызовет нашу функцию обратного вызова my_bus_callback()
   * для уведомления нас о поступлении данного сообщения.
   * Главный цикл обработки событий будет работать до того момента,
   * когда кто-либо вызовет функцию g_main_loop_quit()
   */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  /* Освобождение ресурсов */
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return 0;
}
```

Важно знать о том, что обработчик сигналов будет вызываться в контексте программного потока главного цикла обработки событий. Это означает, что взаимодействие между конвейером и приложением посредством шины сообщений будет осуществляться в *асинхронном режиме* и, следовательно, шина сообщений не может использоваться для выполнения некоторых операций реального времени, таких, как сведение аудиотреков, воспроизведение потоков без перерывов (в теории) или наложение эффектов на видео. Все подобные операции должны выполняться в контексте конвейера, что проще всего сделать, разработав плагин для фреймворка GStreamer. При этом шина сообщений отлично выполняет свою основную задачу: передает сообщения от конвейера приложению. Преимущество использованного механизма передачи сообщений заключается в том, что вся работа с программными потоками выполняется в рамках фреймворка GStreamer и скрыта от приложения, поэтому разработчик приложения вообще не должен беспокоиться о реализации механизмов управления программными потоками.

Учтите, что в том случае, если вы интегрируете код обработки сообщений в главный цикл обработки событий GLib, вы можете вместо начала отслеживания сообщений установить обработчик для сигнала "message" объекта шины сообщений. В этом случае вам не придется использовать оператор `switch()` для обработки всех существующих типов сообщений; следует просто установить обработчики для интересующих сигналов в форме "message::<тип>", где вместо строки <тип> должен использоваться определенный тип сообщения (в следующем разделе приводится описание типов сообщений).

Приведенный выше фрагмент кода может быть записан также и следующим образом:

```
GstBus *bus;

[..]

bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline);
gst_bus_add_signal_watch (bus);
g_signal_connect (bus, "message::error", G_CALLBACK (cb_message_error), NULL);
g_signal_connect (bus, "message::eos", G_CALLBACK (cb_message_eos), NULL);

[..]

```

В том случае, если вы не используете основной цикл обработки событий GLib, асинхронные сигналы о поступлении сообщений не будут доступны по умолчанию. Однако, вы можете установить специализированный синхронный обработчик, который будет активировать специализированный основной цикл обработки событий и использовать функцию `gst_bus_async_signal_func ()` для генерации сигналов (следует также обратиться к [документации](http://gstreamer.freedesktop.org/data/doc/gstreamer/stable/gstreamer/html/GstBus.html) для получения дополнительной информации).

### 7.2. Типы сообщений [message-types]

Фреймворк GStreamer использует несколько типов стандартных сообщений, которые могут передаваться посредством шины сообщений. Однако, механизм сообщений предусматривает расширение возможностей. Дополнительные типы сообщений могут объявляться на уровне плагинов, а на уровне приложений могут приниматься решения о том, следует ли исполнять специальный код при приеме подобных сообщений или просто игнорировать их. При разработке любых приложений настоятельно рекомендуется как минимум обрабатывать сообщения об ошибках и демонстрировать текст таких сообщений пользователю.

Все сообщения имеют поле источника, поле типа и поле времени. Поле источника сообщения может быть использовано для выяснения того, какой элемент сгенерировал сообщение. Например, при обработке сообщений некоторых типов большинству приложений будут интересны только те сообщения, которые были сгенерированы конвейером верхнего уровня (т.е., уведомления об изменениях состояний). Ниже приведен список всех типов сообщений с краткими описаниями их назначения, а также с пояснениями относительно разбора специфичных для сообщений этих типов данных.

- Уведомления с информацией об ошибках, предупреждениях или с дополнительной информацией: эти сообщения используются элементами в том случае, если необходимо проинформировать пользователя о состоянии конвейера. Сообщения об ошибках являются фатальными и приводят к прекращению передачи данных в рамках конвейера. Для продолжения эксплуатации конвейера ошибка должна быть устранена. Предупреждения не являются фатальными, тем не менее они указывают на проблему. Информационные сообщения являются сообщениями, не указывающими на проблему. Все эти сообщения передаются вместе с переменной типа `GError`, содержащей идентификатор типа ошибки, сообщение об ошибке и иногда строку с отладочным сообщением. Эти данные могут быть получены с помощью функций `gst_message_parse_error ()`, `_parse_waring ()` и `_parse_info ()`. Память, использованная для хранения как строки с сообщением об ошибке, так и строки с отладочным сообщением, должна быть освобождена после использования этих строк.

- Уведомление об окончании мультимедийного потока: это уведомление генерируется в момент окончания мультимедийного потока. Состояние конвейера не будет изменено, но обработка мультимедийного потока будет остановлена. Приложения могут использовать данное сообщение для перехода к следующему элементу списка воспроизведения. После получения уведомления об окончании мультимедийного потока имеется возможность возвращения к определенной позиции в потоке. В этом случае проигрывание потока автоматически продолжится. Данное сообщение не имеет специфичных аргументов.

- Тэги: генерируется в моменты, когда в мультимедийном потоке обнаруживаются метаданные. Данное сообщение может генерироваться неоднократно при работе с одним и тем же конвейером (т.е., как при изменении описывающих содержимое мультимедийного потока метаданных, таких, как имя артиста или название композиции, так и при изменении информации о самом потоке, такой, как количество сэмплов в секунду и скорость потока). Приложения должны производить внутреннее кэширование метаданных. Функция `gst_message_parse_tag ()` должна использоваться для разбора списка тэгов, после окончания которого к этому списку тэгов должна быть применена функция `gst_tag_list_unref ()` в том случае, если он более не требуется.

- Изменения состояний: генерируется после успешного изменения состояния. Функция `gst_message_parse_state_changed ()` может использоваться для разбора информации о старом и новом состоянии, установленном в результате данного изменения.

- Буферизация: генерируется в процессе кэширования мультимедийных потоков, передаваемых по сети. Имеется возможность извлечения значения показателя кэширования данных (в процентах) из сообщения путем извлечения значения свойства "buffer-percent" из структуры, возвращаемой функцией `gst_message_get_structure ()`. Для получения дополнительной информации также следует обратиться к [Главе 15, "Буферизация"](http://gstreamer.freedesktop.org/data/doc/gstreamer/head/manual/html/chapter-buffering.html).

- Сообщения элементов: это специальные сообщения, относящиеся к определенным элементам и обычно представляющие их дополнительные возможности. Сообщения, которые может отправлять определенный элемент, должны подробно описываться в документации этого элемента. В качестве примера можно привести элемент демультиплексора потоков формата QuickTime "qtdemux", который может отправлять сообщение элемента "redirect" в определенных ситуациях, когда поток содержит инструкцию перенаправления.

- Специфичные для приложения сообщения: любая информация из таких сообщений может быть извлечена путем получения структуры сообщения и чтения значений ее полей. Обычно эти сообщения могут безопасно игнорироваться.
Сообщения приложений главным образом предназначены для внутреннего использования в рамках приложений в тех случаях, когда приложению требуется передать информацию из какого-либо программного потока в главный программный поток. Это особенно полезно тогда, когда приложение использует сигналы от элементов (так как эти сигналы будут генерироваться в контексте программного потока, предназначенного для обработки мультимедийного потока).

----------

Предыдущий раздел : [Контейнеры](06-bins.html)

Следующий раздел : [Точки соединения и возможности](08-pads.html)

Главная страница : [http://neon1ks.github.io/](../index.html)








