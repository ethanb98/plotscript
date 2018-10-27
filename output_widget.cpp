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
			else if (exp.head().isList()) {
				listCap(exp);
				/*for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
					childScene->addText(QString::fromStdString((*e).transferString()));
				}*/
			}
			else {
				if (exp.isText()) {
					childScene->clear();
					Expression newExp = exp.textReq();
					double x = newExp.pointTail0();
					double y = newExp.pointTail1();

					QString text = QString::fromStdString(exp.transferString().substr(1, (exp.transferString().length() - 2)));
					QGraphicsTextItem *childText = childScene->addText(text);
					childText->setPos(x, y);
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
}

void OutputWidget::listCap(Expression exp) {
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
		double x2 = exp.lineTail1x();
		double y1 = exp.lineTail0y();
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
				pen.setWidth(thicc);
				childScene->QGraphicsScene::addLine(x1, y1, x2, y2, pen);
			}
			else if ((*e).isText()) {

				Expression newExp = (*e).textReq();
				double x = newExp.pointTail0();
				double y = newExp.pointTail1();

				QString text = QString::fromStdString((*e).transferString().substr(1, ((*e).transferString().length() - 2)));
				QGraphicsTextItem *childText = childScene->addText(text);
				childText->setPos(x, y);
			}
		}
	}
}