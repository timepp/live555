#pragma once

#include <stdio.h>
#include <stdarg.h>

namespace tp
{
	/** ��ʽ���ڴ��Ƭ���ࣺһ���ṩ�����ַ�ָ��ת������ʱ�������������Բ�ͬ��ʽ������ʵ�ֲ�ͬ�Ĺ���
	* ��������ʼ�ǿ���ջ�ϵģ���ջ�ϵĻ�������������Ҫʱ���ͻ����´Ӷ��Ϸ����ڴ�
	* һ����˵�����ڹ��캯�������m_buf. ��������ʵ�ʵĿռ��С����resize.
	* \note ��ʽ���ڴ��Ƭ�������ɱ�����б��еĲ���ʱ���������ʽת����const T*����������ʱ����ǰ��&
	*/
	template <typename T, size_t size>
	class format_shim
	{
	public:
		operator const T* () const { return m_buf; }
		const T* operator& () const { return m_buf; }

	protected:
		format_shim() : m_buf(m_buf_content), m_buf_size(size)
		{
		}
		~format_shim()
		{
			free();
		}

		void resize(size_t new_size)
		{
			if (new_size > m_buf_size)
			{
				free();
				m_buf = new T[new_size];
				m_buf_size = new_size;
			}
		}

		T* m_buf;
		size_t m_buf_size;

	private:
		T m_buf_content[size];

		void init()
		{
			m_buf = m_buf_content;
			m_buf_size = size;
		}
		void free()
		{
			if (m_buf != m_buf_content)
			{
				delete[] m_buf;
			}
		}
	};


	// hex_dumper ���ڴ�����dump�ɿɶ��汾
	template <typename T, size_t buf_size = 1024>
	class hex_dumper : public format_shim<T, buf_size>
	{
	public:
		hex_dumper(const void* data, size_t len, size_t indent = 0, size_t bytes_per_line = 16, bool show_ascii = true) : m_gap(0), m_show_ascii(show_ascii)
		{
			const size_t line_size = get_line_size(bytes_per_line, indent);
			size_t line_count = (len + bytes_per_line - 1) / bytes_per_line;
			format_shim<T, buf_size>::resize(line_size * line_count + 1);
			_hex_dump(format_shim<T, buf_size>::m_buf, data, len, indent, bytes_per_line);
		}

	private:
		size_t m_gap;
		bool m_show_ascii;

		size_t get_line_size(size_t bytes_per_line, size_t indent)
		{
			if (m_show_ascii)
			{
				return bytes_per_line * 4 + indent + m_gap + 1;
			}
			else
			{
				return bytes_per_line * 3 - 1 + indent + 1;
			}
		}
		void _hex_dump(T* buf, const void* data, size_t len, size_t indent, size_t bytes_per_line)
		{
			const T cmap[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
			const size_t ascii_pos = bytes_per_line * 3 + indent + m_gap;
			const size_t line_size = get_line_size(bytes_per_line, indent);

			const unsigned char* p = static_cast<const unsigned char*>(data);
			const unsigned char* q = p + len;
			T* line = buf;
			for (; p < q; p += bytes_per_line, line += line_size)
			{
				for (size_t i = 0; i < line_size; i++) line[i] = ' ';
				for (size_t j = 0; j < bytes_per_line && p + j < q; j++)
				{
					int v = (p[j] + 256) % 256;
					line[indent + j * 3] = cmap[v / 16];
					line[indent + j * 3 + 1] = cmap[v % 16];
					if (m_show_ascii)
					{
						line[indent + ascii_pos + j] = (v >= 0x20 && v <= 0x80) ? static_cast<T>(v) : static_cast<T>('.');
					}
				}
				line[line_size - 1] = '\n';
			}
			*line = 0;
			if (line > buf) line[-1] = 0;
		}
	};


	typedef hex_dumper<char>       hex_dumpA;
	typedef hex_dumper<wchar_t>    hex_dump;

}