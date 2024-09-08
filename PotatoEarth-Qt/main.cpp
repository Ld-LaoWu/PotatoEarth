#include "PotatoEarthGUI.h"
#include <iostream>

//控制发行版本
#ifdef PO_DIST
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // 暂时先这么处理一下
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PotatoEarthGUI w;
    w.AddGraphicWidget();
    w.show();

    return a.exec();
}
