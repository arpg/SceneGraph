#ifndef _CVAR_HELPERS_
#define _CVAR_HELPERS_

#include <CVars/CVarVectorIO.h>

#include <Eigen/Core>

namespace Eigen
{
    typedef Matrix<double,5,1> Vector5d;
}

////////////////////////////////////////////////////////////////////////////
// Overloading Eigen for CVars
namespace CVarUtils
{

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<( std::ostream& Stream, Eigen::Vector3d& Mat )
    {
        unsigned int nRows = Mat.rows();
        unsigned int nCols = Mat.cols();

        Stream << "[ ";

        for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
            for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
                Stream << Mat(ii, jj);
                Stream << ", ";
            }
            Stream << Mat(ii, nCols-1);
            Stream << "; ";
        }
        for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
            Stream << Mat(nRows-1, jj);
            Stream << ", ";
        }
        Stream << Mat(nRows-1, nCols-1);
        Stream << " ]";

        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::istream& operator>>( std::istream& Stream, Eigen::Vector3d& Mat )
    {

        unsigned int nRows = Mat.rows();
        unsigned int nCols = Mat.cols();
        char str[256];

        Stream.getline(str, 255, '[');
        if( Stream.gcount() > 1 ) {
            return Stream;
        }
        for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
            for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
                Stream.getline(str, 255, ',');
                Mat(ii, jj) = std::strtod(str, NULL);
            }
            Stream.getline(str, 255, ';');
            Mat(ii, nCols-1) = std::strtod(str, NULL);
        }
        for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
            Stream.getline(str, 255, ',');
            Mat(nRows-1, jj) = std::strtod(str, NULL);
        }
        Stream.getline(str, 255, ']');
        Mat(nRows-1, nCols-1) = std::strtod(str, NULL);
        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<(std::ostream& Stream, std::vector<Eigen::Vector3d>& vEigen ) {
        if( vEigen.size() == 0 ) {
            Stream << "[ ]";
            return Stream;
        }

        Stream << "[ " << vEigen[0];
        for( size_t i=1; i<vEigen.size(); i++ ) {
            Stream << " " << vEigen[i];
        }
        Stream << " ]";

        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::istream& operator>>(std::istream& Stream, std::vector<Eigen::Vector3d>& vEigen ) {

        std::stringstream sBuf;
        Eigen::Vector3d Mat;

        vEigen.clear();

        char str[256];

        // strip first bracket
        Stream.getline(str, 255, '[');
        if( Stream.gcount() > 1 ) {
            return Stream;
        }

        // get first element
        Stream.getline(str, 255, '[');
        Stream.getline(str, 255, ']');

        while( Stream.gcount() > 1 ) {
            sBuf << "[";
            sBuf << str;
            sBuf << "]";
            sBuf >> Mat;
            vEigen.push_back(Mat);

            // get next element
            Stream.getline(str, 255, '[');
            Stream.getline(str, 255, ']');
        }
        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<( std::ostream& Stream, Eigen::Vector5d& Mat )
    {
        unsigned int nRows = Mat.rows();
        unsigned int nCols = Mat.cols();

        Stream << "[ ";

        for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
            for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
                Stream << Mat(ii, jj);
                Stream << ", ";
            }
            Stream << Mat(ii, nCols-1);
            Stream << "; ";
        }
        for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
            Stream << Mat(nRows-1, jj);
            Stream << ", ";
        }
        Stream << Mat(nRows-1, nCols-1);
        Stream << " ]";

        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::istream& operator>>( std::istream& Stream, Eigen::Vector5d& Mat )
    {

        unsigned int nRows = Mat.rows();
        unsigned int nCols = Mat.cols();
        char str[256];

        Stream.getline(str, 255, '[');
        if( Stream.gcount() > 1 ) {
            return Stream;
        }
        for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
            for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
                Stream.getline(str, 255, ',');
                Mat(ii, jj) = std::strtod(str, NULL);
            }
            Stream.getline(str, 255, ';');
            Mat(ii, nCols-1) = std::strtod(str, NULL);
        }
        for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
            Stream.getline(str, 255, ',');
            Mat(nRows-1, jj) = std::strtod(str, NULL);
        }
        Stream.getline(str, 255, ']');
        Mat(nRows-1, nCols-1) = std::strtod(str, NULL);
        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<(std::ostream& Stream, std::vector<Eigen::Vector5d>& vEigen ) {
        if( vEigen.size() == 0 ) {
            Stream << "[ ]";
            return Stream;
        }

        Stream << "[ " << vEigen[0];
        for( size_t i=1; i<vEigen.size(); i++ ) {
            Stream << " " << vEigen[i];
        }
        Stream << " ]";

        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::istream& operator>>(std::istream& Stream, std::vector<Eigen::Vector5d>& vEigen ) {

        std::stringstream sBuf;
        Eigen::Vector5d Mat;

        vEigen.clear();

        char str[256];

        // strip first bracket
        Stream.getline(str, 255, '[');
        if( Stream.gcount() > 1 ) {
            return Stream;
        }

        // get first element
        Stream.getline(str, 255, '[');
        Stream.getline(str, 255, ']');

        while( Stream.gcount() > 1 ) {
            sBuf << "[";
            sBuf << str;
            sBuf << "]";
            sBuf >> Mat;
            vEigen.push_back(Mat);

            // get next element
            Stream.getline(str, 255, '[');
            Stream.getline(str, 255, ']');
        }
        return Stream;
    }


    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<( std::ostream& Stream, Eigen::Vector6d& Mat )
    {
        unsigned int nRows = Mat.rows();
        unsigned int nCols = Mat.cols();

        Stream << "[ ";

        for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
            for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
                Stream << Mat(ii, jj);
                Stream << ", ";
            }
            Stream << Mat(ii, nCols-1);
            Stream << "; ";
        }
        for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
            Stream << Mat(nRows-1, jj);
            Stream << ", ";
        }
        Stream << Mat(nRows-1, nCols-1);
        Stream << " ]";

        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::istream& operator>>( std::istream& Stream, Eigen::Vector6d& Mat )
    {

        unsigned int nRows = Mat.rows();
        unsigned int nCols = Mat.cols();
        char str[256];

        Stream.getline(str, 255, '[');
        if( Stream.gcount() > 1 ) {
            return Stream;
        }
        for( unsigned int ii = 0; ii < nRows-1; ii++ ) {
            for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
                Stream.getline(str, 255, ',');
                Mat(ii, jj) = std::strtod(str, NULL);
            }
            Stream.getline(str, 255, ';');
            Mat(ii, nCols-1) = std::strtod(str, NULL);
        }
        for( unsigned int jj = 0; jj < nCols-1; jj++ ) {
            Stream.getline(str, 255, ',');
            Mat(nRows-1, jj) = std::strtod(str, NULL);
        }
        Stream.getline(str, 255, ']');
        Mat(nRows-1, nCols-1) = std::strtod(str, NULL);
        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::ostream& operator<<(std::ostream& Stream, std::vector<Eigen::Vector6d>& vEigen ) {
        if( vEigen.size() == 0 ) {
            Stream << "[ ]";
            return Stream;
        }

        Stream << "[ " << vEigen[0];
        for( size_t i=1; i<vEigen.size(); i++ ) {
            Stream << " " << vEigen[i];
        }
        Stream << " ]";

        return Stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    inline std::istream& operator>>(std::istream& Stream, std::vector<Eigen::Vector6d>& vEigen ) {

        std::stringstream sBuf;
        Eigen::Vector6d Mat;

        vEigen.clear();

        char str[256];

        // strip first bracket
        Stream.getline(str, 255, '[');
        if( Stream.gcount() > 1 ) {
            return Stream;
        }

        // get first element
        Stream.getline(str, 255, '[');
        Stream.getline(str, 255, ']');

        while( Stream.gcount() > 1 ) {
            sBuf << "[";
            sBuf << str;
            sBuf << "]";
            sBuf >> Mat;
            vEigen.push_back(Mat);

            // get next element
            Stream.getline(str, 255, '[');
            Stream.getline(str, 255, ']');
        }
        return Stream;
    }
}

#endif

