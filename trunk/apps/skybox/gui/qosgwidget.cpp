
#include "qosgwidget.h"

#include <QtCore/QString>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>

#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>

#include <osgViewer/ViewerEventHandlers>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <osgViewer/api/Win32/GraphicsWindowWin32>
typedef HWND WindowHandle;
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;
#elif defined(__APPLE__) && defined(APPLE_PRE_10_3)
#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
typedef WindowRef WindowHandle;
typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData;
#else // all other unix
#include <osgViewer/api/X11/GraphicsWindowX11>
typedef Window WindowHandle;
typedef osgViewer::GraphicsWindowX11::WindowData WindowData;
#endif

#include <osg/GLExtensions>
#include <osg/GL2Extensions>


QOsgWidget::QOsgWidget(QWidget *parent)
:   QFrame(parent)
{
    setAcceptDrops(true);

    createContext();

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
}


void QOsgWidget::createContext()
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance();
    ds->setStereo(false);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    traits->readDISPLAY();
    if (traits->displayNum<0) traits->displayNum = 0;

    traits->screenNum = 0;
    traits->x = x();
    traits->y = y();
    traits->width = width();
    traits->height = height();
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = 0;

    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = new WindowData(winId());

    traits->vsync = false;

     osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    _gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
}


void QOsgWidget::destroyEvent(bool destroyWindow, bool destroySubWindows)
{   
    if(!_gw.valid()) 
        return;

    _gw->getEventQueue()->closeWindow();
}


void QOsgWidget::closeEvent(QCloseEvent *event)
{
    if(!_gw.valid()) 
        return;

    event->accept();
    _gw->getEventQueue()->closeWindow();
}


void QOsgWidget::resizeEvent(QResizeEvent *event)
{
    if(!_gw.valid()) 
        return;

    const QSize & size = event->size();

    _gw->getEventQueue()->windowResize(0, 0, size.width(), size.height() );
    _gw->resized(0, 0, size.width(), size.height());

    emit resizeWidget(size.width(), size.height());
}


void QOsgWidget::keyPressEvent(QKeyEvent *event)
{
    if(!_gw.valid()) 
        return;

    _gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol)*(event->text().toAscii().data()));
}


void QOsgWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(!_gw.valid()) 
        return;

    int c = *event->text().toAscii().data();
    _gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol)(c));
}


void QOsgWidget::mousePressEvent(QMouseEvent *event)
{
    if(!_gw.valid()) 
        return;

    // if you like cross
    setCursor(Qt::CrossCursor);

    int button(0);
    switch(event->button())
    {
    case(Qt::LeftButton):
        button = 1;
        break;
    case(Qt::MidButton):
        button = 2;
        break;
    case(Qt::RightButton):
        button = 3;
        break;
    case(Qt::NoButton):
    default:
        break;
    }
    _gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}


void QOsgWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!_gw.valid()) 
        return;

    int button(0);
    switch(event->button())
    {
    case(Qt::LeftButton):
        button = 1;
        break;
    case(Qt::MidButton):
        button = 2;
        break;
    case(Qt::RightButton): 
        button = 3;
        break;
    case(Qt::NoButton):
    default: 
        break;
    }
    _gw->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);
}


void QOsgWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(!_gw.valid()) 
        return;

    int button(0);

    switch(event->button())
    {
    case(Qt::LeftButton): 
        button = 1; 
        break;
    case(Qt::MidButton): 
        button = 2; 
        break;
    case(Qt::RightButton):
        button = 3;
        break;
    case(Qt::NoButton): 
    default: 
        break;
    }
    _gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}


void QOsgWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!_gw.valid()) 
        return;

    _gw->getEventQueue()->mouseMotion(event->x(), event->y());
}


void QOsgWidget::dragEnterEvent(QDragEnterEvent *event) 
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}


void QOsgWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}


void QOsgWidget::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if(mimeData->hasUrls())
        emit mouseDrop(mimeData->urls());

    event->acceptProposedAction();
}