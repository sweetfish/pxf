#ifndef _PXF_GRAPHICS_WINDOWD3D_H_
#define _PXF_GRAPHICS_WINDOWD3D_H_

#ifdef CONF_FAMILY_WINDOWS

#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <windows.h>
#include <d3dx9.h>

namespace Pxf{
	namespace Graphics {

		class WindowD3D : public Window
		{
		public:
			WindowD3D(WindowSpecifications *_window_spec);
			~WindowD3D();

			bool Open();
			bool Close();
			void Swap();

			void SetTitle(const char *_title);

			int GetFPS();
			int GetWidth();
			int GetHeight();
			float GetAspectRatio();
			char* GetContextTypeName();

			bool IsOpen();
			/*
			bool IsActive();
			bool IsMinimized();
			*/

			// Specific D3D getters
			IDirect3DDevice9* GetD3DDevice();
		private:
			friend class DeviceD3D9;
			bool InitWindow();
			bool InitD3D();
			bool KillWindow();
			bool KillD3D();

			// forward declare?
			WNDCLASS m_window_class;
			HWND m_window;
			IDirect3D9 *m_D3D;
			IDirect3DDevice9 *m_D3D_device;
			D3DPRESENT_PARAMETERS m_pp;

			int m_width, m_height;
			bool m_fullscreen, m_resizeable, m_vsync;

			// Bit settings
			int m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil;

			// FSAA
			int m_fsaa_samples;

			// FPS
			int64 m_fps_laststamp;
			int m_fps, m_fps_count;
		};

	} // Graphics
} // Pxf

#endif // CONF_FAMILY_WINDOWS

#endif //_PXF_GRAPHICS_WINDOWD3D_H_
