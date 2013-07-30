#pragma once

#include <Eigen/Eigen>

namespace SceneGraph {

template<typename T>
class LineSegment
{
public:
    // Uninitialised
    LineSegment()
    {        
    }
    
    template <typename Derived1, typename Derived2>
    LineSegment(const Eigen::MatrixBase<Derived1>& Pa, 
                const Eigen::MatrixBase<Derived2>& Pb)
        : mPa(Pa), mPbmPa(Pb - mPa)
    {
    }
    
    Eigen::Matrix<T,3,1> operator()(T lambda)
    {
        return mPa + lambda * mPbmPa;
    }
    
    bool InSegment(T lambda)
    {
        return 0 <= lambda && lambda <= 1.0;
    }
    
    Eigen::Matrix<T,3,1>& P() { return mPa; }
    const Eigen::Matrix<T,3,1>& P() const { return mPa; }

    Eigen::Matrix<T,3,1>& Dir() { return mPbmPa; }
    const Eigen::Matrix<T,3,1>& Dir() const { return mPbmPa; }

    Eigen::Matrix<T,3,1> Pb() const { 
        return mPa + mPbmPa; 
    }
    
protected:
    Eigen::Matrix<T,3,1> mPa;
    Eigen::Matrix<T,3,1> mPbmPa;
};

template <typename Derived, typename T>
LineSegment<T> operator*(const Eigen::MatrixBase<Derived>& T_ba, const LineSegment<T>& ls_a)
{
    LineSegment<T> ls_b;
    ls_b.P() = T_ba.template block<3,3>(0,0) * ls_a.P() + T_ba.template block<3,1>(0,3);
    ls_b.Dir() = T_ba.template block<3,3>(0,0) * ls_a.Dir();
    return ls_b;
}

}
