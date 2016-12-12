#pragma once
#include <string>
#include <dirent.h>
#include <iostream>
#include "common.h"

class Directory {
	public:
		Directory();
		Directory(const char* dir);
		~Directory();

		void moveUpDirectory();
		bool changeDirectory(const char* name); 
		int readDirectory();

		const char* getFileName(const int index);
		const char* getFilePath(const int index);

		bool isDir(const int index);
	private:
		int strCheckExt(char* str);
		bool strCheckExt(char* str, const char * ext);
		int strcmp(const char* str1, const char* str2);
		dirent* m_dirList = nullptr;
		std::string m_name;
		std::string m_buffer;
		int m_dirCount = 0;

		#if defined(_WIN32)
			const char slash[2] = "\\";
		#else
			const char slash[2] = "/";
		#endif
};


