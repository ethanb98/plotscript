#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QWidget>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>

class OutputWidget : public QWidget {
	Q_OBJECT
public:
	OutputWidget(QWidget * parent = nullptr);

private slots:
	void receiveString(QString str);

private:
	QGraphicsView * childView = new QGraphicsView(this);
	QGraphicsScene * childScene = new QGraphicsScene(this);
};
#endif