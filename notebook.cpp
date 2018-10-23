#include <QApplication>
#include "notebook_app.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  NotebookApp noteApp;

  noteApp.show();
  
  return app.exec();
}

