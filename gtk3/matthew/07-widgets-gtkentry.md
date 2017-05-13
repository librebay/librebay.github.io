# GtkEntry [gtkentry]

`GtkEntry` &mdash; виджет однострочного текстового поля, который обычно применяется для ввода простых текстовых данных, например, адреса электронной почты, имени пользователя или имени узла сети. Существуют вызовы API, позволяющие задать как считывание введенного текста, так и его максимальную длину в символах, а также другие параметры, управляющие местоположением текста и его выделением.

```nohighlight
GObject
╰── GInitiallyUnowned
    ╰── GtkWidget
        ╰── GtkEntry
```

Можно настроить `GtkEntry` на отображение звёздочек (или любого другого определенного пользователем символа) на месте набранных буквенно-цифровых символов, что очень удобно для ввода паролей, когда вы не хотите, чтобы кто-то заглядывал через ваше плечо и читал текст.

Мы опишем большинство самых полезных функций виджета `GtkEntry`:

```C
GtkWidget   *gtk_entry_new                (void);
void         gtk_entry_set_max_length     (GtkEntry *entry, gint max);
const gchar *gtk_entry_get_text           (GtkEntry *entry);
void         gtk_entry_set_text           (GtkEntry *entry, const gchar *text);
void         gtk_entry_set_visibility     (GtkEntry *entry, gboolean visible);
void         gtk_entry_set_invisible_char (GtkEntry *entry, gunichar ch);
```

Вы можете создавать `GtkEntry` с помощью функции `gtk_entry_new`. Функцией `gtk_entry_set_max_length` можно настроить ввод текста фиксированной длины. Ограничение ввода определенной длинной текста избавит вас от проверки корректности длины ввода и, возможно, необходимости информировать пользователя о том, что текст слишком длинный.

Для получения содержимого виджета `GtkEntry` вызывайте функцию `gtk_entry_get_text`, которая возвращает указатель `const gchar`, внутренний по отношению к `GtkEntry`. Если вы хотите изменить текст или передать его в функцию, которая может его модифицировать, следует скомпилировать строку с помощью, например, функции `strcpy`.

Вы можете вручную задать содержимое виджета `GtkEntry`, применив функцию `gtk_entry_set_text`. Учтите, что она принимает указатель `const gchar`.

### Пример: ввод имени пользователя и пароля [example-username-and-password-entry]

Теперь, познакомившись с функциями виджета `GtkEntry`, посмотрим на них в действии в небольшой программе. Программа `gtk_entry.c` будет создавать окно ввода имени пользователя и пароля и сравнивать введенный пароль с секретным.

<ol>
<li>
<p>Сначанала определим секретный пароль, остроумно заданный как <code>secret</code>:</p>
<pre><code class="C">#include &lt;gtk/gtk.h&gt;
#include &lt;stdio.h&gt;
#include &lt;string.h&gt;

const char *password = &quot;secret&quot;;
</code></pre>
</li>
<li>
<p>У вас есть две функции обратно вызова, которые вызываются, когда уничтожается окно и щелкатеся мышью кнопка "OK":</p>
<pre><code>void closeApp(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}

void button_clicked(GtkWidget *button, gpoвinter data)
{
    const char *password_text = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)data));

    if (strcmp(password_text, password) == 0)
        printf(&quot;Access granted!\n&quot;);
    else
        printf(&quot;Access denied!\n&quot;);
}
</code></pre>
</li>
<li>
<p>В функции <code>main</code> создаётся, компонуется интерфейс и связываются обратные вызовы с сигналами. Для компоновки виджетов меток и полей ввода примените виджеты-контейнеры <code>GtkBox</code>:</p>
<pre><code>int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *username_label, *password_label;
    GtkWidget *username_entry, *password_entry;
    GtkWidget *ok_button;
    GtkWidget *hbox1, *hbox2;
    GtkWidget *vbox;

    gtk_init(&amp;argc, &amp;argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), &quot;GtkEntryBox&quot;);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), &quot;destroy&quot;, G_CALLBACK(closeApp), NULL);

    username_label = gtk_label_new(&quot;Login: &quot;);
    password_label = gtk_label_new(&quot;Password: &quot;);

    gtk_label_set_width_chars(GTK_LABEL(username_label), 12);
    gtk_label_set_width_chars(GTK_LABEL(password_label), 12);

    username_entry = gtk_entry_new();
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);

    ok_button = gtk_button_new_with_label(&quot;OK&quot;);

    g_signal_connect(G_OBJECT(ok_button), &quot;clicked&quot;, G_CALLBACK(button_clicked), password_entry);

    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_box_pack_start(GTK_BOX(hbox1), username_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), username_entry, TRUE, FALSE, 5);

    gtk_box_pack_start(GTK_BOX(hbox2), password_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox2), password_entry, TRUE, FALSE, 5);

    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
</code></pre>
</li>
</ol>

Когда вы запустите программу, то получите окно, показанное на рис. 1.

![Рис. 1. Ввод имени пользователя и пароля, используя GtkEntry](images/gtk_entry.png)

#### Как это работает [how-it-works]

Программа создает два виджета типа `GtkEntry`, `username_entry` и `password_entry`, а также задает видимость `password_entry`, равной `FALSE`, чтобы скрыть введенный пароль. Затем она формирует кнопку `GtkButton`, с помощью которой вы связываете сигнал `clicked` с функцией обратного вызова `button_clicked`.

Как только в функции обратного вызова программа извлечет введенный пароль и сравнит его с секретным паролем, на экран выводится соответствующее сообщение.

Обратите внимание на то, что для вставки виджетов в свои контейнеры вы много раз повторили операторы `gtk_box_pack_start`. Для сокращения этого повторяющегося программного кода в последующих примерах будет определена вспомогательная функция.

----------

Назад: [Виджет GtkWindow](06-widgets-gtkwindow.html)

Далее: [Виджет GtkSpinButton](08-widgets-gtkspinbutton.html)

Вернуться  [на главную страницу](../../index.html).