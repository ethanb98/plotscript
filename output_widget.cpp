#include "output_widget.hpp"
#include "message_queue.hpp"

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
	setObjectName("output");
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(childView);
	childView->setScene(childScene);

	std::ifstream ifs(STARTUP_FILE);
	if (!interp.parseStream(ifs)) {
		childScene->addText(QString("Error: Invalid, could not parse"));
	}
	else {
		try {
			Expression exp = interp.evaluate();
		}
		catch (const SemanticError & ex) {
			QString error = QString(ex.what());
			childScene->addText(error);
		}
	}
	newInterp = interp;
	cons = Consumer(iq, oq);
	cons.setThreadRunTrue();
	t1 = std::thread(cons, interp);
}

OutputWidget::~OutputWidget() {
	std::string st;
	iq->push(st);
	if (t1.joinable()) {
		t1.join();
	}
	if (!iq->empty()) {
		iq->wait_and_pop(st);
	}
}

void OutputWidget::start() {
	//Interpreter interpnew;
	Consumer cons1(iq, oq);
	if (!cons.getThreadRun()) {
		cons.setThreadRunTrue();
		cons1.setThreadRunTrue();
		t1 = std::thread(cons1, interp);
	}
	/*else {
		childScene->clear();
		QString error = "Error: Could not start the thread, already a thread running.";
		childScene->addText(error);
		childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
		childView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		childView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}*/
}

void OutputWidget::stop() {
	if (cons.getThreadRun()) {
		cons.setThreadRunFalse();
		std::string st;
		iq->push(st);
		if (t1.joinable()) {
			t1.join();
		}
		if (!iq->empty()) {
			iq->wait_and_pop(st);
		}
	}
	/*else {
		childScene->clear();
		QString error = "Error: Could not stop the thread, already no thread running";
		childScene->addText(error);
		childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
		childView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		childView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}*/
}

void OutputWidget::reset() {
	Consumer cons2(iq, oq);
	// If not stopped
	if (cons.getThreadRun()) {
		// Stop the code
		std::string str;
		iq->push(str);
		if (t1.joinable()) {
			t1.join();
		}
		iq->try_pop(str);
	}
	childScene->clear();
	cons.setThreadRunTrue();
	cons2.setThreadRunTrue();
	// Start the code
	t1 = std::thread(cons2, interp);
	interp = newInterp;
}

void OutputWidget::receiveString(QString str) {
	// If not a list, clear the screen
	// else, recurse through and leave screen alone
	if (cons.getThreadRun()) {
		if (clearScreen) {
			childScene->clear();
		}
		std::istringstream iss(str.toStdString());
		if (!interp.parseStream(iss)) {
			childScene->clear();
			childScene->addText(QString("Error: Invalid, could not parse"));
		}
		else {
			try {
				Expression exp = interp.evaluate();
				// If a list, do not clear screen and recursively collect information
				if (exp.head().isLambda()) {
					childScene->clear();
				}
				else if (exp.head().isList() || exp.head().isDiscrete()) {
					if (exp.getTail().size() >= 10)
					{
						exp.head().setDiscrete();
					}
					listCap(exp);
				}
				else {
					if (exp.isText()) {
						childScene->clear();
						auto font = QFont("Monospace");
						font.setStyleHint(QFont::TypeWriter);
						font.setPointSize(1);

						Expression newExp = exp.textReq();
						double x = newExp.pointTail0();
						double y = newExp.pointTail1();
						Expression scaler = exp.req();
						double scale = scaler.head().asNumber();
						double rot = exp.textRotReq();
						rot = rot * 180 / M_PI;

						QString text = QString::fromStdString(exp.transferString().substr(2, (exp.transferString().length() - 4)));
						QGraphicsTextItem *childText = childScene->addText(text);

						childText->setFont(font);
						QRectF childRect = childText->sceneBoundingRect();
						QPointF childPos = QPointF(x - childRect.width() / 2, y - childRect.height() / 2);
						childText->setPos(childPos);
						QPointF childCenter = childText->sceneBoundingRect().center();
						childText->setTransformOriginPoint(childCenter);
						childText->setScale(scale);
						childText->setRotation(rot);
					}
					else {
						childScene->clear();
						childScene->addText(QString::fromStdString(exp.transferString()));
					}
				}
			}
			catch (const SemanticError & ex) {
				childScene->clear();
				QString error = QString::fromStdString(ex.what());
				childScene->addText(error);
			}
		}
		childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
		childView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		childView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
	else {
		childScene->clear();
		QString error = "Error: interpreter kernel not running";
		childScene->addText(error);
		childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
		childView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		childView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
}

void OutputWidget::listCap(Expression exp) {
	const double N = 20;
	const double A = 3;
	const double B = 3;
	const double C = 2;
	const double D = 2;
	const double P = 0.5;
	//const double orig = 0;
	clearScreen = false;
	if (exp.isPoint()) {
		//std::cout << "Printing Circle" << std::endl;
		childScene->clear();
		double size = 0;
		if (exp.head().isDiscrete()) {
			size = P;
		}
		else {
			Expression require = exp.req();
			size = require.head().asNumber();
		}
		double x = exp.pointTail0();
		double y = exp.pointTail1();

		QPen pen = QPen(Qt::NoPen);
		QBrush brush = QBrush(Qt::black);
		QRectF val = QRectF(x, y, size, size);
		val.moveCenter(QPointF(x, y));
		childScene->addEllipse(val, pen, brush);
	}
	else if (exp.isLine()) {
		//std::cout << "Printing Line" << std::endl;
		childScene->clear();
		double thicc = 0;
		if (exp.head().isDiscrete()) {
			thicc = 0;
		}
		else {
			Expression thickness = exp.req();
			thicc = thickness.head().asNumber();
		}
		double x1 = exp.lineTail0x();
		double y1 = exp.lineTail0y();
		double x2 = exp.lineTail1x();
		double y2 = exp.lineTail1y();
		QPen pen = QPen(Qt::black);
		pen.setWidth(thicc);
		childScene->QGraphicsScene::addLine(x1, y1, x2, y2, pen);
	}
	else {
		childScene->clear();
		for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
			if ((*e).isPoint()) {
				//std::cout << "Making list circle" << std::endl;
				double size = 0;
				if (exp.head().isDiscrete()) {
					size = P;
				}
				else {
					Expression Sizer = (*e).req();
					size = Sizer.head().asNumber();
				}
				double x = (*e).pointTail0();
				double y = (*e).pointTail1();
				QPen pen = QPen(Qt::NoPen);
				QBrush brush = QBrush(Qt::black);
				QRectF val = QRectF(x, y, size, size);
				val.moveCenter(QPointF(x, y));
				childScene->addEllipse(val, pen, brush);
			}
			else if ((*e).isLine()) {
				//std::cout << "Making list line" << std::endl;
				double thicc = 0;
				double y1 = 0;
				double y2 = 0;
				if (exp.head().isDiscrete()) {
					thicc = 0;
					y1 = (*e).lineTail0y() * -1;
					y2 = (*e).lineTail1y() * -1;
				}
				else {
					Expression Thicc = (*e).req();
					thicc = Thicc.head().asNumber();
					y1 = (*e).lineTail0y();
					y2 = (*e).lineTail1y();
				}
				double x1 = (*e).lineTail0x();
				double x2 = (*e).lineTail1x();
				QPen pen = QPen(Qt::black);
				if (exp.head().isDiscrete()) {
					pen.setWidth(0);
				}
				else {
					pen.setWidth(thicc);
				}
				childScene->QGraphicsScene::addLine(x1, y1, x2, y2, pen);
			}
			else if ((*e).isText()) {
				//std::cout << "Making list Text" << std::endl;
				auto font = QFont("Monospace");
				font.setStyleHint(QFont::TypeWriter);
				font.setPointSize(1);
				
				// change the number received (radians) and change to degrees
				Expression newExp = (*e).textReq();
				double x = newExp.pointTail0();
				double y = newExp.pointTail1();
				Expression Scale = (*e).req();
				double scale = Scale.head().asNumber();
				double rot = (*e).textRotReq();
				rot = rot * 180 / M_PI;

				QString text = QString::fromStdString((*e).transferString().substr(2, ((*e).transferString().length() - 4)));
				QGraphicsTextItem *childText = childScene->addText(text);
				childText->setFont(font);

				// Draw the text in the correct location
				QRectF childRect = childText->sceneBoundingRect();
				QPointF childPos = QPointF(x - childRect.width() / 2, y - childRect.height() / 2);
				childText->setPos(childPos);
				QPointF childCenter = childText->sceneBoundingRect().center();
				childText->setTransformOriginPoint(childCenter);
				childText->setScale(scale);
				childText->setRotation(rot);
			}
			else if (exp.head().isDiscrete()) {
				//std::cout << "Back in Discrete" << std::endl;
				static int i = 0;
				static double xmin = 0;
				static double xmax = 0;
				static double ymin = 0;
				static double ymax = 0;
				static double xscale = 0;
				static double yscale = 0;
				static double xpos = 0;
				static double ypos = 0;
				static std::string XMIN = "";
				static std::string XMAX = "";
				static std::string YMIN = "";
				static std::string YMAX = "";
				static std::string TITLE = "";
				static std::string XLABEL = "";
				static std::string YLABEL = "";
				auto font = QFont("Monospace");
				font.setStyleHint(QFont::TypeWriter);
				font.setPointSize(1);

				Expression Function = (*e).req();
				if (Function.head().isNone()) {
					i++;
					//std::cout << i << std::endl;
				}

				if (i == 1) {
					// xmin
					XMIN = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					xmin = std::stod(XMIN);
					//std::cout << "xmin: " << xmin << std::endl;
				}
				else if (i == 2) {
					// xmax
					XMAX = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					xmax = std::stod(XMAX);
					//std::cout << "xmax: " << xmax << std::endl;
					xscale = N / (xmax - xmin);
					//std::cout << "xmax: " << xmax << std::endl;
					//std::cout << "xscale: " << xscale << std::endl;
				}
				else if (i == 3) {
					// ymin
					YMIN = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					ymin = std::stod(YMIN);
					//std::cout << "ymin: " << ymin << std::endl;
				}
				else if (i == 4) {
					// ymax
					YMAX = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					ymax = std::stod(YMAX);
					//std::cout << "ymax: " << ymax << std::endl;
					yscale = N / (ymax - ymin);
					//std::cout << "ymax: " << ymax << std::endl;
					//std::cout << "yscale: " << yscale << std::endl;
				}
				else if (i == 5) {
					// title
					TITLE = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
				}
				else if (i == 6) {
					// x label
					XLABEL = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
				}
				else if (i == 7) {
					//y label
					YLABEL = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					
					xmin *= xscale;
					xmax *= xscale;
					ymin = ymin * yscale * -1;
					ymax = ymax * yscale * -1;
					//std::cout << "xmin: " << xmin << std::endl;
					//std::cout << "xmax: " << xmax << std::endl;
					//std::cout << "ymin: " << ymin << std::endl;
					//std::cout << "ymax: " << ymax << std::endl;

					QString Xmin = QString::fromStdString(XMIN);
					QString Xmax = QString::fromStdString(XMAX);
					QString Ymin = QString::fromStdString(YMIN);
					QString Ymax = QString::fromStdString(YMAX);
					QString Title = QString::fromStdString(TITLE);
					QString Xlabel = QString::fromStdString(XLABEL);
					QString Ylabel = QString::fromStdString(YLABEL);
					QGraphicsTextItem *childTitle = childScene->addText(Title);
					QGraphicsTextItem *childXLabel = childScene->addText(Xlabel);
					QGraphicsTextItem *childYLabel = childScene->addText(Ylabel);
					QGraphicsTextItem *childxmin = childScene->addText(Xmin);
					QGraphicsTextItem *childxmax = childScene->addText(Xmax);
					QGraphicsTextItem *childymin = childScene->addText(Ymin);
					QGraphicsTextItem *childymax = childScene->addText(Ymax);

					childxmin->setFont(font);
					xpos = xmin - (childxmin->boundingRect().width() / 2);
					//std::cout << "xmin xpos: " << xpos << std::endl;
					ypos = (ymin + C) - (childxmin->boundingRect().height() / 2);
					//std::cout << "xmin ypos: " << ypos << std::endl;
					childxmin->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
					
					childxmax->setFont(font);
					xpos = xmax - (childxmax->boundingRect().width() / 2);
					//std::cout << "xmax xpos: " << xpos << std::endl;
					ypos = (ymin + C) - (childxmax->boundingRect().height() / 2);
					//std::cout << "xmax ypos: " << ypos << std::endl;
					childxmax->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);

					childymin->setFont(font);
					xpos = (xmin - D) - (childymin->boundingRect().width() / 2);
					//std::cout << "ymin xpos: " << xpos << std::endl;
					ypos = ymin - (childymin->boundingRect().height() / 2);
					//std::cout << "ymin ypos: " << ypos << std::endl;
					childymin->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);

					childymax->setFont(font);
					xpos = (xmin - D) - (childymax->boundingRect().width() / 2);
					//std::cout << "ymax xpos: " << xpos << std::endl;
					ypos = ymax - (childymax->boundingRect().height() / 2);
					//std::cout << "ymax ypos: " << ypos << std::endl;
					childymax->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);

					//QRectF childRectTit = childTitle->sceneBoundingRect();
					childTitle->setFont(font);
					//std::cout << "Pre-Title xpos: " << xpos << std::endl;
					xpos = ((xmax + xmin) / 2) - (childTitle->boundingRect().width() / 2);
					//std::cout << "Title xpos: " << xpos << std::endl;
					ypos = (ymax - A) - (childTitle->boundingRect().height() / 2);
					//std::cout << "Title ypos: " << ypos << std::endl;
					childTitle->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);


					childXLabel->setFont(font);
					//QRectF childRectXLab = childXLabel->sceneBoundingRect();
					xpos = ((xmax + xmin) / 2) - (childXLabel->boundingRect().width() / 2);
					//std::cout << "Xlabel xpos: " << xpos << std::endl;
					ypos = (ymin + A) - (childXLabel->boundingRect().height() / 2);
					//std::cout << "Xlabel ypos: " << ypos << std::endl;
					childXLabel->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);


					childYLabel->setFont(font);
					childYLabel->setTransformOriginPoint(childYLabel->boundingRect().center());
					childYLabel->setRotation(-90);

					//QRectF childRectYLab = childYLabel->sceneBoundingRect();
					xpos = (xmin - B) - (childYLabel->boundingRect().width() / 2);
					//std::cout << "Ylabel xpos: " << xpos << std::endl;
					ypos = ((ymax + ymin) / 2) - (childYLabel->boundingRect().height() / 2);
					//std::cout << "Ylabel ypos: " << ypos << std::endl;
					childYLabel->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
					i = 0;
					return;
				}
				childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
				childView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
				childView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

			}
			else {
					childScene->addText(QString::fromStdString((*e).transferString()));
			}
		}
	}
}