#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "startup_config.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"


class OutputWidget : public QWidget {
	Q_OBJECT
public:
	OutputWidget(QWidget * parent = nullptr);
	// Put this into Expression.cpp and run everything through there

private slots:
	void receiveString(QString str);

private:
	QGraphicsView * childView = new QGraphicsView(this);
	QGraphicsScene * childScene = new QGraphicsScene(this);
	Interpreter interp;
	bool clearScreen = true;
};
#endif