#ifndef SUPERSAMPLEGRAPHICSVIEW_H
#define SUPERSAMPLEGRAPHICSVIEW_H

#include <QGraphicsView>

class SupersampleGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SupersampleGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void setSupersampleFactor(int factor);
    int supersampleFactor() const { return m_supersample; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_supersample = 1;
};

#endif // SUPERSAMPLEGRAPHICSVIEW_H
