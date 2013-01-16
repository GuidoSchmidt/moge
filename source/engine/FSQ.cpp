//! FSQ.cpp
/*! 
 * @date	01.12.2012
 * @auhtor 	Guido Schmidt
 */

#include "FSQ.h"

//!
/*!
 *
 */
FSQ::FSQ()
{
	m_VAO_ID = 0;
	m_VBO_ID = 0;
	m_IBO_ID = 0;
	m_UVBO_ID = 0;
}


//!
/*!
 *
 */
FSQ::~FSQ()
{
	DeleteBuffers();
}


//!
/*!
 *
 */
void FSQ::CreateBuffers(void)
{
	GLfloat fsqVertices[] = {
			-1.0f,  1.0f,  0.0f,
			-1.0f, -1.0f,  0.0f,
			 1.0f, -1.0f,  0.0f,
			 1.0f,  1.0f,  0.0f
	};
	GLint fsqIndices[] = {
			0, 1, 3,
			2, 3, 1
	};
	GLfloat fsqUVs[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
	};

	glGenVertexArrays(1, &m_VAO_ID);
	glBindVertexArray(m_VAO_ID);

	glGenBuffers(1, &m_VBO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fsqVertices), fsqVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_IBO_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fsqIndices), fsqIndices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_UVBO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_UVBO_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fsqUVs), fsqUVs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}


//!
/*!
 *
 */
void FSQ::DeleteBuffers(void)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


//!
/*!
 *
 */
void FSQ::CreateFSQ(void)
{
	CreateBuffers();
}


//!
/*!
 *
 */
void FSQ::Draw(void)
{
	glBindVertexArray(m_VAO_ID);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
