/*
 * Copyright (C) 2011  Red Hat, Inc.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Chris Lumens <clumens@redhat.com>
 */

#include "BaseWindow.h"
#include "SpokeWindow.h"
#include "intl.h"

/**
 * SECTION: SpokeWindow
 * @title: AnacondaSpokeWindow
 * @short_description: Window for displaying single spokes
 *
 * A #AnacondaSpokeWindow is a top-level window that displays a single spoke
 * on the entire screen.  Examples include the keyboard and language
 * configuration screens off the first hub.
 *
 * The window consists of two areas:
 *
 * - A navigation area in the top of the screen, inherited from #AnacondaBaseWindow
 *   and augmented with a button in the upper left corner.
 *
 * - An action area in the rest of the screen, taking up a majority of the
 *   space.  This is where widgets will be added and the user will do things.
 */

enum {
    PROP_BUTTON_LABEL = 1
};

#define DEFAULT_BUTTON_LABEL _("_Done")

enum {
    SIGNAL_BUTTON_CLICKED,
    LAST_SIGNAL
};

static guint window_signals[LAST_SIGNAL] = { 0 };

struct _AnacondaSpokeWindowPrivate {
    GtkWidget  *button;
};

G_DEFINE_TYPE(AnacondaSpokeWindow, anaconda_spoke_window, ANACONDA_TYPE_BASE_WINDOW)

static void anaconda_spoke_window_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void anaconda_spoke_window_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);

static void anaconda_spoke_window_button_clicked(GtkButton *button,
                                                 AnacondaSpokeWindow *win);

static void anaconda_spoke_window_class_init(AnacondaSpokeWindowClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->set_property = anaconda_spoke_window_set_property;
    object_class->get_property = anaconda_spoke_window_get_property;

    klass->button_clicked = NULL;

    /**
     * AnacondaSpokeWindow:button-label:
     *
     * The :button-label string is the text used to label the button displayed
     * in the upper lefthand of the window.  By default, this is a back button
     * but could be anything else appropriate.
     *
     * Since: 1.0
     */
    g_object_class_install_property(object_class,
                                    PROP_BUTTON_LABEL,
                                    g_param_spec_string("button-label",
                                                        P_("Button Label"),
                                                        P_("Label to appear on the upper left button"),
                                                        DEFAULT_BUTTON_LABEL,
                                                        G_PARAM_READWRITE));

    /**
     * AnacondaSpokeWindow::button-clicked:
     * @window: the window that received the signal
     *
     * Emitted when the button in the upper left corner has been activated
     * (pressed and released).  This is commonly the button that takes the user
     * back to the hub, but could do other things.
     *
     * Since: 1.0
     */
    window_signals[SIGNAL_BUTTON_CLICKED] = g_signal_new("button-clicked",
                                                         G_TYPE_FROM_CLASS(object_class),
                                                         G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                                                         G_STRUCT_OFFSET(AnacondaSpokeWindowClass, button_clicked),
                                                         NULL, NULL,
                                                         g_cclosure_marshal_VOID__VOID,
                                                         G_TYPE_NONE, 0);

    g_type_class_add_private(object_class, sizeof(AnacondaSpokeWindowPrivate));
}

/**
 * anaconda_spoke_window_new:
 *
 * Creates a new #AnacondaSpokeWindow, which is a window designed for
 * displaying a single spoke, such as the keyboard or network configuration
 * screens.
 *
 * Returns: A new #AnacondaSpokeWindow.
 */
GtkWidget *anaconda_spoke_window_new() {
    return g_object_new(ANACONDA_TYPE_SPOKE_WINDOW, NULL);
}

static void anaconda_spoke_window_init(AnacondaSpokeWindow *win) {
    GtkWidget *nav_area;

    win->priv = G_TYPE_INSTANCE_GET_PRIVATE(win,
                                            ANACONDA_TYPE_SPOKE_WINDOW,
                                            AnacondaSpokeWindowPrivate);

    /* Set some default properties. */
    gtk_window_set_modal(GTK_WINDOW(win), TRUE);

    /* Create the buttons. */
    win->priv->button = gtk_button_new_with_mnemonic(DEFAULT_BUTTON_LABEL);
    gtk_widget_set_halign(win->priv->button, GTK_ALIGN_START);

    /* Hook up some signals for that button.  The signal handlers here will
     * just raise our own custom signals for the whole window.
     */
    g_signal_connect(win->priv->button, "clicked",
                     G_CALLBACK(anaconda_spoke_window_button_clicked), win);

    /* And then put the button into the navigation area. */
    nav_area = anaconda_base_window_get_nav_area(ANACONDA_BASE_WINDOW(win));
    gtk_grid_attach(GTK_GRID(nav_area), win->priv->button, 0, 1, 1, 1);
}

static void anaconda_spoke_window_button_clicked(GtkButton *button,
                                                 AnacondaSpokeWindow *win) {
    g_signal_emit(win, window_signals[SIGNAL_BUTTON_CLICKED], 0);
}

static void anaconda_spoke_window_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
    AnacondaSpokeWindow *widget = ANACONDA_SPOKE_WINDOW(object);
    AnacondaSpokeWindowPrivate *priv = widget->priv;

    switch(prop_id) {
        case PROP_BUTTON_LABEL:
            g_value_set_string (value, gtk_button_get_label(GTK_BUTTON(priv->button)));
            break;
    }
}

static void anaconda_spoke_window_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
    AnacondaSpokeWindow *widget = ANACONDA_SPOKE_WINDOW(object);
    AnacondaSpokeWindowPrivate *priv = widget->priv;

    switch(prop_id) {
        case PROP_BUTTON_LABEL:
            gtk_button_set_label(GTK_BUTTON(priv->button), g_value_get_string(value));
            gtk_button_set_use_underline(GTK_BUTTON(priv->button), TRUE);
            break;
    }
}