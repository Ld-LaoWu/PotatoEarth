#include "PotatoEarthGUI.h"
#include <iostream>
#include <QtWidgets/QApplication>

#include "Core/PotatoEarth.h"

//控制发行版本
#ifdef PO_DIST
    #include <Windows.h>
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // 暂时先这么处理一下
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PotatoEarthGUI w;
    w.show();

#ifdef PO_DEBUG
    // 日志打印 + 调试窗口
    std::cout << "欢迎来到土豆地球" << std::endl;
    PotatoEarth::printEarthVersion();
#endif

#ifdef PO_RELEASE
    // 调试窗口
    std::cout << "欢迎来到土豆地球" << std::endl;
#endif

    return a.exec();
}
