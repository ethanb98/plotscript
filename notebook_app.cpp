#include <QWidget>
#include <QLayout>
#include <QPlainTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "notebook_app.hpp"

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent) {
	setObjectName("notebook");
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(&input, 1);
	layout->addWidget(&output, 1);
	this->setLayout(layout);

	QObject::connect(&input, SIGNAL(sendString(QString)), &output, SLOT(receiveString(QString)));
}

