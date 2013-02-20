#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <gtk/gtk.h>

#include "DialogManagerX11.h"
#include "BrowserHost.h"

DialogManager* DialogManager::get()
{
    static DialogManagerX11 inst;
    return &inst;
}


void DialogManagerX11::OpenFolderDialog(const FB::BrowserHostPtr& host, FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    host->ScheduleOnMainThread(boost::shared_ptr<DialogManagerX11>(), boost::bind(&DialogManagerX11::_showFolderDialog, this, win, multi, cb));
}

void DialogManagerX11::_showFolderDialog(FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    FB::VariantList out;

    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Open File",
                      NULL,
                      GTK_FILE_CHOOSER_ACTION_OPEN,
                      GTK_STOCK_CANCEL,
                      GTK_RESPONSE_CANCEL,
                      GTK_STOCK_OPEN,
                      GTK_RESPONSE_ACCEPT,
                      NULL);
    if (multi)
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);
    if (gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        GSList *filenames, *iterator;
        char *filename;

        filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        iterator = filenames;
        while (iterator) {
            filename = (char *)g_slist_nth_data(iterator, 0);
            out.push_back(filename);
            g_free(filename);
            iterator = g_slist_next(iterator);
        }
        g_slist_free(filenames);
    }
    gtk_widget_hide(dialog);
    gtk_widget_destroy(dialog);

    cb(out);
}