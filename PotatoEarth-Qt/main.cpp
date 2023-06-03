#include "PotatoEarthGUI.h"
#include <iostream>
#include <QtWidgets/QApplication>

#include "PotatoEarth.h"

//���Ʒ��а汾
#ifdef PO_DIST
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // ��ʱ����ô����һ��
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PotatoEarthGUI w;
    w.show();

//#if (defined PO_DEBUG) || (defined PO_RELEASE)
    PTEarth::Log::Init();
    PO_EARTH_INFO("��ӭ������������ϵ��");
//#endif // PO_DEBUG || PO_RELEASE


    auto earthApp = new PTEarth::Application();
    delete earthApp;

    return a.exec();
}
