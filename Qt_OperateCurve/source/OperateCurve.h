#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_OperateCurve.h"

#include "CurveChartView.h"


class OperateCurve : public QMainWindow
{
	Q_OBJECT

public:
	OperateCurve(QWidget *parent = Q_NULLPTR);
	~OperateCurve();

private:
	Ui::OperateCurveClass *ui;

	QList<QScatterSeries *> scatterList;
	QList<QSplineSeries *> splineList;
	int color_map[2];
	int curveIndex;
	int minX;
	int maxX;
	int minY;
	int maxY;

	void init_curve_parameter();
	void init_signals_and_slots();
	void init_curve_type_combo();
	void init_curve_chart_view();
	

private slots:
	void curve_type_combo_changed(int index);
	void force_correctness_on_drag_checked(bool status);
	void set_value_on_clicked();
	
	void slotMouseMovePoint(QPoint point);
	void slotCurrentDragPoint(QPointF point);
};
