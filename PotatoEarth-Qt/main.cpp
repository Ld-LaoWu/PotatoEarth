#include "PotatoEarthGUI.h"
#include <iostream>
#include <QtWidgets/QApplication>

#include "PotatoEarth.h"

//控制发行版本
#ifdef PO_DIST
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // 暂时先这么处理一下
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PotatoEarthGUI w;
    w.show();

//#if (defined PO_DEBUG) || (defined PO_RELEASE)
    PTEarth::Log::Init();
    PO_EARTH_INFO("欢迎来到土豆地球系列");
//#endif // PO_DEBUG || PO_RELEASE


    auto earthApp = new PTEarth::Application();
    delete earthApp;

    return a.exec();
}
