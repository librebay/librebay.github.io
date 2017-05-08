# Виджеты упаковочных контейнеров [packing-box-widgets]

Компоновка GUI исключительно важна для удобства применения интерфейса, и добиться наилучшей компоновки труднее всего. Реальная трудность в размещении виджетов заключается в том, что вы не можете полагаться на наличие у всех пользователей одинаковых размеров окон, тем, шрифтов и цветовых схем. То, что может быть отличным интерфейсом для одной системы, в другой системе может оказаться просто нечитаемым.

Для создания GUI, который выглядит одинаково во всех системах, вам необходимо избегать размещения виджетов на основе абсолютных координат и использовать более гибкую систему компоновки. У GTK+ есть для этой цели виджеты контейнеров. Виджеты-контейнеры позволяют управлять компоновкой виджетов в окнах вашего приложения. Виджеты упаковочных контейнеров (на основе `GtkBox`) представляют очень удобный тип виджета-контейнера. GTK+ предлагает множество виджетов-контейнеров других типов, описанных в интерактивной документации к GTK+.

*Виджеты упаковочных контейнеров* &mdash; невидимые виджеты, задача которых &mdash; хранить другие виджеты и управлять их компоновкой или схемой размещения. Для управления размером отдельных виджетов, содержащихся в виджете упаковочного контейнера, вы задаёте правила вместо координат. Поскольку виджеты упаковочных контейнеров могут содержать любые объекты `GtkWidget` и объект `GtkBox` сам является объектом типа `GtkWidget`, для создания сложных компоновок можно формировать виджеты упаковочных контейнеров, вложенные один в другой.

Виджет-контейнер `GtkBox` создаётся функцией:

```
GtkWidget *gtk_box_new (GtkOrientation orientation, gint spacing);
```

При вызове функции `gtk_box_new` задают два параметра: `orientation` и `spacing`. Эти параметры управляют компоновкой всех виджетов в конкретном упаковочном контейнере.

Первый параметр `orientation` определяет как располагать виджеты:

- горизонтально, указав `GTK_ORIENTATION_HORIZONTAL`;
- Или вертикально &mdash; `GTK_ORIENTATION_VERTICAL`.

Параметр `spacing` задает расстояние между виджетами в пикселях.

После того как упаковочный контейнер создан, добавьте в него виджеты с помощью функций `gtk_box_pack_start` и `gtk_box_pack_end`:

```
void gtk_box_pack_start (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
void gtk_box_pack_end (GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
```

Функция `gtk_box_pack_start` упаковывает виджеты в `GtkBox` от начала к концу. А `gtk_box_pack_end`, наоборот, начинает от конца контейнера. Параметры функций управляют расстоянием между виджетами и форматом каждого виджета, находящегося в упаковочном контейнере.

В таблице 1 описаны параметры, которые вы можете передавать в функцию `gtk_box_pack_start` или `gtk_box_pack_end`.

<table class="table">
<caption>Таблица 1.</caption>
<colgroup>
<col class="col-md-3">
<col class="col-md-9">
</colgroup>
<thead>
<tr>
<th>Параметр</th>
<th>Описание</th>
</tr>
</thead>
<tbody>
<tr>
<td><code>GtkBox *box</code></td>
<td>Заполняемый упаковочный контейнер</td>
</tr>
<tr>
<td><code>GtkWidget *child</code></td>
<td>Виджет, который следует поместить в упаковочный контейнер</td>
</tr>
<tr>
<td><code>gboolean expand</code></td>
<td>Если равен <code>TRUE</code>, данный виджет занимает всё доступное пространство, используемое совместно с другими виджетами, у которых этот флаг также равен <code>TRUE</code></td>
</tr>
<tr>
<td><code>gboolean fill</code></td>
<td>Если равен <code>TRUE</code>, данный виджет будет занимать всю доступную площадь вместо использования её как отступа от краёв. Действует, только если флаг <code>expand</code> равен <code>TRUE</code></td>
</tr>
<tr>
<td><code>guint padding</code></td>
<td>Размер отступа вокруг виджета в пикселях</td>
</tr>
</tbody>
</table>

Давайте теперь рассмотрим эти виджеты упаковочных контейнеров и создадим более сложный пользовательский интерфейс, демонстрирующий вложенные упаковочные контейнеры.

## Макет виджета-контейнера [widget-container-layout]

В этом примере вы спланируете размещение нескольких простых виджетов-меток типа `GtkLabel` с помощью контейнеров `GtkBox`. Виджеты-метки &mdash; простые графические элементы, подходящие для вывода коротких текстовых фрагментов. Назовите эту программу `gtk_container.c`:

```C
#include <gtk/gtk.h>

void my_close_app(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}

// Обратный вызов позволяет приложению отменить событие close/destroy.
// (Для отмены возвращает TRUE.)
gboolean my_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    printf("In delete_event\n");

    return FALSE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *label1, *label2, *label3;
    GtkWidget *hbox, *vbox;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "The Window Title");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(my_close_app), NULL);

    g_signal_connect(G_OBJECT(window), "delete_event",
                     G_CALLBACK(my_delete_event), NULL);

    label1 = gtk_label_new("Label 1");
    label2 = gtk_label_new("Label 2");
    label3 = gtk_label_new("Label 3");

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_box_pack_start(GTK_BOX(vbox), label1, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), label2, TRUE, FALSE, 5);

    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), label3, TRUE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(window), hbox);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
```

Когда вы выполните эту программу, то увидите следующую схему расположения виджетов-меток в вашем окне (рис. 1).

![Рис.1. Размещение меток в контейнерах](images/gtk_container.png)

#### Как это работает [how-it-works]

Вы создаёте два виджета упаковочных контейнеров: `hbox` и `vbox`, горизонтальный и вертикальный, соответственно. С помощью функции `gtk_box_pack_start` вы заполняете `vbox` виджетами `label1` и `label2`, причем `label2` располагается у нижнего края контейнера, потому что вставляется после `label1`. Далее контейнер `vbox` целиком наряду с меткой `label3` вставляется в контейнер `hbox`.

В заключении `hbox` добавляется в окно и выводится на экран с помощью функции `gtk_widget_show_all`.

Схему размещения упаковочных контейнеров легче понять с помощью блок-схемы, показанной на рис. 2.

![Рис. 2. Блок-схема размещения упаковочных контейнеров](images/packing-box-layout-diagram.png)

Познакомившись с виджетами, сигналами, обратными вызовами и виджетами-контейнерами, вы рассмотрели основы комплекта инструментов GTK+. Но для того, чтобы стать программистом, профессионально применяющим GTK+, нужно понять, как наилучшим образом использовать имеющиеся в комплекте виджеты.

----------

Назад: [События, сигналы и обратные вызовы](03-events-signals-and-callbacks.html)

Вернуться  [на главную страницу](../../index.html).














