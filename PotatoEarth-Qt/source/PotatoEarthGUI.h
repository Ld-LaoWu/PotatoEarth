#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PotatoEarthGUI.h"

// ¸¨ÖúUI

class PT_QtGlwidget;
class PotatoEarthGUI : public QMainWindow
{
    Q_OBJECT

    PT_QtGlwidget* graphicWidget;

public:
    PotatoEarthGUI(QWidget *parent = nullptr);
    ~PotatoEarthGUI();

    void AddGraphicWidget();

private:
    Ui::PotatoEarthGUIClass ui;
};
