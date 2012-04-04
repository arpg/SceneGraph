#include "SimCam.h"

const int PBO_COUNT = 4; // 2 for Depth, 2 for RGB
GLuint pboIds[PBO_COUNT];

static int m_nUsedAttachments = 0;
static int m_nIndex = 0;

void GLSimCam::PboInit() {
  static int DATA_SIZE = m_nSensorWidth * m_nSensorHeight * 4;
  // Create pixel buffer objects
  glGenBuffersARB(PBO_COUNT, pboIds);
  // TODO: cleanup when done and support resizing
  for (int i = 0; i < PBO_COUNT; i++) {
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[i]);
    glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, DATA_SIZE, 0, GL_STREAM_READ_ARB); // 4 bytes
  }
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
  
  // allocate buffer to store frame
  rgbBuffer = new GLubyte[DATA_SIZE];
  //depthBuffer
}

void GLSimCam::copyBytes(unsigned char* src, int width, int height, unsigned char* dst) {
  if (!src || !dst) {
    return;
  }

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      *dst = 255; // Make everything pink so we can tell it is working
      ++src;
      ++dst;

      for (int h = 0; h < 3; ++h) { 
	*dst = *src;
	++src;
	++dst;
      }
    }
  }
}

void GLSimCam::PboReadRGB() {
  static int index = 0;
  static int offset = 0;
  int nextIndex = 0;

  index = (index + 1) % 2;
  nextIndex = (index + 1) % 2;

  glReadBuffer(GL_FRONT);

  // Copy pixels from framebuffer to PBO
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[offset + index]);
  glReadPixels(0, 0, m_nSensorWidth, m_nSensorHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  // map the PBO that contains the framebuffer pixels
  // Read from the next buffer since the other is likely being written to still
  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboIds[nextIndex]);
  GLubyte* src = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
  if (src) {
    copyBytes(src, m_nSensorWidth, m_nSensorHeight, rgbBuffer);
    glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
  }

  glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
}

void GLSimCam::PboReadDepth() {

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

