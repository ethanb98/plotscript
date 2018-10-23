#ifndef INPUT_WIDGET_H
#define INPUT_WIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QApplication>
#include <QSet>

class InputWidget : public QPlainTextEdit {
	Q_OBJECT
public:
	InputWidget(QWidget * parent = (QWidget *)nullptr);

signals:
	void sendString(QString str);

private:
	bool eventFilter(QObject *obj, QEvent * event);
	QSet<int> pressedKeys;
};
#endif