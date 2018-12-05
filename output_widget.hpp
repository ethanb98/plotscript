#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QtMath>
#include <QTimer>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <utility>
#include <chrono>

#include "startup_config.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "expression.hpp"

typedef MsgSafeQueue<std::string> inputQueue;
typedef std::pair<Expression, std::string> output;
typedef MsgSafeQueue<output> outputQueue;

class Consumer {
public:
	Consumer() {
		threadRun = true;
	}

	Consumer(inputQueue *messageQueueIn, outputQueue *messageQueueOut) {
		threadRun = true;
		mqi = messageQueueIn;
		mqo = messageQueueOut;
	}

	void operator()(Interpreter interp) const{
		while (1) {
			std::string temp;
			Expression exp;
			mqi->wait_and_pop(temp);
			if (temp.empty()) {
				break;
			}
			std::string error;
			std::istringstream expression(temp);
			if (!interp.parseStream(expression)) {
				error = "Invalid Expression. Could not parse.";
			}
			else {
				try {
					exp = interp.evaluate();
				}
				catch (const SemanticError & ex) {
					error = ex.what();
				}
			}
			output out = std::make_pair(exp, error);
			mqo->push(out);
		}
	}

	bool getThreadRun() {
		return threadRun;
	}

	void setThreadRunFalse() {
		threadRun = false;
	}

	void setThreadRunTrue() {
		threadRun = true;
	}

private:
	bool threadRun;
	inputQueue * mqi;
	outputQueue * mqo;
};

class OutputWidget : public QWidget {
	Q_OBJECT
public:
	OutputWidget(QWidget * parent = nullptr);
	void listCap(Expression exp);
	~OutputWidget();

private slots:
	void receiveString(QString str);
	void start();
	void stop();
	void reset();
	void interrupt();
	void timerStart();

private:
	QGraphicsView * childView = new QGraphicsView(this);
	QGraphicsScene * childScene = new QGraphicsScene(this);
	Interpreter interp;
	bool clearScreen = true;
	inputQueue *iq = new inputQueue;
	outputQueue *oq = new outputQueue;
	Consumer cons;
	std::thread t1;
	Interpreter newInterp;
	QTimer *timer;
	output outpair;
};
#endif