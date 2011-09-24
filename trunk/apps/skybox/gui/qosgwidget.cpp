
// Copyright (c) 2011, Daniel Müller <dm@g4t3.de>
// Computer Graphics Systems Group at the Hasso-Plattner-Institute, Germany
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright 
//     notice, this list of conditions and the following disclaimer in the 
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Computer Graphics Systems Group at the 
//     Hasso-Plattner-Institute (HPI), Germany nor the names of its 
//     contributors may be used to endorse or promote products derived from 
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.


// Taken and modified from openscenegraph-2.8.5/examples/osgviewerQT

#include "qosgwidget.h"

#include <osg/Config>


#if defined(_MSC_VER) && defined(OSG_DISABLE_MSVC_WARNINGS)
// disable warning "'QtConcurrent::BlockSizeManager' : assignment operator could not be generated"
#pragma warning( disable : 4512 )
#endif

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtGui/QtGui>
#include <QtGui/QWidget>
using Qt::WindowFlags;


#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>

#include <osgViewer/ViewerEventHandlers>


#if defined(WIN32) && !defined(__CYGWIN__)

#include <osgViewer/api/Win32/GraphicsWindowWin32>
typedef HWND WindowHandle;
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;

#elif defined(__APPLE__)  // Assume using Carbon on Mac.

#include <osgViewer/api/Carbon/GraphicsWindowCarbon>
typedef WindowRef WindowHandle;
typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData;

#else // all other unix

#include <osgViewer/api/X11/GraphicsWindowX11>
typedef Window WindowHandle;
typedef osgViewer::GraphicsWindowX11::WindowData WindowData;

#endif


#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osgDB/ReadFile>

#include <osg/GLExtensions>
#include <osg/GL2Extensions>



QOsgWidget::QOsgWidget(QWidget *parent)
:   QWidget(parent)
{
    setAcceptDrops(true);

    createContext();

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    setFocusPolicy(Qt::ClickFocus);
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

#if defined(__APPLE__) 
    // Extract a WindowPtr from the HIViewRef that QWidget::winId() returns.
    // Without this change, the peer tries to call GetWindowPort on the 
    // HIViewRef which returns 0 and we only render white.
    traits->inheritedWindowData = new WindowData(HIViewGetWindow((HIViewRef)winId()));

#else // all others
    traits->inheritedWindowData = new WindowData(winId());
#endif

    traits->vsync = false;  // Do not force vsync here (let driver decide).

     osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    m_gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
}


void QOsgWidget::destroyEvent(bool destroyWindow, bool destroySubWindows)
{
    if(!m_gw.valid()) 
        return;

    m_gw->getEventQueue()->closeWindow();
}


void QOsgWidget::closeEvent(QCloseEvent *event)
{
    if(!m_gw.valid()) 
        return;

    event->accept();

    m_gw->getEventQueue()->closeWindow();
}


void QOsgWidget::resizeEvent(QResizeEvent *event)
{
    if(!m_gw.valid()) 
        return;

    const QSize & size = event->size();

    m_gw->getEventQueue()->windowResize(0, 0, size.width(), size.height() );
    m_gw->resized(0, 0, size.width(), size.height());

    // emit resizeWidget(size.width(), size.height());
}


void QOsgWidget::keyPressEvent(QKeyEvent *event)
{
    if(!m_gw.valid()) 
        return;

    const osgGA::GUIEventAdapter::KeySymbol keySymbol = 
        static_cast<osgGA::GUIEventAdapter::KeySymbol>(*(event->text().toAscii().data()));

    m_gw->getEventQueue()->keyPress(keySymbol);
}


void QOsgWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(!m_gw.valid()) 
        return;

    const osgGA::GUIEventAdapter::KeySymbol keySymbol = 
        static_cast<osgGA::GUIEventAdapter::KeySymbol>(*(event->text().toAscii().data()));

    m_gw->getEventQueue()->keyRelease(keySymbol);
}


void QOsgWidget::mousePressEvent(QMouseEvent *event)
{
    if(!m_gw.valid()) 
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
    m_gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}


void QOsgWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!m_gw.valid()) 
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
    m_gw->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);
}


void QOsgWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(!m_gw.valid()) 
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
    m_gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}


void QOsgWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_gw.valid()) 
        return;

    m_gw->getEventQueue()->mouseMotion(event->x(), event->y());
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