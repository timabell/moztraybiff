/*
   Copyright 2005 Ilya Konstantinov

   This file is part of mozTrayBiff.

   mozTrayBiff is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   mozTrayBiff is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with mozTrayBiff; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

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
