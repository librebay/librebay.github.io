# GtkTreeView

К этому моменту мы рассмотрели несколько простых виджетов GTK+, но не все виджеты представляют собой однострочные инструменты для ввода или отображения. Сложность виджетов ничего не ограничивается, и `GtkTreeView` &mdash; яркий пример виджета, собравшего в себя огромный объем функциональных возможностей.

```nohighlight
GObject
╰── GInitiallyUnowned
	╰── GtkWidget
		╰── GtkContainer
			╰── GtkTreeView
```

`GtkTreeView` &mdash; член семейства виджетов, создающий представление данных в виде дерева или списка наподобие тех, которые вы можете встретить в электронной таблице или файловом менеджере. С помощью виджета `GtkTreeView` можно создать сложные представления данных, смешивая текст, растровую графику и даже данные вводимые с помощью виджетов `GtkEntry`, и т. д.

Самый быстрый способ испытания `GtkTreeView` &mdash; запуск приложения `gtk3-demo`, которое поставляется вместе с GTK+ 3. Демонстрационное приложение показывает возможности всех виджетов GTK+ 3, включая `GtkTreeView` (рис. 10.1)

![Рис. 10.1. Пример Tree Store в программе gtk3-demo](images/gtk3-demo_TreeStore.png.png)

Семейство `GtkTreeView` состоит из четырех компонентов:

- `GtkTreeView` &mdash; отображение дерева или списка;
- `GtkTreeViewColumn` &mdash; представление столбца или дерева;
- `GtkCellRenderer` &mdash; управление отображаемыми ячейками;
- `GtkTreeModel` &mdash; представление данных дерева и списка.

Первые три компонента формируют так называемое *Представление*, а последний &mdash; *Модель*. Концепция разделения *Представление* и *Модели* (часто называемая проектным шаблоном *модель/Представление/Действие* (Model/View/Controller или сокращенно MVC) не свойственна GTK+, но проектированию уделяется все большее и большее внимание на всех этапах программирования.

Ключевое достоинство проектного шаблона MVC заключается в возможности одновременной визуализации данных в виде разных представлений без ненужного их дублирования. Например, текстовые редакторы могу иметь две разные панели и редактировать разные фрагменты документа без хранения в памяти двух копий документа.

Шаблон MVC также очень популярен в Web-программировании, поскольку облегчает создание Web-сайтов, которые визуализируются в мобильных или WAP-обозревателях не так, как в настольных, просто за счет наличия отдельных компонентов *Представление*, оптимизированных для Web-обозревателя каждого типа. Вы также можете отделить логику сбора данных, например, запросов к базе данных, от логики пользовательского интерфейса.

Мы начнем с рассмотрения компонента *модель*, представленного в GTK+ двумя типами. Объект типа `GtkTreeStore` содержит многоуровневые данные, например, иерархию каталогов, а объект `GtkListStore` предназначен для простых данных.

Для создания объекта `GtkTreeStore` в функцию передаётся количество столбцов, за которыми следуют типы всех столбцов:

```C
GtkWidget *store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN);
```

Чтение, вставка, редактирование и удаление из модели выполняется с помощью структур `GtkTreeIter`. Эти структуры итераторов указывают на узлы дерева (или строки списка) и помогают находить фрагменты структур данных потенциально очень большого объема, а также манипулировать ими. Есть несколько вызовов API для получения объекта-итератора для разных точек дерева, но мы рассмотрим простейшую функцию `gtk_tree_store_append()`.

Перед тем как вставлять какие-либо данные в модель дерева, вам нужно получить итератор, указывающий на новую строку. Функция `gtk_tree_store_append()` заполняет объект `GtkTreeIter`, который представляет новую строку в дереве, как строку верхнего уровня (если вы передаете значение NULL в третьем аргументе), так и подчиненную или дочернюю строку (если вы передаете итератор главного или родительской строки):

```C
GtkTreeIter iter;
gtk_tree_store_append(store, &iter, NULL);
```

Получив итератор, вы можете заполнять строку с помощью функции `gtk_tree_store_set()`:

```C
gtk_tree_store_set(store, &iter,
                   0, "Def Leppard",
                   1, 1987,
                   2, TRUE,
                   -1);
```

Номер столбца и данные передаются парами, которые завершаются `-1`. Позже вы примените тип `enum` для того, чтобы сделать номера столбцов более информативными.

Для того чтобы добавить ветвь к данной строке (дочернюю строку), вам нужен только итератор для дочерней строки, который вы получаете, вызвав снова функцию `gtk_tree_store_append()` и указав на этот раз в качестве параметра строку верхнего уровня:

```
GtkTreeIter child;
gtk_tree_store_append(store, &child, &iter);

```

Дополнительную информацию об объектах `GtkTreeStore` и функциях объекта `GtkListStore` см. в документации API:

- [GtkTreeStore: GTK+ 3 Reference Manual](https://developer.gnome.org/gtk3/stable/GtkTreeStore.html)
- [GtkListStore: GTK+ 3 Reference Manual](https://developer.gnome.org/gtk3/stable/GtkListStore.html)

Пойдём дальше и рассмотрим компонент *Представление* типа `GtkTreeView`.

Создание объекта `GtkTreeView` сама простота: только передайте в конструктор в качестве параметра модель типа `GtkTreeStore` или `GtkListStore`:

```
GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
```

Сейчас самое время настроить виджет для отображения данных именно так, как вы хотите. Для каждого столбца следует определить 'GtkCellRenderer` и источник данных. Можно выбрать, например, визуализацию только определенных столбцов данных или изменить порядок вывода столбцов.

'GtkCellRenderer` &mdash; это объект, отвечающий за прорисовку каждой ячейки на экране, и существует три подкласса, имеющие дело с текстовыми ячейками, ячейками пиксельной графики и ячейками кнопок-выключателей:

- `GtkCellRendererText`;
- `GtkCellRendererPixBuf`;
- `GtkCellRendererToggle`.

В вашем *Представлении* будет применено текстовое представление ячеек, `GtkCellRendererText`.

```C
GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();

gtk_tree_view_insert_column_with_attributes(
    GTK_TREE_VIEW (treeview),
    0, "This is the column title", renderer,
    "text", COLUMN_ITEM_NUMBER,
    NULL);
```

Вы создаете представление ячейки и передаете его в функцию вставки столбца. Эта функция позволяет сразу задать свойства `GtkCellRendererText`, передавая заканчивающиеся значением `NULL` пары "ключ/значение". В качестве параметров указаны представление дерева, номер столбца, заголовок столбца, представление ячейки и его свойства. В приведенном примере вы задаёте атрибут "text", передав номер других атрибутов, включая подчеркивание, шрифт, размер и т. д.

В примере далее, выполнив необходимые шаги, вы увидите, как это работает на практике.


### Пример: использование виджета GtkTreeView

Введите следующий программный код и назовите файл `gtk_tree.c`.

<ol>
<li>
<p>Примените тип <code>enum</code> для обозначения столбцов, чтобы можно было ссылаться на них по именам. Общее количество столбцов удобно обозначить как <code>N_COLUMNS</code>:</p>
<pre><code class="C">#include &lt;gtk/gtk.h&gt;

enum {
    COLUMN_TITLE,
    COLUMN_ARTIST,
    COLUMN_CATALOGUE,
    N_COLUMNS
};

void closeApp(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkTreeStore *store;
    GtkWidget *view;
    GtkTreeIter parent_iter, child_iter;
    GtkCellRenderer *renderer;


    gtk_init(&amp;argc, &amp;argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), &quot;Tree&quot;);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), &quot;destroy&quot;, G_CALLBACK(closeApp), NULL);
</code></pre>
</li>
<li>
<p>Далее вы создаёте модель дерева, передавая количество столбцов и тип каждого из них:</p>
<pre><code class="C">    store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
</code></pre>
</li>
<li>
<p>Следующий этап &mdash; вставка родительской и дочерней строк в дерево:</p>
<pre><code class="C">    gtk_tree_store_append(store, &amp;parent_iter, NULL);
    gtk_tree_store_set(store, &amp;parent_iter,
                       COLUMN_TITLE, &quot;Dark Side of the Moon&quot;,
                       COLUMN_ARTIST, &quot;Pink Floyd&quot;,
                       COLUMN_CATALOGUE, &quot;B000024D4P&quot;,
                       -1);
    gtk_tree_store_append(store, &amp;child_iter, &amp;parent_iter);
    gtk_tree_store_set(store, &amp;child_iter, COLUMN_TITLE, &quot;Speak to Me&quot;, -1);

    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
</code></pre>
</li>
<li>
<p>Наконец, добавьте столбцы в представление, задавая источники данных для них и заголовки:</p>
<pre><code class="C">    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                COLUMN_TITLE,
                                                &quot;Title&quot;, renderer,
                                                &quot;text&quot;, COLUMN_TITLE,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                COLUMN_ARTIST,
                                                &quot;Catalogue&quot;, renderer,
                                                &quot;text&quot;, COLUMN_ARTIST,
                                                NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                COLUMN_CATALOGUE,
                                                &quot;Catalogue&quot;, renderer,
                                                &quot;text&quot;, COLUMN_CATALOGUE,
                                                NULL);

    gtk_container_add(GTK_CONTAINER(window), view);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
</code></pre>
</li>
</ol>






----------

Назад: [Виджеты класса GtkButton](09-widgets-gtkbutton.html)

Далее: 

Вернуться  [на главную страницу](../../index.html).