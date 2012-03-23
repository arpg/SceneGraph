#ifndef SCENEGRAPHLISTENER_H
#define SCENEGRAPHLISTENER_H

class SceneGraphListener
{
public:
    SceneGraphListener( void ) {}
    virtual ~SceneGraphListener( void ) {}

    // Functions that should be implemented by whatever uses us
    virtual bool OnSceneGraphDrawn( void ) { return false; }
};

#endif // SCENEGRAPHLISTENER_H
