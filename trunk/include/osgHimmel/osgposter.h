
//http://thread.gmane.org/gmane.comp.graphics.openscenegraph.cvs/6026

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

#pragma once
#ifndef __PRINTPOSTERHANDLER_H__
#define __PRINTPOSTERHANDLER_H__

#include "declspec.h"

#include <osgViewer/Viewer>


namespace osg
{
    class Image;
}

namespace osgHimmel
{

class OSGH_API PrintPosterHandler : public osgGA::GUIEventHandler
{
public:
    typedef std::pair<unsigned int, unsigned int> t_position;
    typedef std::map<t_position, osg::ref_ptr<osg::Image> > t_tiles;

public:

    PrintPosterHandler();
    virtual ~PrintPosterHandler();

    void print();
    void print(
        const unsigned int width
    ,   const unsigned int height);

    void write() const;
    void write(const std::string &filePath) const;

    const osg::Image *get() const;

    // config 

    // default path to save the posters
    void setDefaultPath(const std::string &path);

    // set default poster resolution in pixel
    void setSize(
        const unsigned int width
    ,   const unsigned int height);

protected:

    const bool isReady() const;
    
    void make(osgViewer::View* view);
    void finish();
    void aggregateAndFreeTiles();


    static const std::string genFileName();

public:

    void setOutputTiles(const bool b);
    const bool getOutputTiles() const;

    void setTileSize(
        const int w
    ,   const int h);
    const osg::Vec2& getTileSize() const;


    void setCameraRoot(osg::Group* root); 
    const osg::Group* getCameraRoot() const;



    virtual bool handle(
        const osgGA::GUIEventAdapter& ea
    ,   osgGA::GUIActionAdapter& aa);

protected:

    osg::Camera* getAvailableCamera();

    void bindCameraToImage(
        osg::Camera* camera
    ,   const int row
    ,   const int col);


protected:

    osg::ref_ptr<osg::Image> m_poster;
    t_tiles m_tiles;

    std::string m_path;

    unsigned int m_width;
    unsigned int m_height;
    
    bool m_make;
    bool m_making;


    bool m_isFinished;

    bool m_outputTiles;

    osg::Vec2 m_tileSize;

    int m_tileRows;
    int m_tileColumns;

    int m_currentRow;
    int m_currentColumn;
    
    unsigned int m_cameraIndex;

    osg::Matrixd m_currentViewMatrix;
    osg::Matrixd m_currentProjectionMatrix;

    osg::ref_ptr<osg::Group> m_cameraRoot;
    
};

} // namespace osgHimmel

#endif // __PRINTPOSTERHANDLER_H__