#ifndef GLSPHEREGRID
#define GLSPHEREGRID

#include <SimpleGui/GLObject.h>
#include <SimpleGui/GLCVars.h>

extern GLWindowConfig gConfig;

class GLSphereGrid : public GLObject
{
  
 public:
  GLSphereGrid() {
    m_nDisplayList = -1;
  }

  void draw() 
  {
    if(m_nDisplayList == -1) {
      m_nDisplayList = glGenLists(1);
      glNewList(m_nDisplayList, GL_COMPILE);

        // to check lighting
        glColor4f( 1, 1, 1, 1 );
        for( int y = -100; y < 100; y+=10 ){
            for( int x = -100; x < 100; x+=10 ){
                glPushMatrix();
                glTranslatef( x, y, 0 );
                glutSolidSphere( 2, 32, 32 );
                glPopMatrix();
            }
        }

	glEndList();
    }
    else {
      if( gConfig.m_bDebugLighting ){
	glCallList(m_nDisplayList);
      }
    }

  }

 private:
  GLint m_nDisplayList;
};

#endif
