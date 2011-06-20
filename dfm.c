#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define ARRSIZE(x) (sizeof(x) / sizeof(*x))
#define CLEANMASK(mask) (mask & ~(GDK_MOD2_MASK))

enum ListColumns {
	NAME_STR,
	PERMS_STR,
	SIZE_STR,
	MTIME_STR,
	PERMS,
	MTIME,
	SIZE,
	IS_DIR
};

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
static void action(GtkWidget *w, GtkTreePath *p, GtkTreeViewColumn *c, FmWindow *fw);
static gchar *create_perm_str(const mode_t mode);
static gchar *create_size_str(const size_t size);
static gchar *create_time_str(const char *fmt, const struct tm *time);
static FmWindow *createwin();
static void destroywin(GtkWidget *w, FmWindow *fw);
static void dir_exec(FmWindow *fw, const Arg *arg);
static gboolean keypress(GtkWidget *w, GdkEventKey *ev, FmWindow *fw);
static void newwin(FmWindow *fw, const Arg *arg);
static void open_directory(FmWindow *fw, const Arg *arg);
static void read_files(FmWindow *fw, DIR *dir);
static void spawn(const gchar *cmd, const gchar *path, gboolean include_path);
static int valid_filename(const char *s, int show_dot);

static const char* permstr[] = { "---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx" };

/* variables */
static int show_dotfiles = 0;
static GList *windows = NULL;

#include "config.h"

/* enters the selected item if directory, otherwise 
 * executes program with the file as argument */
void action(GtkWidget *w, GtkTreePath *p, GtkTreeViewColumn *c, FmWindow *fw)
{
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(fw->tree));
	GtkTreeIter iter;
	gchar *name;
	gchar fpath[PATH_MAX];
	gboolean is_dir;

	Arg arg;

	gtk_tree_model_get_iter(model, &iter, p);
	gtk_tree_model_get(model, &iter, NAME_STR, &name,
			IS_DIR, &is_dir, -1);

	chdir(fw->path);
	realpath(name, fpath);

	if (is_dir) { /* open directory */
		arg.v = (void*)fpath;
		open_directory(fw, &arg);
	} else { /* execute program */
		spawn(filecmd, fpath, TRUE);
	}
}

/* creates a formatted permission string */
gchar* 
create_perm_str(const mode_t mode)
{
	return g_strdup_printf("%s%s%s", permstr[(mode >> 6) & 7],
			permstr[(mode >> 3) & 7],
			permstr[mode & 7]);
}

/* creates a formatted size string */
gchar* 
create_size_str(const size_t size)
{
	if (size < 1024)
		return g_strdup_printf("%i B", (int)size);
	else if (size < 1024*1024)
		return g_strdup_printf("%.1f kB", size/1024.0);
	else if (size < 1024*1024*1024)
		return g_strdup_printf("%.1f MB", size/(1024.0*1024));
	else
		return g_strdup_printf("%.1f GB", size/(1024.0*1024*1024));
}

/* creates a formatted time string */
gchar* 
create_time_str(const char *fmt, const struct tm *time)
{
	gchar buf[64];
	strftime(buf, sizeof(buf), fmt, time);
	return g_strdup(buf);
}

/* creates and initializes a FmWindow */
FmWindow*
createwin()
{
	FmWindow *fw;

	GtkCellRenderer *rend;
	GtkListStore *store;

	fw = g_malloc(sizeof(FmWindow));
	fw->path = NULL;
	fw->show_dot = show_dotfiles;
	fw->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(fw->win), 640, 480);

	/* setup scrolled window */
	fw->scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fw->scroll),
			GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

	/* setup list store */
	store = gtk_list_store_new(8, G_TYPE_STRING, G_TYPE_STRING,
			G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT,
			G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN);

	/* setup tree view */
	fw->tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fw->tree), TRUE);

	rend = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(fw->tree),
			-1, "Name", rend, "text", NAME_STR, NULL);
	rend = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(fw->tree),
			-1, "Permissions", rend, "text", PERMS_STR, NULL);
	rend = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(fw->tree),
			-1, "Size", rend, "text", SIZE_STR, NULL);
	rend = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(fw->tree),
			-1, "Modified", rend, "text", MTIME_STR, NULL);

	/* expand first column */
	gtk_tree_view_column_set_expand(
			gtk_tree_view_get_column(GTK_TREE_VIEW(fw->tree), 0), 
			TRUE); 

	/* connect signals */
	g_signal_connect(G_OBJECT(fw->win), "destroy", 
			G_CALLBACK(destroywin), fw);
	g_signal_connect(G_OBJECT(fw->win), "key-press-event",
			G_CALLBACK(keypress), fw);
	g_signal_connect(G_OBJECT(fw->tree), "row-activated", 
			G_CALLBACK(action), fw);

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

/* change directory to current, and spawns program in background */
void
dir_exec(FmWindow *fw, const Arg *arg)
{
	if (!fw->path || !arg->v)
		return;

	spawn((char *)arg->v, fw->path, FALSE);
}

/* handles key events on the FmWindow */
gboolean
keypress(GtkWidget *w, GdkEventKey *ev, FmWindow *fw)
{
	gint i;

	for (i = 0; i < ARRSIZE(keys); i++) {
		if (gdk_keyval_to_lower(ev->keyval) == keys[i].key &&
				CLEANMASK(ev->state) == keys[i].mod &&
				keys[i].func) {
			keys[i].func(fw, &keys[i].arg);
		}
	}

	return FALSE;
}

/* creates and inserts a new FmWindow to the window list */
void
newwin(FmWindow *fw, const Arg *arg)
{
	FmWindow *new = createwin();
	open_directory(new, arg);
	windows = g_list_append(windows, new);
}

/* open and reads directory data to FmWindow */
void
open_directory(FmWindow *fw, const Arg *arg)
{
	DIR *dir;
	char rpath[PATH_MAX];

	if (!arg->v)
		return;

	/* change to current working directory to get relative paths right */
	if (fw->path)
		chdir(fw->path);

	/* get clean absolute path string */
	realpath((char*)arg->v, rpath);

	if (!(dir = opendir(rpath))) {
		/* TODO handle errors */
		g_warning("%s: %s\n", rpath, g_strerror(errno));
		return;
	}

	if (fw->path)
		g_free(fw->path);

	fw->path = g_strdup(rpath);
	chdir(fw->path);
	/* fw->mtime = mtime(fw->path); */

	read_files(fw, dir);

	closedir(dir);
}

/* reads files in to fw's list store from an opened DIR struct */
void
read_files(FmWindow *fw, DIR *dir)
{
	struct dirent *e;
	struct stat st;
	struct tm *time;
	gchar *name_str;
	gchar *mtime_str;
	gchar *perms_str;
	gchar *size_str;

	GtkListStore *store = GTK_LIST_STORE(
			gtk_tree_view_get_model(GTK_TREE_VIEW(fw->tree)));
	GtkTreeIter iter;

	/* remove previous entries */
	gtk_list_store_clear(store);

	while ((e = readdir(dir))) {

		if (valid_filename(e->d_name, fw->show_dot)
				&& (stat(e->d_name, &st) == 0)) {

			if (S_ISDIR(st.st_mode))
				name_str = g_strdup_printf("%s/", e->d_name);
			else
				name_str = g_strdup(e->d_name);

			time = localtime(&st.st_mtime);
			mtime_str = create_time_str("%Y-%m-%d %H:%M:%S", time);
			perms_str = create_perm_str(st.st_mode);
			size_str = create_size_str(st.st_size);

			gtk_list_store_append(store, &iter);
			gtk_list_store_set(store, &iter,
					NAME_STR, name_str,
					PERMS_STR, perms_str,
					SIZE_STR, size_str,
					MTIME_STR, mtime_str,
					PERMS, st.st_mode,
					MTIME, st.st_mtime,
					SIZE, st.st_size,
					IS_DIR, S_ISDIR(st.st_mode),
					-1);

			g_free(name_str);
			g_free(mtime_str);
			g_free(perms_str);
			g_free(size_str);
		}
	}
}

/* change working directory and spawns a program to the background */
void
spawn(const gchar *cmd, const gchar *path, gboolean include_path) 
{
	gchar *buf;

	if (include_path)
		buf = g_strdup_printf("%s \"%s\" &", cmd, path);
	else
		buf = g_strdup_printf("%s &", cmd);

	chdir(path);
	system(buf);
	g_free(buf);
}

/* returns 1 if valid filename, i.e. not '.' or '..' (or .* if show_dot = 0) */
int
valid_filename(const char *s, int show_dot)
{
	return show_dot ? 
		(strcmp(s, ".") != 0 && strcmp(s, "..") != 0) :
		*s != '.';
}

int
main(int argc, char *argv[])
{
	Arg arg;
	int i;

	/* read arguments */
	for(i = 1; i < argc && argv[i][0] == '-'; i++) {
		switch(argv[i][1]) {
			case 'd':
				show_dotfiles = 1;
				break;
			default:
				g_printerr("Usage: %s [OPTS]... [PATH]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	arg.v = i < argc ? argv[i] : ".";

	gtk_init(&argc, &argv);

	newwin(NULL, &arg);

	gtk_main();

	return 0;
}
