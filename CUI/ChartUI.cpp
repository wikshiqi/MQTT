#include "UI/ChartUI.h"
#include <QVBoxLayout>
#include <QColor>

ChartUI::ChartUI(QWidget *parent) : QWidget(parent), m_time(0)
{
    m_chart = new QChart();
    m_chart->setTitle("OneNET 实时数据");

    axisX = new QValueAxis;
    axisY = new QValueAxis;
    axisX->setRange(0, MAX_POINT);
    axisY->setRange(0, 100);
    axisX->setTitleText("时间");
    axisY->setTitleText("数值");

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);

    m_view = new QChartView(m_chart);
    m_view->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(m_view);
    l->setContentsMargins(0,0,0,0);
}

void ChartUI::updateOneNetData(const QJsonObject& data)
{
    m_time++;
    if(m_time > MAX_POINT) m_time = 0;

    for(auto k = data.begin(); k != data.end(); ++k){
        QString name = k.key();
        double val = k.value().toDouble();

        if(!m_series.contains(name)){
            auto s = new QLineSeries;
            s->setName(name);
            m_chart->addSeries(s);
            s->attachAxis(axisX);
            s->attachAxis(axisY);
            m_series[name] = s;
        }
        m_series[name]->append(m_time, val);
        if(m_series[name]->count() > MAX_POINT)
            m_series[name]->remove(0);
    }
    axisX->setRange(m_time - MAX_POINT, m_time);
}