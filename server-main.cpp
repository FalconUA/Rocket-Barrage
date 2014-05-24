#include <QApplication>
#include <server.h>
#include <ingame.h>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    Server server(9999);
    server.show();
    return app.exec();
}
