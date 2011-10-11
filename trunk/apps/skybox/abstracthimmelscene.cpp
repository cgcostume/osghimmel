
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

#include "abstracthimmelscene.h"

#include <osg/Camera>

#include <osgViewer/View>

namespace
{
	// Properties

    static const QString GROUP_ABSTRACTSCENE (TR("Abstract Scene"));
	static const QString GROUP_ABSTRACTHIMMEL(TR("Abstract Himmel"));

    static const QString PROPERTY_FOV(TR("Field of View"));

    static const QString PROPERTY_TIME   (TR("Time"));
    static const QString PROPERTY_SECONDS(TR("SecondsPerCycle"));
}


AbstractHimmelScene::AbstractHimmelScene(
    osgViewer::View* view
,   const QSize &viewportSize)
:   osg::Group()
,   AbstractPropertySupport()


,   m_camera(NULL)
,   m_size(viewportSize)
,   m_fov(40.f)
,   m_clearColor(osg::Vec4(1.f, 1.f, 1.f, 1.f))
,   m_viewport(NULL)
{
    assert(view);

    m_camera = view->getCamera();

	m_camera->setViewport(new osg::Viewport(0, 0, m_size.width(), m_size.height()));
	m_camera->setProjectionMatrixAsPerspective(
		m_fov, static_cast<double>(m_size.width()) / static_cast<double>(m_size.height()), 0.1f, 8.0f);

	m_camera->setClearColor(m_clearColor);

	osg::ref_ptr<osg::Viewport> m_viewport = m_camera->getViewport();

	view->setSceneData(this);
}


AbstractHimmelScene::~AbstractHimmelScene()
{
}


void AbstractHimmelScene::propertyChanged(QtProperty *p)
{
}


void AbstractHimmelScene::registerProperties()
{
    QtProperty *himmelGroup = createGroup(GROUP_ABSTRACTHIMMEL);

	createProperty(*himmelGroup , PROPERTY_TIME, 0.0, 0.0, 1.0, 0.02);
	registerForFastAccess(PROPERTY_TIME);
}
