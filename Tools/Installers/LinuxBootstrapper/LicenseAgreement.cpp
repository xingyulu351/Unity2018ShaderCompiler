#include "LicenseAgreement.h"
#include "main.h"
#include "artifacts/generated/LinuxBootstrapper/LicenseText.h"
#include <cstdio>
#include <cstdlib>

void DoLicenseAgreement()
{
    gtk_widget_set_sensitive(GTK_WIDGET(GetObject("backButton")), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(GetObject("forwardButton")), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(GetObject("licenseCheckbox"))));
    GtkTextBuffer* buffer = GTK_TEXT_BUFFER(GetObject("licenseTextBuffer"));
    gtk_text_buffer_set_text(buffer, g_LicenseText, -1);
}

bool DoLicenseAgreementUnattended()
{
    printf("%s\n", g_LicenseText);
    printf("\n\nDo you accept the terms of the License Agreement? (y/n)\n");
    char* input = NULL;
    size_t inputLength = 0;
    bool accepted = getline(&input, &inputLength, stdin) && (*input == 'y' || *input == 'Y');
    free(input);
    return accepted;
}

extern "C"
{
    void OnLicenseAcceptanceToggled(GtkToggleButton* toggleButton, gpointer data)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(GetObject("forwardButton")), gtk_toggle_button_get_active(toggleButton));
    }
}
