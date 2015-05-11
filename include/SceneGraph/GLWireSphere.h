#ifndef GLWIRESPHERE_H
#define GLWIRESPHERE_H

#include <Eigen/Eigen>

#define rads 180/M_PI

namespace SceneGraph
{

class GLWireSphere : public GLObject
{

public:

    GLWireSphere(
            float Radius,
            int Lines = 10,
            int ElemsPerDiv = 10,
            GLColor color = GLColor()
            )
        : GLObject("Wire Sphere"),
          m_nLines(Lines),
          m_nElemsPerDiv(ElemsPerDiv),
          m_fRadius(Radius),
          m_Color(color)
    {
        initSphere();
    }

    void initSphere( void )
    {
        float step = M_PI / (m_nLines*m_nElemsPerDiv);

        m_vVerts.clear();
        m_vVerts.push_back(0);
        m_vVerts.push_back(0);
        m_vVerts.push_back(m_fRadius);

         for (float theta = step; theta < M_PI; theta += step) {
             float z = m_fRadius*cos(theta);
             float r = m_fRadius*sin(theta);
             m_vVerts.push_back(r);
             m_vVerts.push_back(0);
             m_vVerts.push_back(z);

             if (((int) ((theta + step) / step)) % m_nElemsPerDiv == 0) {
                 for (float phi = 0; phi <= 2*M_PI + step; phi += step) {
                     m_vVerts.push_back(r*cos(phi));
                     m_vVerts.push_back(r*sin(phi));
                     m_vVerts.push_back(z);
                 }
             }
         }

         m_vVerts.push_back(0);
         m_vVerts.push_back(0);
         m_vVerts.push_back(-m_fRadius);

         for (float phi = 0; phi <=M_PI; phi += m_nElemsPerDiv*step) {
             for (float theta = -M_PI; theta <= M_PI; theta += step) {
                 m_vVerts.push_back(m_fRadius*sin(theta)*cos(phi));
                 m_vVerts.push_back(m_fRadius*sin(theta)*sin(phi));
                 m_vVerts.push_back(m_fRadius*cos(theta));
             }
         }

         for (float theta = -M_PI; theta <= 0; theta += step) {
             m_vVerts.push_back(m_fRadius*sin(theta));
             m_vVerts.push_back(0);
             m_vVerts.push_back(m_fRadius*cos(theta));
         }
    }

    void drawSphere( void )
    {
        m_Color.Apply();
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, m_vVerts.data());
        glDrawArrays(GL_LINE_LOOP, 0, m_vVerts.size() / 3);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    void SetColor(GLColor color)
    {
        m_Color = color;
    }

    GLColor GetColor( void )
    {
        return m_Color;
    }

    void DrawCanonicalObject()
    {
        drawSphere();
    }

private:
    int                           m_nLines;
    int                           m_nElemsPerDiv;
    float                         m_fRadius;
    std::vector< float >          m_vVerts;
    GLColor                       m_Color;
};

}
#endif // GLWIRESPHERE_H
