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
	if (clearScreen == true) {
		childScene->clear();
	}
	std::istringstream iss(str.toStdString());
	if (!interp.parseStream(iss)) { 
		childScene->addText(QString("Error: Invalid, could not parse"));
	}
	else {
		try {
			Expression exp = interp.evaluate();
			// If a list, do not clear screen and recursively collect information
			if (exp.head().isLambda()) {
				childScene->clear();
			}
			else if (exp.isHeadList()) {
				clearScreen = false;
				if (exp.isPoint()) {
					for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
						double size = exp.req();
						double x = (exp.tail0() - (size / 2));
						double y = (exp.tail1() - (size / 2));
						QPen pen = QPen(Qt::black);
						QBrush brush = QBrush(Qt::black);
						childScene->QGraphicsScene::addEllipse(x, y, size, size, pen, brush);
					}
				}
				else if (exp.isLine()) {
					double thicc = exp.req();
					double x = (exp.tail0() - (thicc / 2));
					double y = (exp.tail1() - (thicc / 2));
					QPen pen = QPen(Qt::black);
					QBrush brush = QBrush(Qt::black);
					childScene->QGraphicsScene::addEllipse(x, y, thicc, thicc, pen, brush);

				}
				/*else if (exp.isText()) {

				}*/
				/*for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
					childScene->addText(QString::fromStdString((*e).transferString()));
				}*/
			}
			else {
				childScene->addText(QString::fromStdString(exp.transferString()));
			}
		}
		catch (const SemanticError & ex) {
			
			QString error = QString::fromStdString(ex.what());
			childScene->addText(error);
		}
	}
}
