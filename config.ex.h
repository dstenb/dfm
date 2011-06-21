#define MODKEY GDK_CONTROL_MASK

/* Command to be executed when activating a file */ 
static const char *filecmd = "executor";

static Key keys[] = {

	/* New directory */
	{ MODKEY, GDK_w, newwin, { 0 } },

	/* Go up one level */
	{ MODKEY, GDK_h, open_directory, { .v = ".." } },

	/* Terminal launch */
	{ MODKEY, GDK_x, dir_exec, { .v = "urxvt" } },

	/* Set path */
	{ MODKEY, GDK_l, path_exec, { .v = "dmenu" } }
};
