#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "notebook_app.hpp"

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent) {
	// Resourced from the Qt website for layout design. URL listed below:
	// http://doc.qt.io/qt-5/layout.html
	// Create the Buttons
	QPushButton *button1 = new QPushButton("Start Kernel");
	button1->setObjectName("start");
	QPushButton *button2 = new QPushButton("Stop Kernel");
	button1->setObjectName("stop");
	QPushButton *button3 = new QPushButton("Reset Kernel");
	button1->setObjectName("reset");
	QPushButton *button4 = new QPushButton("Interrupt");
	button1->setObjectName("interrupt");
	QHBoxLayout *newLayout = new QHBoxLayout;
	newLayout->addWidget(button1);
	newLayout->addWidget(button2);
	newLayout->addWidget(button3);
	newLayout->addWidget(button4);
	//setObjectName("notebook");
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(newLayout); // Add the button layouts]
	layout->addWidget(&input, 1);
	layout->addWidget(&output, 1);
	this->setLayout(layout);

	QObject::connect(&input, SIGNAL(sendString(QString)), &output, SLOT(receiveString(QString)));
	QObject::connect(button1, SIGNAL(clicked()), &output, SLOT(start()));
	QObject::connect(button2, SIGNAL(clicked()), &output, SLOT(stop()));
	QObject::connect(button3, SIGNAL(clicked()), &output, SLOT(reset()));
	//QObject::connect(button4, SIGNAL(clicked()), &output, SLOT(interrupt()));
}

