// Copyright (c) 2013 Shotgun Software Inc.

#include <string>
#include <gtk/gtk.h>
#include <boost/filesystem.hpp>

#include "BrowserHost.h"
#include "DialogManagerX11.h"

namespace fs = boost::filesystem;

DialogManager* DialogManager::get()
{
    static DialogManagerX11 inst;
    return &inst;
}

void DialogManagerX11::OpenFolderDialog(const FB::BrowserHostPtr& host, FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    _showFolderDialog(win, multi, cb);
}

void DialogManagerX11::_showFolderDialog(FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    FB::VariantList out;

    // Create the dialog.  Do it the hard way so we can override the default
    // behavior which does not allow files and directories to be selected together.
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Open", NULL, GTK_DIALOG_MODAL,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *action_area = gtk_dialog_get_action_area(GTK_DIALOG(dialog));
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Some nicer formatting
    gtk_box_set_spacing(GTK_BOX(content_area), 2);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 5);
    gtk_container_set_border_width(GTK_CONTAINER(action_area), 5);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

    // Create the file chooser widget
    GtkWidget *chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
    if (multi)
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(chooser), TRUE);

    // And add it to the dialog
    gtk_container_add(GTK_CONTAINER(content_area), chooser);
    gtk_widget_show(chooser);

    // for the life of me I can't figure out how to get the filechooserwidget
    // to return an actual requested size.  Going with the simple hard coded
    // size until that can be figured out.
    gtk_window_resize(GTK_WINDOW(dialog), 600, 400);

    // run the dialog
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_ACCEPT) {
      char *buffer;
      std::string filename;
      GSList *filenames, *iterator;

      filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(chooser));
      iterator = filenames;
      while (iterator) {
          buffer = (char *)g_slist_nth_data(iterator, 0);
          filename = std::string(buffer);
          g_free(buffer);

          // append a trailing slash if the name is a directory
          if (fs::is_directory(filename))
            filename.push_back('/');

          out.push_back(filename);
          iterator = g_slist_next(iterator);
      }
      g_slist_free(filenames);
    }
    gtk_widget_destroy(dialog);

    // call the callback with the results
    cb(out);
}