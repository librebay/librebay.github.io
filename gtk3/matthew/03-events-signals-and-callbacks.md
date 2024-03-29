# События, сигналы и обратные вызовы [events-signals-and-callbabacks]

У всех библиотек GUI есть нечто общее. Должен существовать некий механизм для выполнения программного кода на действие пользователя. Программа, выполняющая в режиме командной строки, может позволить себе остановку выполнения в ожидании вывода и затем применить нечто вроде оператора выбора для выполнения разных ветвей программы в зависимости от введенных данных. Такой подход нецелесообразен в случае приложения GUI, поскольку оно должно непрерывно реагировать на ввод пользователя, например, ему приходится постоянно обновлять области окна.

У современных оконных систем есть система событий и приемники событий, которым адресована эта задача. Идея заключается в том, что каждый пользовательский ввод обычно с помощью мыши или клавиатуры инициирует событие. Нажатие на клавиатуре, например, вызовет "событие клавиатуры". Затем пишется программный код, который ждет приёма такого события и выполняется в случае его возникновения.

Как вы уже видели, эти события генерирует система X Window System, но они мало помогут вам как программисту GTK+, т. к. они очень низкоуровневые. Когда производится щелчок кнопкой мыши, X порождает событие, содержащее координаты указателя мыши, а вам нужно знать, когда пользователь активизирует виджет.

У GTK+ есть собственная система событий и приемников событий, называемых сигналами и обратными вызовами. Их очень легко применять, поскольку для установки обработчика сигнала можно использовать очень полезное свойство языка C, указатель на функцию.

Сначала несколько определений. Сигнал GTK+ порождается объектом типа `G_OBJECT`, когда происходит нечто, например, ввод пользователя. Функция, связанная с сигналом и, следовательно, вызываемая при любом порождении сигнала, называется *функцией обратного вызова*.

> *Примечание*
>
> Имейте в виду, что сигнал GTK+ &mdash; это нечто иное, чем сигнал UNIX.

Как программист, использующий GTK+, вы должны заботиться только о написании и связывании функций обратного вызова, поскольку код порождения сигнала &mdash; это внутренний программный код определенного виджета.

Прототип или заголовок функции обратного вызова обычно похож на следующий:

```C
void a_callback_function(GtkWidget *widget, gpointer user_data);
```

Вы передаете два параметра: первый &mdash; указатель на виджет, породивший сигнал, второй &mdash; произвольный указатель, который вы выбираете самостоятельно, когда связываете обратный вызов. Вы можете использовать этот указатель для любый целей.

Связать функцию обратного вызова тоже очень просто. Вы вызываете функцию-макрос `g_signal_connect` и передаете ей виджет, имя сигнала в виде строки, указатель на функцию обратного вызова и ваш произвольный указатель:

```C
gulong g_signal_connect(gpointer *object, const gchar* name, GCallback func, gpointer user_data);
```

Следует отметить, что для связывания функций обратного вызова нет ограничений. Вы можете иметь много сигналов, связанных с одной и той же функцией обратного вызова, и много функций обратного вызова, связанных с единственным сигналом.

В документации по API GTK+ можно найти подробное описание сигналов, порождаемых каждым виджетом.

> *Примечание*
>
> До появления GTK+ 2 для связывания функций обратного вызова применялась функция-макрос `gtk_signal_connect`. Начиная с GTK+ 2, она была заменена на функцию-макрос `g_signal_connect`.

## Пример: Функция обратного вызова [example-a-callback-function]

Рассмотрим пример, где опробуем работу функции-макроса `g_signal_connect`.

В программе `gtk_callback.c` вставим в окно кнопку и свяжем сигнал `clicked` (щелчок мышью по кнопке) с вашей функцией обратного вызова для вывода короткого сообщения:

```C
#include <gtk/gtk.h>
#include <stdio.h>

static int count = 0;

void button_clicked(GtkWidget *button, gpointer data)
{
    printf("%s pressed %d time(s) \n", (char *)data, ++count);
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    button = gtk_button_new_with_label("Hello World!");
    gtk_container_add(GTK_CONTAINER(window), button);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_clicked), "Button 1");

    gtk_widget_show(button);
    gtk_widget_show(window);

    gtk_main();

    return 0;
}
```

Введите исходный текст программы и сохраните его в файле с именем `gtk_callback.c`. Откомпилируйте и скомпонуйте программу аналогично программе `gtk1.c` из предыдущего раздела. Запустив ее, вы получите окно с кнопкой. При каждом щелчке кнопки мышью будет выводиться короткое сообщение (рис. 1).

![Рис. 1. Рассмотрение работы функции обратного вызова на примере кнопки](images/gtk_callback.png)


### Как это работает [how-it-works]

Вы добавили два новых элемента в программу `gtk_callback.c`: виджет `GtkButton` и функцию обратного вызова. `GtkButton` &mdash; это виджет простой кнопки, которая может содержать текст, в нашем случае "Hello World", и порождает сигнал, названный `clicked`, каждый раз, когда кнопку щелкают мышью.

Функция обратно вызова `button_clicked` связана с сигналом `clicked` виджета кнопки с помощью функции-макроса `g_signal_connect`:

```C
g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_clicked), "Button 1");
```

Обратите внимание на то, что имя кнопки &mdash; `"Button 1"` &mdash; передаётся в функцию обратного вызова как данные пользователя.

Весь остальной добавленный программный код касается виджета кнопки, создаваемой так же, как окно &mdash; вызовом функции `gtk_button_new_with_label` &mdash; функция `gtk_widget_show` делает её видимой.

Для расположения кнопки в окне вызывается функция `gtk_container_add`. Эта простая функция помещает `GtkWidget` внутрь объекта `GtkContainer` и принимает контейнер и виджет как аргументы:

```C
void gtk_container_add (GtkContainer *container, GtkWidget *widget);
```

Как вы уже знаете, `GtkWindow` &mdash; потомок или дочерний объект объекта `GtkContainer`, поэтом вы можете привести тип вашего объекта-окна к типу `GtkContainer` с помощью макроса `GTK_CONTAINER`:

```C
gtk_container_add(GTK_CONTAINER(window), button);
```

Функция `gtk_container_add` прекрасно подходит для расположения в окне одиночного виджета, но гораздо чаще вам потребуется для создания хорошего интерфейса размещать несколько виджетов в разных частях окна. У комплекта GTK+ есть специальные виджеты как раз для этой цели, именуемые виджетами упаковочных контейнеров.


----------

Назад: [Введение в GTK+](02-introducing-gtk.html)

Вернуться  [на главную страницу](../../index.html)

Далее: [Виджеты упаковочных контейнеров](04-packing-box-widgets.html)
