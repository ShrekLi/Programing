
#include "CurveChartView.h"

CurveChartView::CurveChartView(QWidget *parent)
	: QChartView(parent)
{
	forceCorrectnessFlag = false;
	dragPointFlag = false;
	seriesIndex = 0;
	pointIndex = 0;
	minValue = 0;
	maxValue = 0;
	this->setDragMode(QGraphicsView::RubberBandDrag);
	this->setMouseTracking(true);
}


CurveChartView::~CurveChartView()
{
}


void CurveChartView::mousePressEvent(QMouseEvent *event)
{
	QCursor currentCusor = this->cursor();
	if (event->button() == Qt::LeftButton)
	{
		if (currentCusor.shape() == Qt::CrossCursor)
		{
			this->setDragMode(QGraphicsView::RubberBandDrag);
			beginPoint = event->pos();
		}
		else
		{
			this->setDragMode(QGraphicsView::NoDrag);
			dragPointFlag = true;
		}
	}
	QChartView::mousePressEvent(event);
}


void CurveChartView::mouseMoveEvent(QMouseEvent *event)
{
	QPoint point = event->pos();
	emit signalMouseMovePoint(point);
	if (dragPointFlag)
	{
		this->setCursor(Qt::SizeVerCursor);
		updateSeriesData(point);
	}
	else
	{
		this->setCursor(Qt::CrossCursor);
		detectDragPoint(point);
	}

	QChartView::mouseMoveEvent(event);
}


void CurveChartView::mouseReleaseEvent(QMouseEvent *event)
{
	QCursor currentCusor = this->cursor();
	if (event->button() == Qt::LeftButton)
	{
		if (currentCusor.shape() == Qt::CrossCursor)
		{
			endPoint = event->pos();
			QRectF rectF;
			rectF.setTopLeft(this->beginPoint);
			rectF.setBottomRight(this->endPoint);
			this->chart()->zoomIn(rectF);
		}
		else
		{
			dragPointFlag = false;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		this->chart()->zoomReset();
	}
	QChartView::mouseReleaseEvent(event);
}


void CurveChartView::setForceCorrectnessOnDrag(bool status)
{
	forceCorrectnessFlag = status;
}


void CurveChartView::setSeriesIndex(int index)
{
	seriesIndex = index;
}


void CurveChartView::setCurrentPointValue(const QPointF &value)
{
	if (scatterList.isEmpty() && lineList.isEmpty())
		return;

	if (pointIndex < 0 || pointIndex >= dataCount)
		return;

	QScatterSeries *scatter = scatterList.at(seriesIndex);
	QSplineSeries *spline = lineList.at(seriesIndex);
	QPointF orignal = scatter->at(pointIndex);
	scatter->replace(orignal, value);
	spline->replace(orignal, value);
}


void CurveChartView::setSeriesList(
	QList<QScatterSeries *> sList, 
	QList<QSplineSeries *> lList)
{
	scatterList = sList;
	lineList = lList;
	QScatterSeries * tmp = (QScatterSeries *)scatterList.at(0);
	dataCount = tmp->count();
}


void CurveChartView::setYRange(int minVal, int maxVal)
{
	minValue = minVal;
	maxValue = maxVal;
}


qreal CurveChartView::distance(const QPointF &p1, const QPointF &p2)
{
	return qSqrt((p1.x() - p2.x()) * (p1.x() - p2.x())
		+ (p1.y() - p2.y()) * (p1.y() - p2.y()));
}


void CurveChartView::detectDragPoint(const QPointF &point)
{
	// detect data
	if (scatterList.isEmpty() || lineList.isEmpty())
		return;

	// detect drag point
	QPointF curPoint = this->chart()->mapToValue(point);
	QScatterSeries *curSeries = scatterList.at(seriesIndex);
	QVector<QPointF> seriesData = curSeries->pointsVector();
	for (int i = 0; i < seriesData.count(); i++)
	{
		if (distance(curPoint, seriesData.at(i)) <= 1)
		{
			pointIndex = i;
			emit signalCurrentDragPoint(seriesData.at(i));
			this->setCursor(Qt::SizeVerCursor);
			break;
		}
	}
}


void CurveChartView::updateSeriesData(const QPointF &point)
{
	// detect data
	if (scatterList.isEmpty() || lineList.isEmpty())
		return;

	// force and aft to remove
	if (pointIndex <= 0 || pointIndex >= (dataCount - 1))
		return;

	// get data
	QScatterSeries *scatter = scatterList.at(seriesIndex);
	QSplineSeries *spline = lineList.at(seriesIndex);
	QPointF original = scatter->at(pointIndex);
	QPointF target = this->chart()->mapToValue(point);

	// restricted y range
	if (forceCorrectnessFlag)
	{
		QPointF previous = scatter->at(pointIndex - 1);
		QPointF next = scatter->at(pointIndex + 1);
		if (target.y() < previous.y() || target.y() > next.y())
			return;
	}
	else
	{
		if (target.y() < minValue)
			target.setY(minValue);
		else if (target.y() > maxValue)
			target.setY(maxValue);
	}

	// replace current point value
	target.setX(original.x());
	scatter->replace(original, target);
	spline->replace(original, target);

	emit signalCurrentDragPoint(target);
}



