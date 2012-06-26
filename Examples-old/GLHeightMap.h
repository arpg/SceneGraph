#ifndef GLTERRAIN_H
#define GLTERRAIN_H

#include "HeightMap.h"
#include "SimpleGui/GLObject.h"

class GLHeightMap : public GLObject
{
    public:
        GLHeightMap(HeightMap *pHeightMap)
        {
            m_nDisplayList = -1;
            m_pHeightMap = pHeightMap;
        }

        void draw()
        {
            //draw the height map
            glEnable( GL_DEPTH_TEST ); // so picking will work
            glEnable( GL_LIGHTING );
            glEnable( GL_LIGHT0 );
            glEnable( GL_COLOR_MATERIAL ); 

            if(m_nDisplayList == -1) {
                m_nDisplayList = glGenLists(1);
                glNewList(m_nDisplayList, GL_COMPILE);


                float dx = 1.0;
                float dy = 1.0;

                glBegin(GL_QUADS);
                int n=0;
                for( float x =-50;x<50; x += dx ){ // nima, add braces everywhere-it's good practice
                    n++;
                    for( float y = -50;y<50; y += dy ) { // open brace on same line
                        if( n++&0x00000001 ){ //modulo 2
                            glColor3f(1.0f,1.0f,1.0f); //white
                        }
                        else{
                            glColor3f(0.0f,0.0f,0.0f); //black
                        }

                        glVertex3f(    x, y+dy, m_pHeightMap->GetHeight( x, y+dy ) );
                        glNormal3fv( m_pHeightMap->GetNormal(x,y+dy,dx,dy) );

                        glVertex3f( x+dy, y+dy, m_pHeightMap->GetHeight( x+dx,y+dy ) );
                        glNormal3fv( m_pHeightMap->GetNormal(x+dx,y+dy,dx,dy) );

                        glVertex3f( x+dx,    y, m_pHeightMap->GetHeight( x+dx,y ) );
                        glNormal3fv( m_pHeightMap->GetNormal(x+dx,y,dx,dy) );

                        glVertex3f(    x,    y, m_pHeightMap->GetHeight(x,y) );
                        glNormal3fv( m_pHeightMap->GetNormal(x,y,dx,dy) );
                    }
                }
                glEnd();

                glEndList();
            }
            
            glCallList(m_nDisplayList);

        }

    private:
        HeightMap *m_pHeightMap;
        GLint m_nDisplayList;

};

#endif // GLTERRAIN_H
