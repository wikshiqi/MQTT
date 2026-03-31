#ifndef CHARTUI_H
#define CHARTUI_H

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QMap>
#include <QJsonObject>

class ChartUI : public QWidget
{
Q_OBJECT
public:
    explicit ChartUI(QWidget *parent = nullptr);
    void updateOneNetData(const QJsonObject& data);

private:
    QChart* m_chart;
    QChartView* m_view;
    QMap<QString, QLineSeries*> m_series;
    QValueAxis* axisX;
    QValueAxis* axisY;
    int m_time;
    const int MAX_POINT = 20;
};

#endif