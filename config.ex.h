#define MODKEY GDK_CONTROL_MASK

/* Terminal command */
#define TERMINAL (char *[]){ "urxvt", NULL }

/* Bookmarks */
static const char *bookmarks[] = {
	"/",
	"/home/david",
	"/home/david/video",
	"/home/david/documents",
	"/home/david/books"
};

/* Time format */
static const char *timefmt = "%Y-%m-%d %H:%M:%S";

/* Poll time for directory updating (in seconds) */
static const int polltime = 1;

/* Command to be executed when activating a file */ 
static const char *filecmd[] = { "/bin/sh", "-c", 
	"executor \"$DFM_PATH\"", NULL };

/* Key bindings */
static Key keys[] = {

	/* Movement */
	{ MODKEY,                GDK_j,         move_cursor,    { .i = DOWN } },
	{ MODKEY,                GDK_k,         move_cursor,    { .i = UP } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_j,         move_cursor,    { .i = PAGEDOWN } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_k,         move_cursor,    { .i = PAGEUP } },
	{ MODKEY,                GDK_g,         move_cursor,    { .i = HOME } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_g,         move_cursor,    { .i = END } },

	/* Spawn new window */
	{ MODKEY,                GDK_w,         newwin,         { 0 } },

	/* Go up one level */
	{ MODKEY,                GDK_h,         set_path,       { .v = ".." } },
	{ 0,                     GDK_BackSpace, set_path,       { .v = ".." } },

	/* Terminal launch */
	{ MODKEY,                GDK_x,         dir_exec,       { .v = TERMINAL } },

	/* Make directory */
	{ MODKEY|GDK_SHIFT_MASK, GDK_m,         make_dir,       { .i = 0755 } },

	/* Set path */
	{ MODKEY,                GDK_l,         set_path,      { 0 } },

	/* Preferences */
	{ MODKEY|GDK_SHIFT_MASK, GDK_h,         toggle_pref,    { .i = DOTFILES } },

	/* Bookmarks */
	{ MODKEY,                GDK_1,         bookmark,       { .i = 0 } },
	{ MODKEY,                GDK_2,         bookmark,       { .i = 1 } },
	{ MODKEY,                GDK_3,         bookmark,       { .i = 2 } },
	{ MODKEY,                GDK_4,         bookmark,       { .i = 3 } },
	{ MODKEY,                GDK_5,         bookmark,       { .i = 4 } },
	{ MODKEY,                GDK_6,         bookmark,       { .i = 5 } },
	{ MODKEY,                GDK_7,         bookmark,       { .i = 6 } },
	{ MODKEY,                GDK_8,         bookmark,       { .i = 7 } },
	{ MODKEY,                GDK_9,         bookmark,       { .i = 8 } },
	{ MODKEY,                GDK_0,         bookmark,       { .i = 9 } }
};
