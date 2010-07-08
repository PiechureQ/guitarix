/*
 * Copyright (C) 2009, 2010 Hermann Meyer, James Warden, Andreas Degert
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "GxToggleImage.h"
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkprivate.h>
#include <gdk/gdkpixbuf.h>

#define P_(s) (s)   // FIXME -> gettext

enum {
	PROP_BASE_NAME = 1,
};

G_DEFINE_TYPE(GxToggleImage, gx_toggle_image, GTK_TYPE_MISC)

static void gx_toggle_image_set_property(
	GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void gx_toggle_image_get_property(
	GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void gx_toggle_image_destroy (GtkObject *obj);
static void gx_toggle_image_size_request (
	GtkWidget * widget, GtkRequisition * requisition);
static gint gx_toggle_image_expose (
	GtkWidget * widget, GdkEventExpose * event);

static void gx_toggle_image_class_init(GxToggleImageClass *klass)
{
	GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *object_class = GTK_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = (GtkWidgetClass*)klass;

	gobject_class->set_property = gx_toggle_image_set_property;
	gobject_class->get_property = gx_toggle_image_get_property;
	object_class->destroy = gx_toggle_image_destroy;
	widget_class->size_request = gx_toggle_image_size_request;
	widget_class->expose_event = gx_toggle_image_expose;

	g_object_class_install_property(
		gobject_class, PROP_BASE_NAME,
		g_param_spec_string("base-name",
		                    P_("Image base name"),
		                    P_("Base name of the image, append \"_on\" and \"_off\" for the stock names"),
		                    "switch",
		                    GParamFlags(GTK_PARAM_READWRITE)));
}

static void gx_toggle_image_init(GxToggleImage *toggle_image)
{
	toggle_image->base_name = g_strdup("switch");
}

static void gx_toggle_image_destroy (GtkObject *obj)
{
	GxToggleImage *toggle_image = GX_TOGGLE_IMAGE(obj);
	g_free(toggle_image->base_name);
	toggle_image->base_name = 0;
	GTK_OBJECT_CLASS(gx_toggle_image_parent_class)->destroy(obj);
}

static void gx_toggle_image_size_request (GtkWidget * widget, GtkRequisition * requisition)
{
	GxToggleImage *toggle_image = GX_TOGGLE_IMAGE(widget);
	GtkMisc *misc = GTK_MISC(widget);
	char *s = g_strconcat(toggle_image->base_name, "_on", NULL);
	GdkPixbuf *img = gtk_widget_render_icon(widget, s, GtkIconSize(-1), NULL);
	g_free(s);
	requisition->width = gdk_pixbuf_get_width(img) + misc->xpad * 2;
	requisition->height = gdk_pixbuf_get_height(img) + misc->ypad * 2;
	g_object_unref(img);
}

static gboolean gx_toggle_image_expose(GtkWidget *widget, GdkEventExpose *event)
{
	GxToggleImage *toggle_image = GX_TOGGLE_IMAGE(widget);
	const char *s = "_off";
	GtkWidget *p = widget->parent;
	while (TRUE) {
		if (GTK_IS_TOGGLE_BUTTON(p)) {
			if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(p))) {
				s = "_on";
			}
			break;
		}
		p = p->parent;
		if (!p) {
			break;
		}
	}
	char *nm = g_strconcat(toggle_image->base_name, s, NULL);
	GdkPixbuf *img = gtk_widget_render_icon(widget, nm, GtkIconSize(-1), NULL);
	g_free(nm);
	if (!img) {
		return FALSE;
	}
	//x, y, w, h = self.allocation;
	int x = (widget->allocation.width - gdk_pixbuf_get_width(img))/2;
	int y = (widget->allocation.height - gdk_pixbuf_get_height(img))/2;
	gdk_draw_pixbuf(gtk_widget_get_window(widget), NULL, img,
	                       0, 0, x, y, -1, -1,
	                       GDK_RGB_DITHER_NORMAL, 0, 0);
	g_object_unref(img);
	return FALSE;
}

static void
gx_toggle_image_set_property (GObject *object, guint prop_id, const GValue *value,
                              GParamSpec *pspec)
{
	GxToggleImage *toggle_image = GX_TOGGLE_IMAGE (object);

	switch(prop_id) {
	case PROP_BASE_NAME: {
		const char *str = g_value_get_string(value);
		g_free(toggle_image->base_name);
		toggle_image->base_name = g_strdup(str ? str : "");
		g_object_notify(object, "base-name");
		break;
	}
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
gx_toggle_image_get_property(GObject *object, guint prop_id, GValue *value,
                             GParamSpec *pspec)
{
	GxToggleImage *toggle_image = GX_TOGGLE_IMAGE(object);

	switch(prop_id) {
	case PROP_BASE_NAME:
		g_value_set_string(value, toggle_image->base_name);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}