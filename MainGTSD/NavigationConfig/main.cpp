#include <QApplication>
#include <QQuickView>
#include <QQmlContext>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QQuickView viewer;
//  viewer.load(QUrl(QStringLiteral("qrc:/main.qml")));
  viewer.setSource(QUrl::fromLocalFile("D:/Exercise/QML/QML_Navigation/Navigation/main.qml"));
  //无边框，背景透明
  viewer.setFlags(Qt::Window|Qt::FramelessWindowHint);
  viewer.setColor(QColor(Qt::transparent));

  viewer.rootContext()->setContextProperty("mainwindow",&viewer);
  viewer.show();

  return app.exec();
}

