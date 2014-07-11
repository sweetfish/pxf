#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/QuadBatch.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Graphics/OpenGL/VertexBufferGL2.h>
#include <Pxf/Graphics/OpenGL/TextureGL2.h>
#include <Pxf/Graphics/OpenGL/RenderTargetGL2.h>
#include <Pxf/Graphics/OpenGL/QuadBatchGL2.h>
#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Graphics/OpenGL/OpenGL.h>
#include <Pxf/Graphics/OpenGL/OpenGLUtils.h>


#define LOCAL_MSG "DeviceGL2"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

DeviceGL2::DeviceGL2()
{
	// Initialize GLFW
	if (glfwInit() != GL_TRUE)
	{
		Message(LOCAL_MSG, "Could not initialize GLFW!");
		return;
	}

	Message(LOCAL_MSG, "Device initiated.");

}

DeviceGL2::~DeviceGL2()
{

	// Close any open window.
	CloseWindow();


	// Terminate GLFW
	glfwTerminate();
}

Window* DeviceGL2::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowGL2(_pWindowSpecs);
	m_Window->Open();
	return m_Window;
}

void DeviceGL2::CloseWindow()
{
	if (m_Window && m_Window->IsOpen())
	{
		m_Window->Close();
		delete m_Window;
	}
}

void DeviceGL2::GetSize(int *_w, int *_h)
{
    (*_w) = m_Window->GetWidth();
    (*_h) = m_Window->GetHeight();
}

void DeviceGL2::SetViewport(int _x, int _y, int _w, int _h)
{
	glViewport(_x, _y, _w, _h);
}

void DeviceGL2::SetProjection(Math::Mat4 *_matrix)
{
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf((GLfloat*)(_matrix->m));
	glMatrixMode (GL_MODELVIEW);
}

void DeviceGL2::SwapBuffers()
{
	if (m_Window)
	{
		m_Window->Swap();
	}
}

void DeviceGL2::Translate(Math::Vec3f _translate)
{
	glTranslatef(_translate.x, _translate.y, _translate.z);
}

Texture* DeviceGL2::CreateEmptyTexture(int _Width,int _Height, TextureFormatStorage _Format)
{
	TextureGL2* _Tex = new TextureGL2(this);
	_Tex->LoadData(NULL,_Width,_Height,_Format);

	return _Tex;
}

Texture* DeviceGL2::CreateTexture(const char* _filepath)
{
	TextureGL2* _tex = new TextureGL2(this);
	_tex->Load(_filepath);
	return _tex;
}

Texture* DeviceGL2::CreateTexture(const char* _filepath, bool _autoload)
{
    TextureGL2* _Tex;
    glEnable(GL_TEXTURE_2D);
    if (_autoload)
    {
        _Tex = new TextureGL2(this);
    	_Tex->Load(_filepath);
    } else {
        _Tex = new TextureGL2(_filepath, this);
    }

	return _Tex;
}

Texture* DeviceGL2::CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	TextureGL2* _tex = new TextureGL2(this);
	_tex->LoadData(_datachunk, _width, _height, _channels);
	return _tex;
}

void DeviceGL2::BindTexture(Texture* _texture)
{
    if (_texture == NULL)
        glBindTexture(GL_TEXTURE_2D, 0);
    else
	    glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
}

static GLuint _texture_units_array[16] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4,
										GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9,
										GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14,
										GL_TEXTURE15};
void DeviceGL2::BindTexture(Texture* _texture, unsigned int _texture_unit)
{
	glActiveTextureARB(_texture_units_array[_texture_unit]);
	if (_texture == NULL)
        glBindTexture(GL_TEXTURE_2D, 0);
    else
	    glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
}


QuadBatch* DeviceGL2::CreateQuadBatch(int _maxSize)
{
	return new QuadBatchGL2(this, _maxSize);
}


VertexBuffer* DeviceGL2::CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag)
{
	return new VertexBufferGL2(this, _VertexBufferLocation, _VertexBufferUsageFlag);
}

void DeviceGL2::DestroyVertexBuffer(VertexBuffer* _pVertexBuffer)
{
	if (_pVertexBuffer)
		delete _pVertexBuffer;
}

static unsigned LookupPrimitiveType(VertexBufferPrimitiveType _PrimitiveType)
{
	switch(_PrimitiveType)
	{
	case VB_PRIMITIVE_POINTS:		return GL_POINTS;
	case VB_PRIMITIVE_LINES:		return GL_LINES;
	case VB_PRIMITIVE_LINE_LOOP:	return GL_LINE_LOOP;
	case VB_PRIMITIVE_LINE_STRIP:	return GL_LINE_STRIP;
	case VB_PRIMITIVE_TRIANGLES:	return GL_TRIANGLES;
	case VB_PRIMITIVE_TRIANGLE_STRIP:	return GL_TRIANGLE_STRIP;
	case VB_PRIMITIVE_TRIANGLE_FAN:	return GL_TRIANGLE_FAN;
	case VB_PRIMITIVE_QUADS:		return GL_QUADS;
	case VB_PRIMITIVE_QUAD_STRIP:	return GL_QUAD_STRIP;
	}
	PXFASSERT(0, "Unknown primitive type.");
	return 0;
}

void DeviceGL2::DrawBuffer(VertexBuffer* _pVertexBuffer)
{
	_pVertexBuffer->_PreDraw();
	GLuint primitive = LookupPrimitiveType(_pVertexBuffer->GetPrimitive());
	glDrawArrays(primitive, 0, _pVertexBuffer->GetVertexCount());
	_pVertexBuffer->_PostDraw();
}

RenderTarget* DeviceGL2::CreateRenderTarget(int _Width,int _Height,RTFormat _ColorFormat,RTFormat _DepthFormat)
{
	bool t_FBOSupported = IsExtensionSupported("GL_ARB_framebuffer_object");
	bool t_PBOSupported = IsExtensionSupported("GL_ARB_pixel_buffer_object");

	if (t_FBOSupported)
		return new FBO(this);
	else if(t_PBOSupported)
		return new PBO(this);
	else
	{
		Message(LOCAL_MSG,"Graphics card does not have support for buffer objects");
		return NULL;
	}
}

void DeviceGL2::BindRenderTarget(RenderTarget* _RenderTarget)
{
	// ??
	BindRenderTarget(_RenderTarget,1);
}

void DeviceGL2::BindRenderTarget(RenderTarget* _RenderTarget, int _DrawID = 1)
{
	RTType _Type =_RenderTarget->GetType();

	if (_Type == RT_TYPE_FBO)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ((FBO*)_RenderTarget)->GetFBOHandle() );
		glDrawBuffers(_DrawID, ((FBO*)_RenderTarget)->GetColorAttachments());
	}
	else if(_Type == RT_TYPE_PBO)
	{
		// do something
		;
	}
	else
		Message(LOCAL_MSG,"Trying to bind RenderType to incompatible device context");
}

void DeviceGL2::ReleaseRenderTarget(RenderTarget* _RenderTarget)
{
	RTType _Type =_RenderTarget->GetType();
	if (_Type == RT_TYPE_FBO)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else if(_Type == RT_TYPE_PBO)
	{
		// do something
		;
	}
}



