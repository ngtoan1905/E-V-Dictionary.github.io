#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/bt-5.0.0/inc/btree.h"
#include "lib/libfdr/inc/jrb.h"
#include <gdk/gdkkeysyms.h>
#include <ctype.h>

// Khai Báo biến
BTA *book = NULL;
const gchar *a, *b;
GtkWidget *about_dialog,*entry_search, *textview /*set_textview_text*/;
GtkListStore *list; /*jrb_to_list*/
GtkWidget *view1, *view2;

//Phần 1 Search word
void set_textview_text(char *text);
const char* soundex(const char* name);// chuyen ve ma soundex
int prefix( char *nextword,  char *word); // tra lai 1 neu giong prefix, 0 neu nguoc lai
void jrb_to_list(JRB nextWordarray); // dua danh sach nhung tu cung prefix vao list_store
int insert_insoundexlist(char * soundexlist , char * newword,  char * word, char * soundexWord); // dua tu giong ma soundex voi word vao soundexlist
void suggest(char * word, gboolean Tab_pressed); // suggest, dua vao prefix, dung JRB to list ~
gboolean search_suggest(GtkWidget * entry, GdkEvent * event, gpointer No_need); // gioi han ky tu, chi nhan alphabelt va tab
static void search(GtkWidget *w, gpointer data);

//Phần 2 Insert word
void insert_word(GtkWidget widget, gpointer window);
void add(GtkWidget *w, gpointer data);
void edit(GtkWidget *widget,gpointer data);
//Phần 3 Delete word
void delete_word(GtkWidget widget, gpointer window);
void delete_word_controller(GtkWidget *w, gpointer data);

//Phần 4 Information
void information(GtkWidget widget, gpointer window);

//Phần 5 Quit
void close_window(GtkWidget *widget, gpointer window);

//Một số hàm phụ
void Show_message(GtkWindow * parent , GtkMessageType type,  char * mms, char * content);

int main(int argc, char *argv[])
{
	btinit();
	book = btopn("words.dat", 0, 1);

	//GTK+
	GtkWidget *window_main;
	GtkWidget *fixed_main,*image;
	GtkWidget *btn_find_word, *btn_insert_word, *btn_delete_word, *btn_information, *btn_quit, *btn_edit;
	GtkWidget *lbl_input, *lbl_output;
	GList *List;
	const char *format = "<span style=\"oblique\" size=\"medium\" >\%s</span>";
	char *markup;

	gtk_init(&argc, &argv);

	//tao cua so
	window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_main), "Từ Điển Anh Việt");
	gtk_window_set_default_size(GTK_WINDOW(window_main), 900, 600);
	gtk_window_set_position(GTK_WINDOW(window_main), GTK_WIN_POS_CENTER);


	//tao nen
	fixed_main = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_main), fixed_main);

	image = gtk_image_new_from_file("common/black.jpg");
	gtk_container_add(GTK_CONTAINER(fixed_main), image);

	btn_find_word = gtk_button_new_with_label("");
	List = gtk_container_get_children(GTK_CONTAINER(btn_find_word));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\"  color=\"#FFCC00\" >Find Word</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main), btn_find_word, 720, 40);
	gtk_widget_set_size_request(btn_find_word, 120, 50);
	gtk_widget_set_tooltip_text(btn_find_word, "Search");


	markup = g_markup_printf_escaped(format, "Nhập:");
	lbl_input = gtk_label_new("...");
	gtk_fixed_put(GTK_FIXED(fixed_main), lbl_input, 170, 40);
	gtk_label_set_markup(GTK_LABEL(lbl_input), markup);
	gtk_widget_set_size_request(lbl_input, 120, 50);

	entry_search = gtk_search_entry_new();
	gtk_fixed_put(GTK_FIXED(fixed_main), entry_search, 300, 40);
	gtk_widget_set_size_request(entry_search, 400, 50);
	gtk_entry_set_max_length(GTK_ENTRY(entry_search),100);
	
	GtkEntryCompletion  *comple = gtk_entry_completion_new();
		gtk_entry_completion_set_text_column(comple, 0);
		list = gtk_list_store_new(10, G_TYPE_STRING, G_TYPE_STRING,
			G_TYPE_STRING, G_TYPE_STRING,
			G_TYPE_STRING, G_TYPE_STRING,
			G_TYPE_STRING, G_TYPE_STRING,
			G_TYPE_STRING, G_TYPE_STRING);
		gtk_entry_completion_set_model(comple, GTK_TREE_MODEL(list));
		gtk_entry_set_completion(GTK_ENTRY(entry_search), comple);

	markup = g_markup_printf_escaped(format, "Nghĩa:");
	lbl_output = gtk_label_new("...:");
	gtk_fixed_put(GTK_FIXED(fixed_main), lbl_output, 170, 100);
	gtk_label_set_markup(GTK_LABEL(lbl_output), markup);
	gtk_widget_set_size_request(lbl_output, 120, 50);


	textview = gtk_text_view_new();
	gtk_widget_set_size_request(textview, 400, 500);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
	
	GtkWidget *scrolling = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolling), textview);
	gtk_fixed_put(GTK_FIXED(fixed_main), scrolling, 300, 100);
	gtk_widget_set_size_request(scrolling, 400, 500);


	GtkWidget *data[3];
		data[0]= entry_search;
		data[1]= window_main;
		data[2]= textview;

		g_signal_connect(entry_search, "key-press-event", G_CALLBACK(search_suggest), NULL);

		g_signal_connect(G_OBJECT(entry_search), "activate", G_CALLBACK(search), data);

		g_signal_connect(G_OBJECT(btn_find_word), "clicked", G_CALLBACK(search), data);

	
	btn_insert_word = gtk_button_new_with_label("");
	List = gtk_container_get_children(GTK_CONTAINER(btn_insert_word));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\" color=\"#FFCC00\" >Insert Word</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main), btn_insert_word, 50, 100);
	gtk_widget_set_size_request(btn_insert_word, 120, 50);
	gtk_widget_set_tooltip_text(btn_insert_word, "Insert");
	g_signal_connect(G_OBJECT(btn_insert_word), "clicked", G_CALLBACK(insert_word), NULL);

	btn_edit = gtk_button_new_with_label("");
	List = gtk_container_get_children(GTK_CONTAINER(btn_edit));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\" color=\"#FFCC00\" >Edit</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main), btn_edit, 720, 100);
	gtk_widget_set_size_request(btn_edit, 120, 50);
	gtk_widget_set_tooltip_text(btn_edit, "Edit");
	g_signal_connect(G_OBJECT(btn_edit), "clicked", G_CALLBACK(edit), data);

	btn_delete_word = gtk_button_new_with_label("");
	List = gtk_container_get_children(GTK_CONTAINER(btn_delete_word));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\"  color=\"#FFCC00\" >Delete Word</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main), btn_delete_word, 50, 200);
	gtk_widget_set_size_request(btn_delete_word, 120, 50);
	gtk_widget_set_tooltip_text(btn_delete_word, "Delete");
	g_signal_connect(G_OBJECT(btn_delete_word), "clicked", G_CALLBACK(delete_word), NULL);

	btn_information = gtk_button_new_with_label("");
	List = gtk_container_get_children(GTK_CONTAINER(btn_information));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\"  color=\"#FFCC00\" >Information</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main), btn_information, 50, 300);
	gtk_widget_set_size_request(btn_information, 120, 50);
	gtk_widget_set_tooltip_text(btn_information, "Information");
	g_signal_connect(G_OBJECT(btn_information), "clicked", G_CALLBACK(information), NULL);

	btn_quit = gtk_button_new_with_label("");
	List = gtk_container_get_children(GTK_CONTAINER(btn_quit));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\"  color=\"#FFCC00\" >Quit</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main), btn_quit, 50, 400);
	gtk_widget_set_size_request(btn_quit, 120, 50);
	gtk_widget_set_tooltip_text(btn_quit, "Exit");

	g_signal_connect(G_OBJECT(btn_quit), "clicked", G_CALLBACK(close_window), window_main);
	g_signal_connect(G_OBJECT(window_main), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window_main);
	gtk_window_set_resizable(GTK_WINDOW(window_main), FALSE);
	gtk_main();

	btcls(book);

	return 0;
}
void set_textview_text(char * text)  // dua tu vao bo nho de
{
	GtkTextBuffer *buffer; //Lưu trữ văn bản được gán để hiển thị trong GtkTextView
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));//Trả về GtkTextBuffer đang được hiển thị bởi chế độ xem văn bản này. Số tham chiếu trên bộ đệm không tăng lên; người gọi hàm này đã giành được một sở hữu mới.
	if (buffer == NULL)
		buffer = gtk_text_buffer_new(NULL);//một bảng thẻ hoặc NULL để tạo một bảng mới.
	gtk_text_buffer_set_text(buffer, text, -1);//Xóa nội dung hiện tại của bộ đệm và thay vào đó chèn văn bản. Nếu len là -1, văn bản phải được chấm dứt. văn bản phải hợp lệ UTF-8.
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview), buffer);//Đặt bộ đệm làm bộ đệm được hiển thị bởi text_view. Bộ đệm trước đó được hiển thị bởi chế độ xem văn bản là không được ước tính và tham chiếu được thêm vào bộ đệm. Nếu bạn sở hữu một tham chiếu đến bộ đệm trước khi chuyển nó vào hàm này, bạn phải tự xóa tham chiếu đó; GtkTextView sẽ không nhận thông qua.
}	
const char* soundex(const char* name)
{
    static char s[4];
    int si = 1;
    int c;

    //                 ABCDEFGHIJKLMNOPQRSTUVWXYZ
    char mappings[] = "01230120022455012623010202";

    s[0] = toupper(name[0]);

    for(int i = 1, l = strlen(name); i < l; i++)
    {
        c = toupper(name[i]) - 65;

        if(c >= 0 && c <= 25)
        {
            if(mappings[c] != '0')
            {
                if(mappings[c] != s[si-1])
                {
                    s[si] = mappings[c];
                    si++;
                }

                if(si > 3)
                {
                    break;
                }
            }
        }
    }

    if(si <= 3)
    {
        while(si <= 3)
        {
            s[si] = '0';
            si++;
        }
    }
    return s;
}
int prefix(char* nextword,char* word)//tim nhung tu co phan dau giong het word
{
	int word_len = strlen(word);
	int nextword_len = strlen(nextword);
	int i;
	if (nextword_len < word_len)
		return 0;
	for (i = 0; i < word_len; i++)
		{
			if (nextword[i] != word[i])
			return 0;
		}
		return 1;    
}
void jrb_to_list(JRB nextWordarray) 
    {
     	GtkTreeIter Iter;
		JRB tmp;
		int max=0;
		jrb_traverse(tmp,nextWordarray)
		{
			max++;
			if(max>10) 
			{
				max=10;
				break;
			}
		}
     	jrb_traverse(tmp, nextWordarray) 
		{		if(max==0)  return;
				gtk_list_store_append(GTK_LIST_STORE(list), &Iter);
     			gtk_list_store_set(GTK_LIST_STORE(list), &Iter, 0, jval_s(tmp->key), -1 );
     			max--;
    	}
    }
int insert_insoundexlist(char * soundexlist , char * newword,  char * word, char * soundexword) // dua tu co cung soundex vao soundexlist
{
	if (strcmp(soundexword, soundex(newword)) == 0) 
	{
		if (strcmp(newword, word) != 0) 
		{
			strcat(soundexlist, newword);
			strcat(soundexlist, "\n");
			return 1;
		}
	}
	return 0;
}
void suggest(char *word, gboolean Tab_pressed) // suggest, dua vao prefix, dung JRB to list ~
{
	char nextword[100], prevword[100];
	int i;
	int max;
	GtkTreeIter Iter;
	JRB  nextWordarray = make_jrb();
	BTint value;
	int existed = 0;
	strcpy(nextword, word);
	gtk_list_store_clear(GTK_LIST_STORE(list));//Loại bỏ tất cả các hàng từ cửa hàng danh sách.

	if (bfndky(book, word, &value) ==  0) 
	{ // tim word trong book, value la gia tri cua 'word' tim duoc
		existed = 1;
		gtk_list_store_append(GTK_LIST_STORE(list), &Iter);
		gtk_list_store_set(GTK_LIST_STORE(list), &Iter, 0, nextword, -1 ); // neu dung thi ok
	}
	if (!existed)
		btins(book, nextword, "", 1); // chen key va data vao B-tree
	max=10;
	for (i = 0; i < 1000; i++) 
	{
		bnxtky(book, nextword, &value);  // tim 'key' tiep theo
		if (prefix(nextword, word)) { // tim nhung tu co prefix giong
			if(max==0) break;
			jrb_insert_str(nextWordarray, strdup(nextword), JNULL);// va chen vao array nextword (de show ra list -> jrb_to_list)
			max--;  
		}			
		else break;
	}	


	if (!existed && Tab_pressed) 
	{ // an tab de ra tu goi y 
		if (jrb_empty(nextWordarray)) {
			char soundexlist[10000] = "Ý của bạn là: \n";
			char soundexWord[50];
			strcpy(nextword, word);
			strcpy(prevword, word);
			strcpy(soundexWord, soundex(word)); // soundex dung de tim tu 'xung quanh'
			max = 5;
			for (i = 0; (i < 1000 ) && max; i++) {
				if (bprvky(book , prevword, &value) == 0)//tim 'key' lui ve truoc
					if (insert_insoundexlist(soundexlist, prevword, word, soundexWord))
						max--;
				}
			bfndky(book, word, &value);
			max = 5;
			for (i = 0; (i < 1000 ) && max; i++) {
				if (bnxtky(book , prevword, &value) == 0)//tim 'key' lui ve truoc
					if (insert_insoundexlist(soundexlist, prevword, word, soundexWord))
						max--;
				}
			set_textview_text(soundexlist);
				}
				else {
					strcpy(nextword, jval_s(jrb_first(nextWordarray)->key));
					
						gtk_entry_set_text(GTK_ENTRY(entry_search), nextword);//Đặt văn bản trong widget thành giá trị đã cho, thay thế nội dung hiện tại.

						gtk_editable_set_position(GTK_EDITABLE(entry_search), strlen(nextword));
					}
				}

			else
				jrb_to_list(nextWordarray);
			if (!existed)
				btdel(book, word);
			jrb_free_tree(nextWordarray);
		}
gboolean search_suggest(GtkWidget * entry, GdkEvent * event, gpointer No_need) // gioi han ky tu, chi nhan alphabelt va tab
{
	GdkEventKey *keyEvent = (GdkEventKey *)event;
	char word[100];
	int len;
	strcpy(word, gtk_entry_get_text(GTK_ENTRY(entry_search)));
	if (keyEvent->keyval == GDK_KEY_Tab) {
		suggest(word,  TRUE);
	}
	else {
		if (keyEvent->keyval != GDK_KEY_BackSpace) {
			len = strlen(word);
			word[len] = keyEvent->keyval;
			word[len + 1] = '\0';
		}
		else {
			len = strlen(word);
			word[len - 1] = '\0';
		}
		suggest(word, FALSE);
	}
	return FALSE;
}
void Show_message(GtkWindow * parent , GtkMessageType type,  char * mms, char * content) // dua ra thong bao 
{
	GtkWidget *mdialog;
	mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		type,
		GTK_BUTTONS_OK,
		"%s", mms);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s",  content);
	gtk_dialog_run(GTK_DIALOG(mdialog));
	gtk_widget_destroy(mdialog);
}
void close_window(GtkWidget *widget, gpointer window)
{
	gtk_widget_destroy(GTK_WIDGET(window));
}
int btfind(char * word) // dung cho ham search
{
	char mean[10000];
	int size;

	if(btsel(book,word,mean,10000,&size)==0)
	{
       
		set_textview_text(mean);
		//g_print(mean);
		return 1;
	}
    else 
    	return 0;
}
static void search(GtkWidget *w, gpointer data) // search tai day
{
	GtkWidget *entry1= ((GtkWidget**)data)[0];
	GtkWidget *window1=((GtkWidget**)data)[1];

	a = gtk_entry_get_text(GTK_ENTRY(entry1));
	//g_print("%s\n",a);
	char word[50];

	strcpy(word,a);
	if (word[0] == '\0')
		Show_message(GTK_WINDOW(window1), GTK_MESSAGE_WARNING, "Cảnh báo!", "Cần nhập từ để tra.");
	else
	{
		int result = btfind(word);
		if (result==0)
			Show_message(GTK_WINDOW(window1),GTK_MESSAGE_ERROR, "Xảy ra lỗi!","Không tìm thấy từ này trong từ điển.");
	}
	return;
}
void information(GtkWidget widget, gpointer window)
{
	about_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog), "gtk-about");
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "Từ điển Anh Việt");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about_dialog),"https://developer.gnome.org");
	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG(about_dialog),"Nguyễn Khánh Toàn - 20184203\nNguyễn Bá Luận - 20184147\nTrần Thăng Đức - 20184076\n Trần Hoàng Long Hải - 20184090");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "Full");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "Team 3");
	gtk_dialog_run(GTK_DIALOG(about_dialog));
	gtk_widget_destroy(about_dialog);
}
void insert_word(GtkWidget widget, gpointer window)
{
	//design 
	GtkWidget *lbl_input_word,*lbl_input_mean;
	GtkWidget *btn_insert_word,*btn_quit;
	GtkWidget *window_insert_word;
	GtkWidget *fixed_insert_main;
	GtkWidget *image;
	GtkWidget *table;
	GList *List;

	const char *format = "<span style=\"italic\" size=\"medium\" >\%s</span>";
	char *markup;

	window_insert_word = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_insert_word), "Insert Word");
	gtk_window_set_default_size(GTK_WINDOW(window_insert_word), 720, 590);
	gtk_window_set_position(GTK_WINDOW(window_insert_word), GTK_WIN_POS_CENTER);

	fixed_insert_main = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_insert_word), fixed_insert_main);

	image = gtk_image_new_from_file("common/Insert.png");
	gtk_container_add(GTK_CONTAINER(fixed_insert_main), image);

	table = gtk_table_new(2,4,FALSE);
	gtk_container_add(GTK_CONTAINER(fixed_insert_main), table);

	markup = g_markup_printf_escaped(format, "Nhập Từ :");
	lbl_input_word = gtk_label_new("...");
	gtk_label_set_markup(GTK_LABEL(lbl_input_word), markup);
	gtk_fixed_put(GTK_FIXED(fixed_insert_main), lbl_input_word, 50, 50);
	gtk_widget_set_size_request(lbl_input_word, 120, 50);

    
	view1 = gtk_text_view_new();
	gtk_widget_set_size_request(view1, 300, 50);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view1), GTK_WRAP_WORD);
	gtk_table_attach(GTK_TABLE(table), view1, 1, 2, 0, 1,GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 200, 50);
    
	markup = g_markup_printf_escaped(format, "Nhập Nghĩa :");
	lbl_input_mean = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(lbl_input_mean), markup);
	gtk_fixed_put(GTK_FIXED(fixed_insert_main), lbl_input_mean, 50, 120);
	gtk_widget_set_size_request(lbl_input_mean, 120, 50);

    
	view2 = gtk_text_view_new();
	gtk_widget_set_size_request(view2, 300, 400);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view2), GTK_WRAP_WORD);
    	gtk_table_attach(GTK_TABLE(table), view2, 1, 2, 1, 2,GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 200, 0);
    
	
	btn_insert_word = gtk_button_new_with_label("Insert");
	List = gtk_container_get_children(GTK_CONTAINER(btn_insert_word));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\"  color=\"#FFCC00\" >Insert</span>");
	gtk_fixed_put(GTK_FIXED(fixed_insert_main), btn_insert_word, 520, 50);
	gtk_widget_set_size_request(btn_insert_word, 120, 50);
	gtk_widget_set_tooltip_text(btn_insert_word, "Insert");

	btn_quit = gtk_button_new_with_label("Quit");
	List = gtk_container_get_children(GTK_CONTAINER(btn_quit));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\"  color=\"#FFCC00\" >Quit</span>");
	gtk_fixed_put(GTK_FIXED(fixed_insert_main), btn_quit, 520, 120);
	gtk_widget_set_size_request(btn_quit, 120, 50);
	gtk_widget_set_tooltip_text(btn_quit, "Quit");

	//code 
	g_signal_connect(G_OBJECT(btn_insert_word), "clicked", G_CALLBACK(add), NULL);
	//code

	g_signal_connect(G_OBJECT(btn_quit), "clicked", G_CALLBACK(close_window), window_insert_word);
	g_signal_connect(G_OBJECT(window_insert_word), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window_insert_word);
	// 
	gtk_window_set_resizable(GTK_WINDOW(window_insert_word), FALSE);
	gtk_main();
}
void delete_word(GtkWidget widget, gpointer window){
	GtkWidget *lbl_input,*lbl_output;
	GtkWidget *entry_word,*image;
	GtkWidget *btn_delete_word,*btn_quit;
	GtkWidget *window_main_delete;
	GtkWidget *fixed_main_delete;
	GList *List;
	const char *format = "<span style=\"italic\" size=\"medium\" >\%s</span>";
	char *markup;

	window_main_delete =  gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_main_delete), "Delete Word");
	gtk_window_set_default_size(GTK_WINDOW(window_main_delete), 870, 250);
	gtk_window_set_position(GTK_WINDOW(window_main_delete), GTK_WIN_POS_CENTER);

	fixed_main_delete = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window_main_delete), fixed_main_delete);

	image = gtk_image_new_from_file("common/delete.png");
	gtk_container_add(GTK_CONTAINER(fixed_main_delete), image);

	markup = g_markup_printf_escaped(format, "Nhập Từ :");
	lbl_input = gtk_label_new("Nhập Từ :");
	gtk_label_set_markup(GTK_LABEL(lbl_input), markup);
	gtk_fixed_put(GTK_FIXED(fixed_main_delete), lbl_input, 50, 50);
	gtk_widget_set_size_request(lbl_input, 200, 50);

	lbl_output =  gtk_label_new(".........");
	gtk_fixed_put(GTK_FIXED(fixed_main_delete), lbl_output, 260, 110);
	gtk_widget_set_size_request(lbl_output, 300, 50);
	
	btn_delete_word = gtk_button_new_with_label("Delete");
	List = gtk_container_get_children(GTK_CONTAINER(btn_delete_word));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\"  color=\"#FFCC00\" >Delete</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main_delete), btn_delete_word, 570, 50);
	gtk_widget_set_size_request(btn_delete_word, 120, 50);
	gtk_widget_set_tooltip_text(btn_delete_word, "Delete");

	btn_quit = gtk_button_new_with_label("Quit");
	List = gtk_container_get_children(GTK_CONTAINER(btn_quit));
	gtk_label_set_markup(GTK_LABEL(List->data), "<span style=\"oblique\" color=\"#FFCC00\" >Quit</span>");
	gtk_fixed_put(GTK_FIXED(fixed_main_delete), btn_quit,700,50);
	gtk_widget_set_size_request(btn_quit,120,50);
	gtk_widget_set_tooltip_text(btn_quit, "Quit");

	entry_word = gtk_entry_new();
	gtk_fixed_put(GTK_FIXED(fixed_main_delete),entry_word,260,50);
	gtk_widget_set_size_request(entry_word,300,50);
	//code
	GtkWidget *data[3];
	data[0] = entry_word;
	data[1] = window_main_delete;
	data[2] = lbl_output;

	g_signal_connect(G_OBJECT(btn_delete_word), "clicked", G_CALLBACK(delete_word_controller), data);
	g_signal_connect(G_OBJECT(entry_word), "activate", G_CALLBACK(delete_word_controller), data);

	//code
	g_signal_connect(G_OBJECT(btn_quit), "clicked", G_CALLBACK(close_window), window_main_delete);
	g_signal_connect(G_OBJECT(window_main_delete), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window_main_delete);
	// 
	gtk_window_set_resizable(GTK_WINDOW(window_main_delete), FALSE);
	gtk_main();
}
void delete_word_controller(GtkWidget *w, gpointer data)
{
	GtkWidget *entry1 = ((GtkWidget **)data)[0];
	GtkWidget *window1 = ((GtkWidget **)data)[1];
	GtkWidget *lbl_output = ((GtkWidget **)data)[2];
	gchar *a;

	a = gtk_entry_get_text(entry1);
	btpos(book, ZSTART);
	g_print("%s\n", a);
	char mean[100000], word[50];
	int size;
	strcpy(word, a);

	g_print("%s", word);
	g_print("%d\n", btsel(book, word, mean, 100000, &size));
	g_print("%s", mean);
	g_print("%d", word[3]);

	if (word[0] == '\0')
		Show_message(GTK_WINDOW(window1), GTK_MESSAGE_WARNING, "Cảnh báo!", "Cần nhập từ muốn xoá.");
	else if (btsel(book, word, mean, 100000, &size) != 0)
		gtk_label_set_text(GTK_LABEL(lbl_output), "Từ vừa nhập không tồn tại.");
	else if (btsel(book, word, mean, 100000, &size) == 0)
	{
		btdel(book, word);
		gtk_label_set_text(GTK_LABEL(lbl_output), "Xóa Thành Công.");
	}
	else
		Show_message(GTK_WINDOW(window1), GTK_MESSAGE_ERROR, "Xảy ra lỗi!", "Không thể xoá từ khỏi từ điển.");
	return;
}
void add(GtkWidget *w, gpointer data)
{

	GtkTextBuffer *buffer1,*buffer2;
	GtkTextIter start, end,iter;
	const gchar mean[10000], word[50];
	buffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(GTK_TEXT_VIEW(view1)));
	gtk_text_buffer_get_iter_at_offset(buffer1, &iter, 0);

	gtk_text_buffer_insert(buffer1, &iter, "", -1);
	gtk_text_buffer_get_bounds (buffer1, &start, &end);
	b = gtk_text_buffer_get_text (buffer1, &start, &end, FALSE);

	strcpy(word,b);
	g_print(word);

	buffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(GTK_TEXT_VIEW(view2)));
	gtk_text_buffer_get_iter_at_offset(buffer2, &iter, 0);

	gtk_text_buffer_insert(buffer2, &iter, "", -1);
	gtk_text_buffer_get_bounds (buffer2, &start, &end);
	b = gtk_text_buffer_get_text (buffer2, &start, &end, FALSE);

	strcpy(mean,b);
	printf("\n");
	g_print(mean);

	BTint x;

	if (word[0] == '\0' || mean[0] == '\0')
		Show_message(GTK_WINDOW(data), GTK_MESSAGE_WARNING, "Cảnh báo!", "Không được bỏ trống phần nào.");
	else if (bfndky(book, word, &x ) == 0)
		Show_message(GTK_WINDOW(data), GTK_MESSAGE_ERROR, "Xảy ra lỗi!", "Từ vừa nhập đã có trong từ điển.");
	else
	{
		if(btins(book,word, mean,10000))
			Show_message(GTK_WINDOW(data),GTK_MESSAGE_ERROR, "Xảy ra lỗi!","Không thể thêm vào từ điển.");
		else
			Show_message(GTK_WINDOW(data),GTK_MESSAGE_INFO, "Thành công!","Đã thêm vào từ điển.");
	}
	return;
}
void edit(GtkWidget *widget,gpointer data){
	GtkWidget *entry_search = ((GtkWidget **)data)[0];
	GtkWidget *window1 = ((GtkWidget **)data)[1];
	GtkWidget *edit_view = ((GtkWidget **)data)[2];
	const gchar *a;
	GtkTextIter start, end;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (edit_view);
	const gchar *b;
	char word[50];
	char mean[10000];
	char tmpMean[10000];
	char tmpWord[50];
	int size;
	a = gtk_entry_get_text(GTK_ENTRY(entry_search));
	g_print("%s",a);
	strcpy(word,a);
	gtk_text_buffer_get_bounds (buffer, &start, &end);
	b = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
	g_print("%s",b);
	strcpy(mean,b);
	if(strlen(word) == 0&&strlen(mean)==0){
		Show_message(GTK_WINDOW(window1),GTK_MESSAGE_WARNING,"Cảnh báo","Tìm kiếm trước khi chỉnh sửa.");
		return;
	}
	gtk_text_buffer_get_bounds (buffer, &start, &end);
	b = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
	strcpy(tmpMean,b);
	a = gtk_entry_get_text(GTK_ENTRY(entry_search));
	strcpy(tmpWord,a);
	if(strlen(word) == 0||strlen(mean) == 0){
		Show_message(GTK_WINDOW(window1), GTK_MESSAGE_WARNING,"Cảnh báo","Bạn đang bỏ trống một phần xin vui lòng điền đủ.");
	}else if (btsel(book, word, mean, 100000, &size) != 0)
	{
		Show_message(GTK_WINDOW(window1),GTK_MESSAGE_WARNING,"Cảnh báo","Từ cần chỉnh sửa không có trong từ điển.\n Nếu muốn thêm từ mới xin vui lòng chọn Insert Word.\n");
	}else{
		if(strcmp(tmpMean,mean) == 0 && strcmp(tmpWord,word) == 0){
			Show_message(GTK_WINDOW(window1),GTK_MESSAGE_INFO,"Thông Báo","Không có sự thay đổi nào.");
		}else if( btupd(book, word, tmpMean, strlen(mean) + 1)==1)
			Show_message(GTK_WINDOW(window1),GTK_MESSAGE_ERROR, "Xảy ra lỗi!","Không thể cập nhật từ.");
		else
			Show_message(GTK_WINDOW(window1),GTK_MESSAGE_INFO, "Thành công!","Đã cập nhật từ.");

	}


}