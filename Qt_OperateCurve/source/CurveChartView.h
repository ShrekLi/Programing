#ifndef _CURVECHARTVIEW_H
#define _CURVECHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>
#include <QtCore/QtMath>

QT_CHARTS_USE_NAMESPACE

class CurveChartView : public QChartView
{
	Q_OBJECT

public:
	CurveChartView(QWidget *parent = Q_NULLPTR);
	~CurveChartView();

	void setForceCorrectnessOnDrag(bool status);
	void setSeriesIndex(int index);
	void setCurrentPointValue(const QPointF &value);
	void setSeriesList(QList<QScatterSeries *> sList, 
		QList<QSplineSeries *> lList);
	void setYRange(int minVal, int maxVal);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	bool forceCorrectnessFlag;
	bool dragPointFlag;
	QPoint beginPoint;
	QPoint endPoint;
	int pointIndex;
	int seriesIndex;
	int dataCount;
	int minValue;
	int maxValue;

	QList<QScatterSeries *> scatterList;
	QList<QSplineSeries *> lineList;

	qreal distance(const QPointF &p1, const QPointF &p2);
	void detectDragPoint(const QPointF &point);
	void updateSeriesData(const QPointF &point);

signals:
	void signalMouseMovePoint(QPoint point);
	void signalCurrentDragPoint(QPointF point);

};

#endif

