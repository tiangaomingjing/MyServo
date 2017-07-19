#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  QQuickView engine;
  engine.setSource(QUrl(QStringLiteral("qrc:///main.qml")));
  engine.show();

  return app.exec();
}

