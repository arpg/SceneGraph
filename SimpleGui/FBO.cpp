#include "FBO.h"

FBO* FBO::m_pInstance = NULL;

FBO* FBO::Instance()
{
  printf("FBO: Setting up instance.\n");
  if (!m_pInstance) {
    m_pInstance = new FBO;
    printf("FBO: new instance!.\n");
  }
  
  printf("FBO: returning instance.\n");
  return m_pInstance;
}
