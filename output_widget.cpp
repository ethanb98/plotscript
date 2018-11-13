#include "output_widget.hpp"

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
}

void OutputWidget::receiveString(QString str) {
	// If not a list, clear the screen
	// else, recurse through and leave screen alone
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
					double scale = exp.req();
					double rot = exp.textRotReq();
					rot = rot * 180 / M_PI;


					QString text = QString::fromStdString(exp.transferString().substr(2, (exp.transferString().length() - 4)));
					QGraphicsTextItem *childText = childScene->addText(text);
					
					
					childText->setFont(font);
					childText->setScale(scale);
					childText->setRotation(rot);
					QRectF childRect = childText->sceneBoundingRect();
					//QPointF childCenter = childText->sceneBoundingRect().center();
					//childText->setTransformOriginPoint(childCenter);

					x = x - childRect.width() / 2;
					y = y - childRect.height() / 2;
					childText->setPos(x, y);
					//childText->boundingRect().moveCenter(QPointF(x, y));
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

void OutputWidget::listCap(Expression exp) {
	const double N = 20;
	const double A = 3;
	const double B = 3;
	const double C = 2;
	const double D = 2;
	/*const double P = 0.5;
	const double orig = 0;*/
	clearScreen = false;
	if (exp.isPoint()) {
		childScene->clear();
		double size = exp.req();
		double x = (exp.pointTail0() - (size / 2));
		double y = (exp.pointTail1() - (size / 2));
		QPen pen = QPen(Qt::black);
		QBrush brush = QBrush(Qt::black);
		childScene->QGraphicsScene::addEllipse(x, y, size, size, pen, brush);
	}
	else if (exp.isLine()) {
		childScene->clear();
		double thicc = exp.req();
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
				double size = (*e).req();
				double x = ((*e).pointTail0() - (size / 2));
				double y = ((*e).pointTail1() - (size / 2));
				QPen pen = QPen(Qt::black);
				QBrush brush = QBrush(Qt::black);
				childScene->QGraphicsScene::addEllipse(x, y, size, size, pen, brush);
			}
			else if ((*e).isLine()) {
				double thicc = (*e).req();
				double x1 = (*e).lineTail0x();
				double x2 = (*e).lineTail1x();
				double y1 = (*e).lineTail0y();
				double y2 = (*e).lineTail1y();
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
				auto font = QFont("Monospace");
				font.setStyleHint(QFont::TypeWriter);
				font.setPointSize(1);
				
				Expression newExp = (*e).textReq();
				double x = newExp.pointTail0();
				double y = newExp.pointTail1();
				double scale = (*e).req();
				double rot = (*e).textRotReq();
				rot = rot * 180 / M_PI;

				QString text = QString::fromStdString((*e).transferString().substr(2, ((*e).transferString().length() - 4)));
				QGraphicsTextItem *childText = childScene->addText(text);
				childText->setFont(font);
				childText->setScale(scale);
				childText->setRotation(rot);
				//QPointF childCenter = childText->sceneBoundingRect().center();
				//childText->setTransformOriginPoint(childCenter);
				QRectF childRect = childText->sceneBoundingRect();
				x = x - childRect.width() / 2;
				y = y - childRect.height() / 2;
				childText->setPos(x, y);
				//childText->boundingRect().moveCenter(QPointF(x, y));
			}
			else if (exp.head().isDiscrete()) {
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
				if (i == 0) {
					// xmin
					XMIN = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					xmin = std::stod(XMIN);
					std::cout << "xmin: " << xmin << std::endl;
					i++;
				}
				else if (i == 1) {
					// xmax
					XMAX = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					xmax = std::stod(XMAX);
					std::cout << "xmax: " << xmax << std::endl;
					xscale = N / (xmax - xmin);
					std::cout << "xmax: " << xmax << std::endl;
					std::cout << "xscale: " << xscale << std::endl;
					i++;
				}
				else if (i == 2) {
					// ymin
					YMIN = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					ymin = std::stod(YMIN);
					std::cout << "ymin: " << ymin << std::endl;
					i++;
				}
				else if (i == 3) {
					// ymax
					YMAX = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					ymax = std::stod(YMAX);
					std::cout << "ymax: " << ymax << std::endl;
					yscale = N / (ymax - ymin);
					std::cout << "ymax: " << ymax << std::endl;
					std::cout << "yscale: " << yscale << std::endl;
					i++;
				}
				else if (i == 4) {
					// title
					TITLE = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					i++;
				}
				else if (i == 5) {
					// x label
					XLABEL = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					i++;
				}
				else if (i == 6) {
					//y label
					YLABEL = (*e).head().asString().substr(1, (*e).head().asString().length() - 2);
					
					xmin *= xscale;
					xmax *= xscale;
					ymin *= yscale;
					ymax *= yscale;
					std::cout << "xmin: " << xmin << std::endl;
					std::cout << "xmax: " << xmax << std::endl;
					std::cout << "ymin: " << ymin << std::endl;
					std::cout << "ymax: " << ymax << std::endl;

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
					std::cout << "xmin xpos: " << xpos << std::endl;
					ypos = (ymax + C) - (childxmin->boundingRect().height() / 2);
					std::cout << "xmin ypos: " << ypos << std::endl;
					childxmin->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);
					
					childxmax->setFont(font);
					xpos = xmax - (childxmax->boundingRect().width() / 2);
					std::cout << "xmax xpos: " << xpos << std::endl;
					ypos = (ymax + C) - (childxmax->boundingRect().height() / 2);
					std::cout << "xmax ypos: " << ypos << std::endl;
					childxmax->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);

					childymin->setFont(font);
					xpos = (xmin - D) - (childymin->boundingRect().width() / 2);
					std::cout << "ymin xpos: " << xpos << std::endl;
					ypos = ymax - (childymin->boundingRect().height() / 2);
					std::cout << "ymin ypos: " << ypos << std::endl;
					childymin->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);

					childymax->setFont(font);
					xpos = (xmin - D) - (childymax->boundingRect().width() / 2);
					std::cout << "ymax xpos: " << xpos << std::endl;
					ypos = ymin - (childymax->boundingRect().height() / 2);
					std::cout << "ymax ypos: " << ypos << std::endl;
					childymax->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);

					//QRectF childRectTit = childTitle->sceneBoundingRect();
					childTitle->setFont(font);
					std::cout << "Pre-Title xpos: " << xpos << std::endl;
					xpos = ((xmax + xmin) / 2) - (childTitle->boundingRect().width() / 2);
					std::cout << "Title xpos: " << xpos << std::endl;
					ypos = (ymin - A) - (childTitle->boundingRect().height() / 2);
					std::cout << "Title ypos: " << ypos << std::endl;
					childTitle->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);


					childXLabel->setFont(font);
					//QRectF childRectXLab = childXLabel->sceneBoundingRect();
					xpos = ((xmax + xmin) / 2) - (childXLabel->boundingRect().width() / 2);
					std::cout << "Xlabel xpos: " << xpos << std::endl;
					ypos = (ymax + A) - (childXLabel->boundingRect().height() / 2);
					std::cout << "Xlabel ypos: " << ypos << std::endl;
					childXLabel->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);


					childYLabel->setFont(font);
					//QRectF childRectYLab = childYLabel->sceneBoundingRect();
					xpos = (xmin - B) - (childYLabel->boundingRect().height() / 2);
					std::cout << "Ylabel xpos: " << xpos << std::endl;
					ypos = ((ymax + ymin) / 2) + (childYLabel->boundingRect().width() / 2);
					std::cout << "Ylabel ypos: " << ypos << std::endl;
					childYLabel->setRotation(-90);
					childYLabel->setPos(xpos, ypos);
					childView->fitInView(childScene->itemsBoundingRect(), Qt::KeepAspectRatio);


					i = 0;
					xmin = 0;
					xmax = 0;
					ymin = 0;
					ymax = 0;
					xscale = 0;
					yscale = 0;
					xpos = 0;
					ypos = 0;
					XMIN = "";
					XMAX = "";
					YMIN = "";
					YMAX = "";
					TITLE = "";
					XLABEL = "";
					YLABEL = "";

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