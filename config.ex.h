#define MODKEY GDK_CONTROL_MASK

static Key keys[] = {

	/* New directory */
	{ MODKEY, GDK_w, newwin, { 0 } },

	/* Go up one level */
	{ MODKEY, GDK_h, open_directory, { .v = ".." } },

	/* Terminal launch */
	{ MODKEY, GDK_x, dir_exec, { .v = "urxvt" } }
};
