# Виджеты GTK+ [widgets-gtk]

В этом разделе мы рассмотрим API самых популярных виджетов GTK+, которые вы будете применять чаще всего в своих приложениях.

## GtkWindow [gtkwindow]

`GtkWindow` &mdash; базовый элемент всех приложений GTK+. До сих пор вы использовали его для хранения своих виджетов.

```nohighlight
GObject
╰── GInitiallyUnowned
    ╰── GtkWidget
        ╰── GtkContainer
            ╰── GtkBin
                ╰── GtkWindow
```

Существуют десятки вызовов API `GtkWindow`, но далее приведены функции, заслуживающие особого внимания.

```C
GtkWidget *gtk_window_new (GtkWindowType type);
void gtk_window_set_title (GtkWindow *window, const gchar *title);
void gtk_window_set_position (GtkWindow *window, GtkWindowPosition position);
void gtk_window_set_default_size (GtkWindow *window, gint width, gint height);
void gtk_window_resize (GtkWindow *window, gint width, gint height);
void gtk_window_set_resizable (GtkWindow *window, gboolean resizable);
void gtk_window_present (GtkWindow *window);
void gtk_window_maximize (GtkWindow *window);
void gtk_window_unmaximize (GtkWindow *window);
```

Как вы видели, функция `gtk_window_new` создаёт в памяти новое пустое окно. Заголовок окна не задан и местоположение окна не определены. Обычно вы будете заполнять окно виджетами и задавать меню и панель инструментов перед выводом окна на экран с помощью вызова функции `gtk_widget_show`.

Функция `gtk_window_set_title` изменяет текст полосы заголовка, информируя оконный менеджер запроса.

> *Примечание*
>
> Поскольку за отображение оформления окна отвечает оконный менеджер, а не библиотека GTK+, шрифт, цвет и размер текста зависят от вашего выбора оконного менеджера.

Функция `gtk_window_set_position` управляет начальным местоположением на экране. Параметр `position` может принимать пять значений, перечисленных в табл. 1.

<table class="table">
<caption>Таблица 1.</caption>
<colgroup>
<col class="col-md-4">
<col class="col-md-8">
</colgroup>
<thead>
<tr>
<th>Параметр <code>position</code></th>
<th>Описание</th>
</tr>
</thead>
<tbody>
<tr>
<td><code>GTK_WIN_POS_NONE</code></td>
<td>Окно располагается по усмотрению оконного менеджера</td>
</tr>
<tr>
<td><code>GTK_WIN_POS_CENTER</code></td>
<td>Окно центрируется на экране</td>
</tr>
<tr>
<td><code>GTK_WIN_POS_MOUSE</code></td>
<td>Расположение окна задается указателем мыши</td>
</tr>
<tr>
<td><code>GTK_WIN_POS_CENTER_ALWAYS</code></td>
<td>Окно остаётся отцентрированным независимо от его размера</td>
</tr>
<tr>
<td><code>GTK_WIN_POS_CENTER_ON_PARENT</code></td>
<td>Окно центрируется относительно родительского окна (удобно для диалоговых окон)</td>
</tr>
</tbody>
</table>

Функция `gtk_window_set_default_size` задаёт окно на экране в единицах отображения GTK+. Явное задание размера окна гарантирует, что содержимое окна не будет закрыто чем-либо или скрыто. Для того чтобы изменить размеры окна после его вывода на экран, можно воспользоваться функцией `gtk_window_resize`. По умолчанию пользователь может изменить размер окна, перемещая обычным способом его границу мышью. Если вы хотите помешать этому, можно вызвать функцию `gtk_window_set_resizable`, приравненную `FALSE`.

Для того чтобы убедиться в том, что ваше окно присутствует на экране и видно пользователю, т.е. не свернуто или скрыто, подойдет функция `gtk_window_present`. Она полезна для диалоговых окон, т.е. позволяет убедиться в том, что она не свернуты, когда вам нужен какой-либо пользовательский ввод. В противном случае, для раскрытия окна на весь экран и его сворачивания у вас есть функции `gtk_window_maximize` и `gtk_window_unmaximize`.


## GtkEntry [gtkentry]

`GtkEntry` &mdash; виджет однострочного текстового поля, который обычно применяется для ввода простых текстовых данных, например, адреса электронной почты, имени пользователя или имени узла сети. Существуют вызовы API, позволяющие задать как считывание введенного текста, так и его максимальную длину в символах, а также другие параметры, управляющие местоположением текста и его выделением.

```nohighlight
GObject
╰── GInitiallyUnowned
    ╰── GtkWidget
        ╰── GtkEntry
```

Можно настроить `GtkEntry` на отображение звёздочек (или любого другого определенного пользователем символа) на месте набранных буквенно-цифровых символов, что очень удобно для ввода паролей, когда вы не хотите, чтобы кто-то заглядывал через ваше плечо и читал текст.

Мы опишем большинство самых полезных функций виджета `GtkEntry`:

```
GtkWidget *gtk_entry_new (void);
void gtk_entry_set_max_length (GtkEntry *entry, gint max);
const gchar *gtk_entry_get_text (GtkEntry *entry);
void gtk_entry_set_text (GtkEntry *entry, const gchar *text);
void gtk_entry_set_visibility (GtkEntry *entry, gboolean visible);
void gtk_entry_set_invisible_char (GtkEntry *entry, gunichar ch);
```








