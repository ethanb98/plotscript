#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
	setObjectName("output");
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(childView);
	childView->setScene(childScene);
}

void OutputWidget::receiveString(QString str) {


}