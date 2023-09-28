#ifndef QVIEWER_WIDGET_H
#define QVIEWER_WIDGET_H

#include <QtGui>
#include <QWidget>
#include <QWindow>
#include <iostream>

#include <osgViewer/Viewer>
#include <osgQt/GraphicsWindowQt>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class QViewerWidget : public QWidget
{
public:

    QViewerWidget(const QRect &geometry);

    virtual ~QViewerWidget();

    osg::Group *getScene();

    osgViewer::Viewer *getViewer();

protected:

    osg::ref_ptr<osg::Group> scene;

    osgViewer::Viewer viewer;

private:

    osgQt::GraphicsWindowQt *createGraphicsWindow(const QRect &geometry);

    void initCamera(const QRect &geometry);

    void paintEvent(QPaintEvent *);

    osg::ref_ptr<osgQt::GraphicsWindowQt> m_gw;
};

#endif // QVIEWER_WIDGET_H
