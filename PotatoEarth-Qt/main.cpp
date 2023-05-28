#include "PotatoEarthGUI.h"
#include <iostream>
#include <QtWidgets/QApplication>

#include "Core/PotatoEarth.h"

//���Ʒ��а汾
#ifdef PO_DIST
    #include <Windows.h>
    #pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup") // ��ʱ����ô����һ��
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PotatoEarthGUI w;
    w.show();

#ifdef PO_DEBUG
    // ��־��ӡ + ���Դ���
    std::cout << "��ӭ������������" << std::endl;
    PotatoEarth::printEarthVersion();
#endif

#ifdef PO_RELEASE
    // ���Դ���
    std::cout << "��ӭ������������" << std::endl;
#endif

    return a.exec();
}
