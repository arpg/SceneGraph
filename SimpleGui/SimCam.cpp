#include "SimCam.h"

const int PBO_COUNT = 4; // 2 for Depth, 2 for RGB
GLuint pboIds[PBO_COUNT];

static int m_nUsedAttachments = 0;
static int m_nIndex = 0;

void GLSimCam::PboInit() {
  static int DATA_SIZE = m_nSensorWidth * m_nSensorHeight * 3;
  // Create pixel buffer objects
  glGenBuffersARB(PBO_COUNT, pboIds);
  // TODO: cleanup when done and support resizing
  
  // 2 buffers for RGB
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[0]);
  glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_READ_ARB); // 3 bytes
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[1]);
  glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_READ_ARB); // 3 bytes

  // 2 buffers for depth
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[2]);
  glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, m_nSensorWidth * m_nSensorHeight * 4, 0, GL_STREAM_READ_ARB); // 4 bytes
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[3]);
  glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, m_nSensorWidth * m_nSensorHeight * 4, 0, GL_STREAM_READ_ARB); // 4 bytes


  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
  
  // allocate buffer to store frame
  rgbBuffer = new GLubyte[DATA_SIZE];
  depthBuffer = new GLfloat[m_nSensorWidth * m_nSensorHeight];
}

void GLSimCam::PboReadRGB() {
  static int index = 0;
  static int offset = 0;
  int nextIndex = 0;

  index = (index + 1) % 2;
  nextIndex = (index + 1) % 2;

  // Copy pixels from framebuffer to PBO
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[offset + index]);
  glReadPixels(0, 0, m_nSensorWidth, m_nSensorHeight, GL_RGB, GL_UNSIGNED_BYTE, 0);

  // map the PBO that contains the framebuffer pixels
  // Read from the next buffer since the other is likely being written to still
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[nextIndex]);
  GLubyte* src = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);

  if (src) {
    memcpy(rgbBuffer, src, m_nSensorWidth * m_nSensorHeight * 3);
    glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
  }

  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
}

void GLSimCam::PboReadDepth() {
  static int index = 0;
  static int offset = 2;
  int nextIndex = 0;

  index = (index + 1) % 2;
  nextIndex = (index + 1) % 2;

  // Copy pixels from framebuffer to PBO
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[offset + index]);
  glReadPixels(0, 0, m_nSensorWidth, m_nSensorHeight, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

  // map the PBO that contains the framebuffer pixels
  // Read from the next buffer since the other is likely being written to still
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[nextIndex]);
  GLfloat* src = (GLfloat*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);

  if (src) {
    memcpy(depthBuffer, src, m_nSensorHeight * m_nSensorWidth * 4);
    glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
  }
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);

  CheckForGLErrors(); // TODO: Remove
}

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

