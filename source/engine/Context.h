/**
 * @author: Guido Schmidt
 * @date: 30.11.12
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_
//! To use GLFW & AntTweakBar define GLFW_CDEL
#define GLFW_CDECL
//! C++ includes
#include <string>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
#include <GL/glfw.h>
//! AntTweakBar include
#include <AntTweakBar.h>

class Context
{
	private:
		std::string TITLE;
		TwBar* bar;

	protected:
		void setSize(int width, int height);
		void setTitle(std::string title);

	public:
		Context(int width = 500, int height = 500);
		~Context();
		void OpenWindow(int width, int height, std::string title, int openglMajorVersion, int openglMinorVersion);
		void SwapBuffers(void);
		bool IsExiting(void);
		void AddAntTweakBar(void);

		//! Getter
		std::string* GetTitle();
		TwBar* GetBar();
		int GetWidth();
		int GetHeight();

		//! Setter
		void SetSize(int width, int height);
};

#endif /* CONTEXT_H_ */