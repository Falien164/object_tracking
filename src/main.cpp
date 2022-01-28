#include "TrackingApp.h"
#include <QtWidgets/QApplication>
#include "YoloTracking.h"

#include "resources/Darkstyle.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	a.setStyle(new DarkStyle);
    TrackingApp w;
    w.show();
    return a.exec();
}
