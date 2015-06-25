// name: vao.h
// type: c++ header
// desc: class declaration
// auth: is0urce

// vertex array object for opengl

#pragma once

#include "gl\glew.h"
#include <gl\gl.h>
#include <gl\glu.h>

#include <vector>

namespace px
{
	namespace shell
	{
		struct VAO
		{
		private:
			bool _init;
			int _num; // number of attributes
			std::unique_ptr<int[]> _depth; // attribute depth
			int _length; // size of index array

			GLuint vao;
			GLuint indices;
			std::unique_ptr<GLuint[]> buffers;

		public:
			VAO() : _init(false), _num(0), _length(0) {}
		private:
			VAO(const VAO &that);

		public:
			// count - number of buffers
			// depth - array of buffer element size
			inline void Init(int count, int* depths)
			{
				if (count <= 0) throw std::exception("VAO::Init(int count, int*) count <= 0");

				if (_init)
				{
					Clear();
				}
				_init = true;

				_num = count;
				_depth.reset(new int[_num]);
				for (int i = 0; i < _num; ++i)
				{
					_depth[i] = depths[i];
				}
				buffers.reset(new GLuint[_num]);

				glGenBuffers(1, &indices);
				glGenBuffers(_num, buffers.get());
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);
				for (int i = 0; i < _num; ++i)
				{
					glEnableVertexAttribArray(i);
				}
				for (int i = 0; i < _num; ++i)
				{
					glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
					glVertexAttribPointer(i, depths[i], GL_FLOAT, GL_FALSE, 0, 0);
				}
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
			}
			inline void Init(std::vector<int> depths)
			{
				if (depths.empty()) throw std::exception("void VAO::Init(std::vector<int> depth) depth is empty");
				Init(depths.size(), &depths[0]);
			}
			inline int GetDepth(int attribute) const
			{
				if (attribute <= _num) throw std::runtime_error("VAO::GetDepth(int attribute) const - attribute out of range");
				return _depth[attribute];
			}
			inline void Clear()
			{
				if (_init)
				{
					glDeleteVertexArrays(1, &vao);
					glDeleteBuffers(1, &indices);
					glDeleteBuffers(_num, buffers.get());
				}
				_init = false;
				_num = 0;
				_length = 0;
				_depth.reset();
				buffers.reset();
			}
			// points - number of points in values
			// length - number of indices in index_values
			inline void Fill(int points, int length, GLfloat** values, GLuint* index_values)
			{
				_length = length;
				glBindVertexArray(vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_values[0]) * length, index_values, GL_STATIC_DRAW);

				for (int i = 0; i < _num; i++)
				{
					glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
					glBufferData(GL_ARRAY_BUFFER, sizeof(values[i][0]) * points * _depth[i], values[i], GL_STATIC_DRAW);
				}
			}
			inline void Draw() const
			{
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, _length, GL_UNSIGNED_INT, 0);
			}
			inline void Draw(int points, int length, GLfloat** values, GLuint* index_values)
			{
				Fill(points, length, values, index_values);
				Draw();
			}
		};
	}
}