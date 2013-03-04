#pragma once

#include <pangolin/gl.h>
#include <SceneGraph/SceneGraph.h>
#include <SceneGraph/GLLight.h>

namespace SceneGraph
{

class GLShadowLight : public GLLight
{
public:
    GLShadowLight(double x=0, double y=0, double z=0, int shadowBufferWidth = 1024, int shadowBufferHeight = 1024, bool bStatic = false)
        : GLLight(x,y,z),
          fb_img(shadowBufferWidth,shadowBufferHeight),
          fb_depth(shadowBufferWidth,shadowBufferHeight,GL_DEPTH_COMPONENT),
          framebuffer(fb_img,fb_depth),
          depth_tex(shadowBufferWidth,shadowBufferHeight,GL_DEPTH_COMPONENT,true,1,GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE),
          m_bStatic(bStatic), m_bShadowsComputed(false), m_bShadowsEnabled(true)
    {
        // Setup border so we don't cast shadows beyond shadow map.
        depth_tex.Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat color[4] = {1,1,1,1};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
        CheckForGLErrors();
    }

    const pangolin::OpenGlRenderState& GetRenderState() const
    {
        return stacks_light;
    }

    void AddShadowCaster(GLObject* obj)
    {
        shadow_casters.push_back(obj);
    }

    void AddShadowReceiver(GLObject* obj)
    {
        shadow_receivers.push_back(obj);
    }

    void AddShadowCasterAndReceiver(GLObject* obj)
    {
        shadow_casters.push_back(obj);
        shadow_receivers.push_back(obj);
    }


    virtual void PreRender(GLSceneGraph& /*scene*/) {
        if(m_bShadowsEnabled){
            SetupLight();
            ComputeShadows();
        }

        EnableLight();
        GLfloat ambientLight [] = {0.2*0.5, 0.2*0.5, 0.2*0.5, 1.0};
        GLfloat diffuseLight [] = {0.4*0.5, 0.4*0.5, 0.4*0.5, 1.0};
        glLightfv(m_nLigthId, GL_AMBIENT, ambientLight);
        glLightfv(m_nLigthId, GL_DIFFUSE, diffuseLight);
    }

    virtual void PostRender(GLSceneGraph& /*scene*/) {
        if(m_bShadowsEnabled){
            GLfloat lowAmbient[4] = {0.1f*0.5, 0.1f*0.5, 0.1f*0.5, 1.0f};
            GLfloat lowDiffuse[4] = {0.2f*0.5, 0.2f*0.5, 0.2f*0.5, 1.0f};
            glLightfv(m_nLigthId, GL_AMBIENT, lowAmbient);
            glLightfv(m_nLigthId, GL_DIFFUSE, lowDiffuse);
            DrawShadows();

            GLfloat ambientLight [] = {0.2*0.5, 0.2*0.5, 0.2*0.5, 1.0};
            GLfloat diffuseLight [] = {0.4*0.5, 0.4*0.5, 0.4*0.5, 1.0};
            glLightfv(m_nLigthId, GL_AMBIENT, ambientLight);
            glLightfv(m_nLigthId, GL_DIFFUSE, diffuseLight);
        }
        //DisableLight();
    }

    void ComputeShadows()
    {
        if(m_bShadowsComputed == false || m_bStatic == false){
            // Save ModelView and Projection
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glMatrixMode(GL_MODELVIEW);

            // Put depthmap from light into depth_tex
            glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT);
            framebuffer.Bind();
            glShadeModel(GL_FLAT);
            glDisable(GL_LIGHTING);
            glDisable(GL_COLOR_MATERIAL);
            glDisable(GL_NORMALIZE);
            glColorMask(0, 0, 0, 0);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.0, 4.0);

            glViewport(0,0,fb_img.width,fb_img.height);
            glScissor(0,0,fb_img.width,fb_img.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            stacks_light.Apply();
            for(unsigned int i=0; i<shadow_casters.size(); ++i ) {
                shadow_casters[i]->DrawObjectAndChildren(eRenderNoPrePostHooks);
            }

            depth_tex.Bind();
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, fb_img.width, fb_img.height, 0);

            framebuffer.Unbind();
            glPopAttrib();
            CheckForGLErrors();

            // Restore ModelView and Projection matrices
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            m_bShadowsComputed = true;
        }
    }

    void DrawCanonicalObject()
    {
        GLLight::DrawCanonicalObject();

        glPopMatrix();

        glColor4f( 1,0,0,1 );
        DrawAABB(m_bboxcasters);

        glPushMatrix();
    }

    void DrawAABB(AxisAlignedBoundingBox& aabb)
    {
        glPushAttrib(GL_ENABLE_BIT);

        glDisable(GL_LIGHTING);
        glDisable( GL_DEPTH_TEST );
        glBegin( GL_LINE_STRIP );
        Eigen::Vector3d bmin = aabb.Min();
        Eigen::Vector3d bmax = aabb.Max();

        glVertex3f(bmin(0), bmax(1), bmax(2));
        glVertex3f(bmax(0), bmax(1), bmax(2));
        glVertex3f(bmax(0), bmin(1), bmax(2));
        glVertex3f(bmin(0), bmin(1), bmax(2));


        glVertex3f(bmax(0), bmax(1), bmin(2));
        glVertex3f(bmin(0), bmax(1), bmin(2));
        glVertex3f(bmin(0), bmin(1), bmin(2));
        glVertex3f(bmax(0), bmin(1), bmin(2));


        glVertex3f(bmin(0), bmax(1), bmin(2));
        glVertex3f(bmax(0), bmax(1), bmin(2));
        glVertex3f(bmax(0), bmax(1), bmax(2));
        glVertex3f(bmin(0), bmax(1), bmax(2));


        glVertex3f(bmax(0), bmin(1), bmin(2));
        glVertex3f(bmin(0), bmin(1), bmin(2));
        glVertex3f(bmin(0), bmin(1), bmax(2));
        glVertex3f(bmax(0), bmin(1), bmax(2));


        glVertex3f(bmax(0), bmax(1), bmax(2));
        glVertex3f(bmax(0), bmax(1), bmin(2));
        glVertex3f(bmax(0), bmin(1), bmin(2));
        glVertex3f(bmax(0), bmin(1), bmax(2));


        glVertex3f(bmin(0), bmax(1), bmin(2));
        glVertex3f(bmin(0), bmax(1), bmax(2));
        glVertex3f(bmin(0), bmin(1), bmax(2));
        glVertex3f(bmin(0), bmin(1), bmin(2));

        glEnd();
        glPopAttrib();
    }

    void SetShadowsEnabled(bool bEnabled){
        m_bShadowsEnabled = bEnabled;
    }

    void DrawShadows()
    {
        // TODO: Probably use shader
        // e.g. http://fabiensanglard.net/shadowmapping/index.php

        glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);

        glActiveTextureARB(GL_TEXTURE1_ARB);
        stacks_light.EnableProjectiveTexturing();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        depth_tex.Bind();

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GEQUAL );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, 0.5f);

        glAlphaFunc(GL_GREATER, 0.9f);
        glEnable(GL_ALPHA_TEST);

        glEnable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);


        for(unsigned int i=0; i<shadow_receivers.size(); ++i ) {
            shadow_receivers[i]->DrawObjectAndChildren(eRenderNoPrePostHooks);
        }

        glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);

        glPopAttrib();
        CheckForGLErrors();
    }

protected:
    inline AxisAlignedBoundingBox ObjectBounds(std::vector<GLObject*> objs)
    {
        AxisAlignedBoundingBox bbox;
        for(std::vector<GLObject*>::const_iterator i=objs.begin(); i!= objs.end(); ++i) {
            bbox.Insert((*i)->GetPose4x4_po(), (*i)->ObjectAndChildrenBounds() );
        }
        return bbox;
    }

    inline void SetupLight()
    {
        // TODO: Ideally we would work out a tight fit around shadow_casters only
        m_bboxcasters = ObjectBounds(shadow_casters);
        m_bboxreceivers = ObjectBounds(shadow_receivers);
        m_bbox_both.Clear();

        m_bbox_both.Insert(m_bboxcasters);
        m_bbox_both.Insert(m_bboxreceivers);

        const Eigen::Vector3d lpos = this->GetPose().head<3>();
        const Eigen::Vector3d center = m_bboxcasters.Center();
        const double dist = (center - lpos).norm();
        const double bothrad = m_bbox_both.Size().norm() / 2.0f;

        const double f = fb_img.width * dist / (m_bboxcasters.Size().norm());

        // Point light at scene
        stacks_light.SetProjectionMatrix(pangolin::ProjectionMatrix(fb_img.width,fb_img.height, f, f, fb_img.width/2.0f,fb_img.height/2.0f, std::max(dist-bothrad,0.1), dist+bothrad));
        stacks_light.SetModelViewMatrix(pangolin::ModelViewLookAt(lpos(0), lpos(1), lpos(2), center(0), center(1), center(2), pangolin::AxisNegZ));
    }

    AxisAlignedBoundingBox m_bboxcasters;
    AxisAlignedBoundingBox m_bboxreceivers;
    AxisAlignedBoundingBox m_bbox_both;

    pangolin::GlTexture fb_img;
    pangolin::GlRenderBuffer fb_depth;
    pangolin::GlFramebuffer framebuffer;
    pangolin::GlTexture depth_tex;
    pangolin::OpenGlRenderState stacks_light;

    std::vector<GLObject*> shadow_casters;
    std::vector<GLObject*> shadow_receivers;

    bool m_bStatic;
    bool m_bShadowsComputed;
    bool m_bShadowsEnabled;
};

}
