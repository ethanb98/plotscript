#include "input_widget.hpp"

InputWidget::InputWidget(QWidget * parent) : QPlainTextEdit(parent) {
	setObjectName("input");
	this->installEventFilter(this);
}

// Taken from stack overflow post, posted by user Nejat at the following URL:
// https://stackoverflow.com/questions/23816380/using-multiple-keys-in-qt-c
bool InputWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		pressedKeys += ((QKeyEvent*)event)->key();
		if (pressedKeys.contains(Qt::Key_Shift) && pressedKeys.contains(Qt::Key_Enter)) {
			// special instruction handling here
			// Makes this into a plain string and sent it to notebook to be changed
			emit sendString(this->toPlainText());
		}
		// Taken from stack overflow, posted by user Bill at the following URL:
		// https://stackoverflow.com/questions/8026101/correct-way-to-quit-a-qt-program
		else if (pressedKeys.contains(Qt::Key_Control) && pressedKeys.contains(Qt::Key_C)) {
			// quit the loop 
			emit QApplication::quit();
		}
	}
	else if (event->type() == QEvent::KeyRelease) {
		pressedKeys -= ((QKeyEvent*)event)->key();
	}
		// returns the event of the object
		return obj->event(event);
}