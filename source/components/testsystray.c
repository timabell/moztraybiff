#include "eggstatusicon.h"
#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(argv[1], NULL);
	g_assert(pixbuf);
	// This should be ideally replaced by a completely libpr0n-based icon rendering.
	EggStatusIcon* trayIcon = egg_status_icon_new_from_pixbuf(pixbuf);
	g_object_unref(G_OBJECT(pixbuf));
	gtk_main();
	return 0;
}
