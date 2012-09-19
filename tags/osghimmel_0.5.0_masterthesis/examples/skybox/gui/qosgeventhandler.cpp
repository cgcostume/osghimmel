
// Copyright (c) 2011-2012, Daniel Müller <dm@g4t3.de>
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

#include "qosgeventhandler.h"


QOsgEventHandler::QOsgEventHandler(const float fov)
:   m_fov(fov)
,   m_fovBackup(fov)
{
}

bool QOsgEventHandler::handle(
    const osgGA::GUIEventAdapter &ea
,   osgGA::GUIActionAdapter &)
{
    if(ea.getEventType() == osgGA::GUIEventAdapter::FRAME
    || ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
        return false;

    switch(ea.getEventType())
    {

  case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                if(ea.getKey() == '-'
                || ea.getKey() == '+')
                {
                    const float f = 1.00f
                        + (ea.getKey() == '+' ? +0.004f : -0.004f);

                    if(m_fov * f >= 1.f && m_fov * f <= 179.f)
                        m_fov *= f;

                    emit fovChanged(m_fov);
                }
            }
            break;

    case(osgGA::GUIEventAdapter::SCROLL):
        {
            const float f = 1.00f
                + (ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN ? -0.08f : +0.08f);

            if(m_fov * f >= 1.f && m_fov * f <= 179.f)
                m_fov *= f;

            emit fovChanged(m_fov);

            return true;
        }
        break;

    case(osgGA::GUIEventAdapter::RELEASE):

        if(ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON
        && (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) != 0)
        {
            m_fov = m_fovBackup;
            emit fovChanged(m_fov);

            return true;
        }
        break;

    default:
        break;
    };
    return false;
}