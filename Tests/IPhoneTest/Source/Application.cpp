/*
 *  Application.cpp
 *  project
 *
 *  Created by jhonny on 6/2/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "../Include/Application.h"
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

using namespace Pxf;

Application::Application(const char* _Title)
	: m_Title(_Title),
	  m_Engine(0),
	  m_IsRunning(false),
	  m_TestSprite(0)
{

}

Application::~Application()
{
	m_Title = 0;
	delete m_Engine;
}
	

bool Application::Update()
{
	bool _RetVal = true;
	// Call update on scene
	
	_UpdateFPS();

	
	return _RetVal;
}

void Application::_UpdateFPS()
{
	m_FPS.ticks++;
	
	if(m_FPS.elapsed_time >= 60.0f)
	{
		m_FPS.fps = m_FPS.ticks / m_FPS.elapsed_time;
		m_FPS.ticks = 0;
		m_FPS.elapsed_time = 0.0f;
	}	
}

bool Application::Render()
{
	bool _RetVal = true;
	// Call render on scene 
	
	
	const GLfloat squareVertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };
    const GLubyte squareColors[] = {
        255, 255,   0, 255,
        0,   255, 255, 255,
        0,     0,   0,   0,
        255,   0, 255, 255,
    };
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	return _RetVal;
}

bool Application::Init()
{
	m_Engine		= new Engine();	
	m_IsRunning		= true;
	//m_TestSprite	= new Pxf::Game::Sprite(
	
	
	if(!m_Engine)
	{
		m_IsRunning = false;
	}
	
	return m_IsRunning;
}

bool Application::IsRunning()
{
	return m_IsRunning;
}

void Application::Shutdown()
{
	m_IsRunning = false;
	printf("Application: Shutting down\n");
}
