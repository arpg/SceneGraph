#ifndef PEAKSHEIGHTMAP_H
#define PEAKSHEIGHTMAP_H

#include "HeightMap.h"
#include <math.h>
#include <Eigen/Core>

class PeaksHeightMap: public HeightMap
{
public:
    PeaksHeightMap()
    {
    }

    double GetHeight( double x, double y )
    {
        double dScale = 0.1;
        x *= dScale;
        y *= dScale;
        double z =  3*pow((1-x),2)*exp(-(pow(x,2)) - pow((y+1),2))
                - 10*(x/5 - pow(x,3) - pow(y,5))*exp(-pow(x,2)-pow(y,2))
                - 1/3*exp(-pow((x+1),2) - pow(y,2));
        return z;
    }

    float* GetNormal( float x, float y, float dx, float dy )
    {
        float zl = GetHeight( x-dx, y );
        Eigen::Vector3f l; 
        l << x-dx,y,zl;

        float zr = GetHeight( x+dx, y );
        Eigen::Vector3f r; 
        r << x+dx,y,zr;

        float zu = GetHeight( x, y-dy );
        Eigen::Vector3f u; 
        u << x,y-dy,zu;

        float zd = GetHeight( x, y+dy );
        Eigen::Vector3f d; 
        d << x,y+dy,zd;

        Eigen::Vector3f gx = (r-l);
        Eigen::Vector3f gy = (d-u);
        m_n = gy.cross( gx ); 
        m_n = m_n / m_n.norm();

        return m_n.data();
    }
    
    Eigen::Vector3f m_n;

};

#endif // PEAKSHEIGHTMAP_H
