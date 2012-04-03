#include "SimCam.h"

static int m_nUsedAttachments = 0;
static int m_nIndex = 0;

int GLSimCam::getNextIndex() {
  int nextIndex = m_nIndex;
  m_nIndex++;
  return nextIndex;
}

// Looks like the limit is 16.
int GLSimCam::getNextAttachment()  {
  unsigned int nextAttachment = -1;
  if (m_nUsedAttachments < GL_MAX_COLOR_ATTACHMENTS) { // Not sure if this check works
    switch (m_nUsedAttachments) {
    case 0:
      nextAttachment = GL_COLOR_ATTACHMENT0_EXT;
      break;
    case 1:
      nextAttachment = GL_COLOR_ATTACHMENT1_EXT;
      break;
    case 2:
      nextAttachment = GL_COLOR_ATTACHMENT2_EXT;
      break;
    case 3:
      nextAttachment = GL_COLOR_ATTACHMENT3_EXT;
      break;
    case 4:
      nextAttachment = GL_COLOR_ATTACHMENT4_EXT;
      break;
    case 5:
      nextAttachment = GL_COLOR_ATTACHMENT5_EXT;
      break;
    case 6:
      nextAttachment = GL_COLOR_ATTACHMENT6_EXT;
      break;
    case 7:
      nextAttachment = GL_COLOR_ATTACHMENT7_EXT;
      break;
    case 8:
      nextAttachment = GL_COLOR_ATTACHMENT8_EXT;
      break;
    case 9:
      nextAttachment = GL_COLOR_ATTACHMENT9_EXT;
      break;
    case 10:
      nextAttachment = GL_COLOR_ATTACHMENT10_EXT;
      break;
    case 11:
      nextAttachment = GL_COLOR_ATTACHMENT11_EXT;
      break;
    case 12:
      nextAttachment = GL_COLOR_ATTACHMENT12_EXT;
      break;
    case 13:
      nextAttachment = GL_COLOR_ATTACHMENT13_EXT;
      break;
    case 14:
      nextAttachment = GL_COLOR_ATTACHMENT14_EXT;
      break;
    case 15:
      nextAttachment = GL_COLOR_ATTACHMENT15_EXT;
      break;
    }
    m_nUsedAttachments++;
  } else {
    printf("ERROR: Out of color attachments.\n");
  }
  
  return nextAttachment;
}

