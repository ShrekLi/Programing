#include "OperateCurve.h"

OperateCurve::OperateCurve(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::OperateCurveClass)
{
	ui->setupUi(this);

	init_curve_parameter();
	init_signals_and_slots();
	init_curve_type_combo();
	init_curve_chart_view();
}


OperateCurve::~OperateCurve()
{
	delete ui;
}


void OperateCurve::init_curve_parameter()
{
	minX = 0;
	maxX = 10;
	minY = 0;
	maxY = 100;
	curveIndex = 0;
	int map[2] = { 0x696969, 0x1E90FF };
	memcpy(color_map, map, sizeof(int)* 2);

	QScatterSeries *scatter1 = new QScatterSeries();
	QSplineSeries *spline1 = new QSplineSeries();
	// y = 10 * x
	for (int i = 0; i < 11; i++)
	{
		scatter1->append(i, 10 * i);
		spline1->append(i, 10 * i);
	}
	scatterList.append(scatter1);
	splineList.append(spline1);

	QScatterSeries *scatter2 = new QScatterSeries();
	QSplineSeries *spline2 = new QSplineSeries();
	// y = x * x
	for (int i = 0; i < 11; i++)
	{
		scatter2->append(i, i * i);
		spline2->append(i, i * i);
	}
	scatterList.append(scatter2);
	splineList.append(spline2);
}


void OperateCurve::init_signals_and_slots()
{
	QObject::connect(ui->widgetChart, SIGNAL(signalMouseMovePoint(QPoint)),
		this, SLOT(slotMouseMovePoint(QPoint)));	// mouse drag event
	QObject::connect(ui->widgetChart, SIGNAL(signalCurrentDragPoint(QPointF)),
		this, SLOT(slotCurrentDragPoint(QPointF)));	// drag point event
}


void OperateCurve::init_curve_type_combo()
{
	QStringList curvetype;
	curvetype << "y = 10 * x" << "y = x * x";
	ui->comboCurveType->addItems(curvetype);
	ui->comboCurveType->setCurrentIndex(curveIndex);
}


void OperateCurve::init_curve_chart_view()
{
	QChart *chart = new QChart();

	// add series
	for (int i = 0; i < scatterList.count(); i++)
	{
		QScatterSeries *scatter = scatterList.at(i);
		QSplineSeries *spline = splineList.at(i);

		QPen pen;
		pen.setColor(color_map[i]);
		pen.setWidth(2);
		spline->setPen(pen);

		scatter->setMarkerShape(QScatterSeries::MarkerShapeCircle);
		scatter->setBorderColor(color_map[i]);
		scatter->setBrush(QBrush(color_map[i]));
		scatter->setMarkerSize(8);

		if (i != curveIndex)
		{
			scatter->hide();
			spline->hide();
		}

		chart->addSeries(spline);
		chart->addSeries(scatter);
	}

	// chart setting
	chart->legend()->hide();
	chart->createDefaultAxes();
	chart->axisY()->setRange(minY, maxY);
	chart->axisY()->setTitleText("y");
	chart->axisX()->setRange(minX, maxX);
	chart->axisX()->setTitleText("x");
	chart->setMargins(QMargins(1, 1, 1, 1));

	ui->widgetChart->setChart(chart);
	ui->widgetChart->setRenderHint(QPainter::Antialiasing);
	ui->widgetChart->setSeriesIndex(0);
	ui->widgetChart->setForceCorrectnessOnDrag(false);
	ui->widgetChart->setYRange(minY, maxY);
	ui->widgetChart->setSeriesList(scatterList, splineList);
}


void OperateCurve::curve_type_combo_changed(int index)
{
	if (scatterList.isEmpty() || splineList.isEmpty())
		return;

	// update series index
	curveIndex = index;
	ui->widgetChart->setSeriesIndex(curveIndex);

	// set current curve display
	for (int i = 0; i < scatterList.count(); i++)
	{
		scatterList.at(i)->hide();
		splineList.at(i)->hide();
	}
	scatterList.at(curveIndex)->show();
	splineList.at(curveIndex)->show();
}


void OperateCurve::force_correctness_on_drag_checked(bool status)
{
	ui->widgetChart->setForceCorrectnessOnDrag(status);
}


void OperateCurve::set_value_on_clicked()
{
	// check value
	QString strx = ui->editX->text();
	QString stry = ui->editY->text();
	if (strx.isEmpty() || stry.isEmpty())
		return;

	int x = strx.toInt();
	int y = stry.toInt();
	if (y < minY)
		ui->editY->setText(QString::asprintf("%d", y));
	else if (y > maxY)
		ui->editY->setText(QString::asprintf("%d", y));

	// set value and update curve data
	QPointF point((float)x, (float)y);
	ui->widgetChart->setCurrentPointValue(point);
}


void OperateCurve::slotMouseMovePoint(QPoint point)
{
	// convert to chart value
	QPointF curPoint = ui->widgetChart->chart()->mapToValue(point);	

	// update position label
	if (curPoint.x() >= minX && curPoint.x() <= maxX && 
		curPoint.y() >= minY && curPoint.y() <= maxY)
	{
		ui->labelPosition->setText(QString::asprintf("Position: X=%d, Y=%d.",
			(int)curPoint.x(), (int)curPoint.y()));
	}
	else
	{
		ui->labelPosition->setText(QString::asprintf("Position: Out of chart range."));
	}	
}


void OperateCurve::slotCurrentDragPoint(QPointF point)
{
	ui->editX->setText(QString::asprintf("%d", (int)point.x()));
	ui->editY->setText(QString::asprintf("%d", (int)point.y()));
}


