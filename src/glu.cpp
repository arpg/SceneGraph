#include <SceneGraph/GLHelpers.h>
#include <SceneGraph/simple_math.h>

namespace SceneGraph
{

  void sgLookAt(float ex, float ey, float ez,
      float lx, float ly, float lz, float ux,
      float uy, float uz
      )
  {
    float mat[16];
    const float up_des[3] = {ux,uy,uz};
    float right[3], up[3];
    float forward[] = {lx - ex, ly - ey, lz - ez};
    Normalize<3>(forward);

    // create 
    CrossProduct(right,forward,up_des);
    CrossProduct(up,forward,right);

    // Normalize x, y
    const float lenright = Length<3>(right);
    const float lenup = Length<3>(up);

    if( lenright > 0 && lenup > 0) {
      for(size_t r = 0; r < 3; ++r ) {
        right[r] /= lenright;
        up[r] /= lenup;
      }
#define M(row,col)  mat[col*4+row]
      M(0,0) = right[0];
      M(0,1) = right[1];
      M(0,2) = right[2];
      M(1,0) = -up[0];
      M(1,1) = -up[1];
      M(1,2) = -up[2];
      M(2,0) = -forward[0];
      M(2,1) = -forward[1];
      M(2,2) = -forward[2];
      M(3,0) = 0.0;
      M(3,1) = 0.0;
      M(3,2) = 0.0;
      M(0,3) = -(M(0,0)*ex + M(0,1)*ey + M(0,2)*ez);
      M(1,3) = -(M(1,0)*ex + M(1,1)*ey + M(1,2)*ez);
      M(2,3) = -(M(2,0)*ex + M(2,1)*ey + M(2,2)*ez);
      M(3,3) = 1.0;
#undef M
    }else{
      printf("here\n");
     // "'Look' and 'up' vectors cannot be parallel when calling ModelViewLookAt.");
    }
    glLoadMatrixf( mat );
  }

}

