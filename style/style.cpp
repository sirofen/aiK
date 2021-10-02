#include <style/style.hpp>

#define back_color "#dedede"
#define back_color2 "#272727"
#define border_color "#dedede"

QString StyleHelper::getCloseStyleSheet()
{
    return "QToolButton { "
           "image: url(:/aiK/res/buttons/close-gray.png);"
           //"background-color: #292929; "
           "icon-size: 12px;"
           "padding-left: 10px;"
           "padding-right: 10px;"
           "padding-top: 5px;"
           "padding-bottom: 5px;"
           "border: 1px solid " border_color";"
           "}"
           "QToolButton:hover {"
           "image: url(:/aiK/res/buttons/close.png); "
           "}"
           "QToolButton:pressed { "
           "image: url(:/aiK/res/buttons/close.png);"
           "background-color: " back_color2";"
           "}";
}

QString StyleHelper::getMinimizeStyleSheet()
{
    return "QToolButton { "
           "image: url(:/aiK/res/buttons/window-minimize-gray.png);"
           //"background-color: #292929;"
           "icon-size: 12px;"
           "padding-left: 10px;"
           "padding-right: 10px;"
           "padding-top: 5px;"
           "padding-bottom: 5px;"
           "border: 1px solid " border_color";"
           "}"
           "QToolButton:hover { "
           "image: url(:/aiK/res/buttons/window-minimize.png); "
           "}"
           "QToolButton:pressed { "
           "image: url(:/aiK/res/buttons/window-minimize.png);"
           "background-color: " back_color2";"
           "}";
}
