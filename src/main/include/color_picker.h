#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

char color_picker_h[] = {
"<html>"
    "<head>"
    "</head>"
    "<body>"
        "<form id=\"colorForm\" method=\"GET\">"
            "<label for=\"colorpicker\">Color Picker:</label>"
            "<input type=\"color\" name=\"colorlamp\" id=\"colorpicker\" value=\"#000000\" onchange=\"send_color_value()\">"
        "</form>"
        "<script>"
            "function send_color_value(){"
                "document.getElementById(\"colorForm\").submit();"
            "}"
        "</script>"
    "</body>"
"</html>"
};
#endif
