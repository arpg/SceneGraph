#ifndef GLWIDGETPANEL_H
#define GLWIDGETPANEL_H

#include "pangolin/pangolin.h"
#include "SceneGraph/SceneGraph.h"
#include "nvGlutWidgets.h"

using namespace SceneGraph;
using namespace pangolin;


class GLWidgetView : public pangolin::View, public pangolin::Handler
{
public:

    GLWidgetView() :
        m_RenderState2d(pangolin::ProjectionMatrixOrthographic(0,1,0,1,0.0,100))
    {
        SetHandler(this);
    }

    void Init(const Attach bottom,const Attach top,const Attach left, const Attach right,
              std::function<void(nv::GlutUIContext&,nv::Rect&)> func) {
        SetBounds(bottom,top,left,right);
        m_DrawFunc = func;
    }

    int DoNumericLineEdit(nv::Rect& rect, const char *label, int* nNumber)
    {
        m_Ui.doLabel(rect,label);
        int charsReturned;
        char sNumber[10];
        sprintf(sNumber,"%d",*nNumber);
        m_Ui.doLineEdit(rect,sNumber,10,&charsReturned);
        *nNumber = atoi(sNumber);
        return charsReturned;
    }

    void Render(){
        ActivateAndScissor(m_RenderState2d);

        glColor4f(0.0,0.0,0.0,0.5);
        pangolin::glDrawRect(-1.0f,-1.0f,1.0f,1.0f);

        m_Rect = nv::Rect(0,0,m_nWidth,0);
        m_Ui.begin();
        m_DrawFunc(m_Ui,m_Rect);
        m_Ui.end();
    }

    void Resize(const Viewport &parent)
    {
        View::Resize(parent);
        m_nWidth = v.w;
        m_nHeight = v.h;
        m_Ui.init(m_nWidth, m_nHeight);
    }

    template<typename T>
    T GetVar(std::string name){
        return (T)m_mVars[name];
    }

    GLWidgetView& SetVar(std::string name, void *ptr)
    {
        m_mVars[name] = ptr;
        return *this;
    }

    nv::GlutUIContext* GetUIContext(){ return &m_Ui; }

    void Mouse(pangolin::View& view, pangolin::MouseButton button, int x, int y, bool pressed, int button_state)
    {
        //convert pangolin button to GLUT button
        int glutButton = (int)button;
        int log2 = 0;
        while (glutButton >>= 1) ++log2;
        m_Ui.mouse(log2,(int)!pressed,x,view.v.h - y);
    }

    void MouseMotion(pangolin::View& view, int x, int y, int button_state)
    {
        m_Ui.mouseMotion(x,view.v.h - y);
    }

    void Keyboard(View& view, unsigned char key, int x, int y, bool pressed)
    {
        m_Ui.keyboard(key,x,y);
    }

//    void Resize(const Viewport &parent)
//    {
//        m_Ui.reshape(parent.w, parent.h);
//    }

protected:
    int m_nWidth;
    int m_nHeight;
    nv::Rect m_Rect;
    nv::GlutUIContext m_Ui;
    std::map<std::string,void *> m_mVars;
  std::function<void(nv::GlutUIContext&,nv::Rect&)> m_DrawFunc;
    pangolin::OpenGlRenderState m_RenderState2d;

};

#endif // GLWIDGETPANEL_H
