#include "eggstatusicon.h"
#include <gtk/gtk.h>
#include "mozilla_mailnews.xpm"

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_xpm_data(xpm_mozilla_mailnews_icon);
	// This should be ideally replaced by a completely libpr0n-based icon rendering.
	EggStatusIcon* trayIcon = egg_status_icon_new_from_pixbuf(pixbuf);
	g_object_unref(G_OBJECT(pixbuf));
	gtk_main();
	return 0;
}
