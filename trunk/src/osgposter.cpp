
/* OpenSceneGraph example, osgposter.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include "osgposter.h"

#include <osg/ArgumentParser>
#include <osg/Camera>
#include <osg/Texture2D>

//#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgViewer/Viewer>

#include <iostream>
#include <sstream>
#include <ctime>
#include <assert.h>

namespace osgHimmel
{


PrintPosterHandler::PrintPosterHandler()
:   
    m_poster(NULL)
,   m_path("")
,   m_make(false)
,   m_making(false)

,   m_isFinished(false)
,   m_outputTiles(false)
,   m_currentRow(0)
,   m_currentColumn(0)
,   m_cameraIndex(0)
,   m_cameraRoot(0)
{
}

PrintPosterHandler::~PrintPosterHandler()
{
}


void PrintPosterHandler::setDefaultPath(const std::string &path)
{
    m_path = path;
}


bool PrintPosterHandler::handle(
    const osgGA::GUIEventAdapter& ea
,   osgGA::GUIActionAdapter& aa)
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);

    if(!view) 
        return false;

    switch(ea.getEventType())
    {
        case osgGA::GUIEventAdapter::FRAME:
        {
            // Wait until all paged LOD are processed
            if(view->getDatabasePager() && view->getDatabasePager()->getRequestsInProgress())
                break;

            if(m_isFinished)
            {
                const osg::FrameStamp* fs = view->getFrameStamp();
                if((fs->getFrameNumber() % 2) == 0)
                {
                    aggregateAndFreeTiles(); // Record images and unref them to free memory
                    std::cout << "finished" << std::endl;

                    write();
                    m_isFinished = false;
                }
            }

            if(m_make)
                make(view);

            if(m_making)
            {

                // Every "copy-to-image" process seems to be finished in 2 frames.
                // So record them and dispatch cameras to next tiles.

                const osg::FrameStamp* fs = view->getFrameStamp();

                if((fs->getFrameNumber() % 2) == 0)
                {
                    // Record images and unref them to free memory
                    aggregateAndFreeTiles();

                    osg::Camera* camera = 0;

                    while((camera = getAvailableCamera()) != NULL)
                    {
                        std::cout << "Binding sub-camera " << m_currentRow << "_" << m_currentColumn
                                    << " to image..." << std::endl;

                        bindCameraToImage(camera, m_currentRow, m_currentColumn);

                        if (m_currentColumn < m_tileColumns - 1)
                            m_currentColumn++;
                        else
                        {
                            if(m_currentRow < m_tileRows - 1)
                            {
                                m_currentRow++;
                                m_currentColumn = 0;
                            }
                            else
                            {
                                m_making = false;
                                m_isFinished = true;
                                    
                                std::cout << "Sub-cameras dispatching finished." << std::endl;
                                break;
                            }
                        }
                    }
                    m_cameraIndex = m_cameraRoot->getNumChildren();
                }
            }
            break;
        }

        case osgGA::GUIEventAdapter::KEYDOWN:
        {
            if ( ea.getKey()=='p' || ea.getKey()=='P' )
            {
                print();
                return true;
            }
            break;
        }

        default:
            break;
    }

    return false;
}


osg::Camera* PrintPosterHandler::getAvailableCamera()
{
    // Find an available camera for rendering current tile image.
    if(!m_cameraIndex || !m_cameraRoot.valid())
        return NULL;
        
    return dynamic_cast<osg::Camera*>(m_cameraRoot->getChild(--m_cameraIndex));
}


void PrintPosterHandler::bindCameraToImage(
    osg::Camera* camera
,   const int row
,   const int col)
{
    std::stringstream stream;
    stream << "image_" << row << "_" << col;

    osg::ref_ptr<osg::Image> image = new osg::Image;

    image->setName(stream.str());
    image->allocateImage(static_cast<int>(m_tileSize.x()), static_cast<int>(m_tileSize.y())
        , 1, GL_RGBA, GL_UNSIGNED_BYTE);

    m_tiles[t_position(row, col)] = image.get();

    // calculate projection matrix offset of each tile
    osg::Matrixf offsetMatrix = osg::Matrixf::scale(m_tileColumns, m_tileRows, 1.0) 
        * osg::Matrixf::translate(m_tileColumns - 1 - 2 * col, m_tileRows - 1 - 2 * row, 0.0);
        
    camera->setViewMatrix(m_currentViewMatrix);
    camera->setProjectionMatrix(m_currentProjectionMatrix * offsetMatrix);

    // Reattach cameras and new allocated images
    camera->setRenderingCache(NULL);  // FIXME: Uses for reattaching camera with image, maybe inefficient?
    camera->detach(osg::Camera::COLOR_BUFFER);
    camera->attach(osg::Camera::COLOR_BUFFER, image.get(), 0, 0);
}


void PrintPosterHandler::aggregateAndFreeTiles()
{
    t_tiles::iterator i(m_tiles.begin());
    const t_tiles::const_iterator iEnd(m_tiles.end());

    for(; i != iEnd; ++i)
    {
        osg::Image* image = i->second.get();

        if(m_poster.valid())
        {
            // FIXME: A stupid way to combine tile images to final result. Any better ideas?

            const unsigned int row = i->first.first;
            const unsigned int col = i->first.second;

            for(int s = 0; s < image->s(); ++s)
            {
                for(int t = 0; t < image->t(); ++t)
                {
                    unsigned char* src = image->data(s, t);
                    unsigned char* target = m_poster->data(
                        s + col * static_cast<int>(m_tileSize.x()),
                        t + row * static_cast<int>(m_tileSize.y()));

                    for(int u = 0; u < 4; ++u)
                        *(target + u) = *(src++);
                }
            }
        }
    }
    m_tiles.clear();
}


const bool PrintPosterHandler::isReady() const
{
    return !m_make && !m_making && m_cameraRoot.valid();
}


void PrintPosterHandler::print()
{
    if(!isReady())
        return;

    m_make = true;
}


void PrintPosterHandler::print(
    const unsigned int width
,   const unsigned int height)
{
    if(!isReady())
        return;

    setSize(height, width);
    return print();
}


void PrintPosterHandler::setSize(
    const unsigned int width
,   const unsigned int height)
{
    m_width  = width  > 0 ? width  : 1;
    m_height = height > 0 ? height : 1;
}


void PrintPosterHandler::make(osgViewer::View* view)
{
    assert(m_make);
    assert(!m_making);

    m_poster = new osg::Image;
    m_poster->allocateImage(m_width, m_height, 1, GL_RGBA, GL_UNSIGNED_BYTE);


    m_tileRows    = m_height / m_tileSize.y();
    m_tileColumns = m_width / m_tileSize.x();

    m_currentRow  = m_currentColumn = 0;
    m_cameraIndex = m_cameraRoot->getNumChildren();

    m_currentViewMatrix       = view->getCamera()->getViewMatrix();
    m_currentProjectionMatrix = view->getCamera()->getProjectionMatrix();

    m_tiles.clear();


    m_make = false;
    m_making = true;
}


const osg::Image *PrintPosterHandler::get() const 
{
    return m_poster.get(); 
}


void PrintPosterHandler::write() const
{
    return write(m_path);
}


void PrintPosterHandler::write(const std::string &filePath) const
{
    const std::string fileName = (filePath.empty() ? "" : filePath + "/") + genFileName();
    osgDB::writeImageFile(*m_poster, fileName);
}


const std::string PrintPosterHandler::genFileName()
{
    std::time_t rawtime = std::time(NULL);
    struct tm* timeinfo = localtime(&rawtime);
    
    char p[16];
    std::strftime(p, 16,"%Y%m%d%H%M%S", timeinfo);
    std::puts(p);

    std::string fileName = "_poster_" + std::string(p) + ".bmp";

    return fileName;
}


void PrintPosterHandler::setOutputTiles(const bool b)
{ 
    m_outputTiles = b; 
}

const bool PrintPosterHandler::getOutputTiles() const 
{ 
    return m_outputTiles; 
}


void PrintPosterHandler::setTileSize(
    const int w
,   const int h) 
{
    m_tileSize.set(w, h);
}

const osg::Vec2f& PrintPosterHandler::getTileSize() const 
{ 
    return m_tileSize; 
}


void PrintPosterHandler::setCameraRoot( osg::Group* root ) 
{
    m_cameraRoot = root;
}

const osg::Group* PrintPosterHandler::getCameraRoot() const 
{
    return m_cameraRoot.get(); 
}

} // namespace osgHimmel