#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define CLEANMASK(mask) (mask & ~(GDK_MOD2_MASK))

typedef struct {
	GtkWidget *win;
	GtkWidget *scroll;
	GtkWidget *tree;
	gchar *path;
	gboolean show_dot;
	gint mtime;
} FmWindow;

typedef struct {
	gboolean b;
	gint i;
	void *v;
} Arg;

typedef struct {
	guint mod;
	guint key;
	void (*func)(FmWindow *fw, const Arg *arg);
	const Arg arg;
} Key;

/* functions */
static FmWindow *createwin();
static void destroywin(GtkWidget *w, FmWindow *fw);
static void newwin(FmWindow *fw, const Arg *arg);

/* variables */
static GList *windows = NULL;

#include "config.h"

/* creates and initializes a FmWindow */
FmWindow*
createwin()
{
	FmWindow *fw;

	GtkCellRenderer *rend;
	GtkListStore *store;

	fw = g_malloc(sizeof(FmWindow));
	fw->path = NULL;
	fw->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(fw->win), 640, 480);

	/* setup scrolled window */
	fw->scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fw->scroll),
			GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	/* setup list store */
	store = gtk_list_store_new(1, G_TYPE_STRING); /* TODO columns */

	/* setup tree view */
	fw->tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fw->tree), TRUE);

	rend = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(fw->tree),
			-1, "Name", rend, "text", 0, NULL);

	/* connect signals */
	g_signal_connect(G_OBJECT(fw->win), "destroy", 
			G_CALLBACK(destroywin), fw);

	/* add widgets */
	gtk_container_add(GTK_CONTAINER(fw->scroll), fw->tree);
	gtk_container_add(GTK_CONTAINER(fw->win), fw->scroll);

	gtk_widget_show_all(fw->win);

	return fw;
}

/* removes and deallocates a FmWindow */
void
destroywin(GtkWidget *w, FmWindow *fw)
{
	if ((windows = g_list_remove(windows, fw)) == NULL)
		gtk_main_quit();

	gtk_widget_destroy(fw->tree);
	gtk_widget_destroy(fw->scroll);
	gtk_widget_destroy(fw->win);
	
	if (fw->path)
		g_free(fw->path);

	g_free(fw);
}

/* creates and inserts a new FmWindow to the window list */
void
newwin(FmWindow *fw, const Arg *arg)
{
	FmWindow *new = createwin();
	windows = g_list_append(windows, new);
}

int
main(int argc, char *argv[])
{
	Arg arg;
	int i;

	/* read arguments */
	for(i = 1; i < argc && argv[i][0] == '-'; i++) {
		switch(argv[i][1]) {
			default:
				g_printerr("Usage: %s [OPTS]... [PATH]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	arg.v = i < argc ? argv[i] : ".";

	gtk_init(&argc, &argv);

	newwin(NULL, &arg);
	newwin(NULL, &arg);
	newwin(NULL, &arg);

	gtk_main();

	return 0;
}
