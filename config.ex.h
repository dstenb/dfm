#define MODKEY GDK_CONTROL_MASK

/* Time format */
static const char *timefmt = "%Y-%m-%d %H:%M:%S";

/* Poll time for directory updating (in seconds) */
static const int polltime = 1;

/* Command to be executed when activating a file */ 
static const char *filecmd = "executor";

static Key keys[] = {

	/* Movement */
	{ MODKEY,                GDK_j,         move_cursor,    { .i = DOWN } },
	{ MODKEY,                GDK_k,         move_cursor,    { .i = UP } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_j,         move_cursor,    { .i = PAGEDOWN } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_k,         move_cursor,    { .i = PAGEUP } },
	{ MODKEY,                GDK_g,         move_cursor,    { .i = HOME } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_g,         move_cursor,    { .i = END } },

	/* New directory */
	{ MODKEY,                GDK_w,         newwin,         { 0 } },

	/* Go up one level */
	{ MODKEY,                GDK_h,         open_directory, { .v = ".." } },
	{ 0,                     GDK_BackSpace, open_directory, { .v = ".." } },

	/* Terminal launch */
	{ MODKEY,                GDK_x,         dir_exec,       { .v = "urxvt" } },

	/* Set path */
	{ MODKEY,                GDK_l,         path_exec,      { .v = "dmenu -p path" } },

	/* Preferences */
	{ MODKEY|GDK_SHIFT_MASK, GDK_h,         toggle_pref,    { .i = DOTFILES } }
};
