#include "PotatoEarthGUI.h"
#include "widgets/PT_QtGlwidget.h"

PotatoEarthGUI::PotatoEarthGUI(QWidget *parent)
    : QMainWindow(parent)
{

    ui.setupUi(this);

}

PotatoEarthGUI::~PotatoEarthGUI()
{
    if (graphicWidget) {
        delete graphicWidget;
    }
}

void PotatoEarthGUI::AddGraphicWidget()
{
    graphicWidget = new PT_QtGlwidget;
    ui.horizontalLayout->addWidget(graphicWidget);
    graphicWidget->InitRenderApplication();
}
