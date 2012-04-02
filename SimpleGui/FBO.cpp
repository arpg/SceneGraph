#include "FBO.h"

FBO* FBO::m_pInstance = NULL;

FBO* FBO::Instance()
{
  if (!m_pInstance) {
    m_pInstance = new FBO;
  }
  
  return m_pInstance;
}
