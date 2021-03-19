#include "Introduction.h"
#include "main.h"

void DoIntroduction()
{
    gtk_widget_set_sensitive(GTK_WIDGET(GetObject("forwardButton")), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(GetObject("backButton")), FALSE);
}
