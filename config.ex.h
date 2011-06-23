#define MODKEY GDK_CONTROL_MASK

/* Command to be executed when activating a file */ 
static const char *filecmd = "executor";

/* Command to be executed for selecting path */
static const char *pathcmd = "dmenu -p path";

/* Terminal */
static const char *termcmd= "urxvt";

static Key keys[] = {

	/* Movement */
	{ MODKEY,                GDK_j,         move_cursor,    { .i = DOWN } },
	{ MODKEY,                GDK_k,         move_cursor,    { .i = UP } },
	{ MODKEY,                GDK_g,         move_cursor,    { .i = HOME } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_g,         move_cursor,    { .i = END } },

	/* New directory */
	{ MODKEY,                GDK_w,         newwin,         { 0 } },

	/* Go up one level */
	{ MODKEY,                GDK_h,         open_directory, { .v = ".." } },
	{ 0,                     GDK_BackSpace, open_directory, { .v = ".." } },

	/* Terminal launch */
	{ MODKEY,                GDK_x,         dir_exec,       { .v = termcmd } },

	/* Set path */
	{ MODKEY,                GDK_l,         path_exec,      { .v = pathcmd } }
};
