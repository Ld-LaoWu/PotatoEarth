#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PotatoEarthGUI.h"

class PotatoEarthGUI : public QMainWindow
{
    Q_OBJECT

public:
    PotatoEarthGUI(QWidget *parent = nullptr);
    ~PotatoEarthGUI();

private:
    Ui::PotatoEarthGUIClass ui;
};
