#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

class HeightMap
{
public:
    HeightMap()
    {
    }

    virtual double GetHeight(double x, double y) = 0;
    virtual float* GetNormal( float x, float y, float dx, float dy ) = 0;
};

#endif // HEIGHTMAP_H
