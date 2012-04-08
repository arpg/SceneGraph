#ifndef _GL_POINT_CLOUD_H_
#define _GL_POINT_CLOUD_H_

class GLPointCloud : public GLObject
{

    public:

        GLPointCloud()
        {
            m_nDisplayList = -1;
            m_nOldRangeDataSize = 0;
        }

        /// will recompile list if users add more data
        void draw()
        {
            if( m_nDisplayList == -1 ){
                m_nDisplayList = glGenLists(1);
                CompileDisplyList();
            }
            if( m_vRangeData.size() != m_nOldRangeDataSize ){
                m_nOldRangeDataSize = m_vRangeData.size();
                CompileDisplyList();
            }

            // TODO re-compile only when data changes
            CompileDisplyList();

            glCallList(m_nDisplayList);
        }
       
        // will compile new drawlist
        void CompileDisplyList()
        {
            glNewList( m_nDisplayList, GL_COMPILE );
            glPointSize( 2 );
            glEnable( GL_COLOR_MATERIAL );
            glColor3f( 1,0,1 );
            glBegin( GL_POINTS );
            for( size_t ii = 0; ii < m_vRangeData.size(); ii+=3 ){
                glVertex3fv( &m_vRangeData[ii] );
            }
            glEnd();
            glPointSize( 1 );
            glEndList();
        }

        // return ref to our range data
        std::vector<float>& RangeDataRef()
        {
            return m_vRangeData;
        }

    private:
        GLint                  m_nDisplayList;
        std::vector<float>     m_vRangeData;
        unsigned int           m_nOldRangeDataSize;
        Eigen::Matrix4d        m_dPose;
};

#endif
