#include "PotatoEarthGUI.h"
#include <iostream>

//���Ʒ��а汾
#ifdef PO_DIST
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // ��ʱ����ô����һ��
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PotatoEarthGUI w;
    w.AddGraphicWidget();
    w.show();

    return a.exec();
}
