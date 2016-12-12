#include "Directory.h"

Directory::Directory()
{
	//m_name = "C:\\";
}

Directory::Directory(const char * name)
{
	changeDirectory(name);
}

Directory::~Directory()
{
	delete [] m_dirList;
	m_dirList = nullptr;
}

void Directory::moveUpDirectory()
{
	int len = 0;
	int i = 0;

	//m_name[m_name.length()-1] = '\0';

	//moveup directory
	while (m_name[i] != '\0') {
		if (m_name[i] == slash[0]) {
			len = i;
		}
		++i;
	}

	//m_name[len] = '\0';
	m_name.resize(len);
}

bool Directory::changeDirectory(const char * name)
{
	DIR* dir = nullptr;
	int len = 0;
	int i = 0;
	bool re = true;


	if (name != nullptr) {
		m_name = name;
		//Remove Any Garbage
		while (m_name[i] != '\0') {
			if (m_name[i] == slash[0]) {
				len = i;
			}
			++i;
		}
		//m_name[len] = '\0';
		m_name.resize(len);

		if ((dir = opendir(m_name.c_str())) != nullptr) {
			closedir(dir);
		}
		else {
			//m_name = "C:\\";
			re = false;
		}
	}

	return re;
}

int Directory::readDirectory()
{
	DIR* dir = nullptr;
	dirent* temp = nullptr;
	int count = 0;

	delete [] m_dirList;
	m_dirList = nullptr;

	//Count Entries
	if ( (dir = opendir(m_name.c_str())) != nullptr) {
		while ((temp = readdir(dir)) != nullptr) {
			if ((strCheckExt(temp->d_name) || temp->d_type == DT_DIR) && (lb_strncmp(temp->d_name, ".", 2) != 0))
				++count;
		}
		closedir(dir);
	}
	else {
		std::cout << "invalid Directory \n";
	}

	//Insert Entries Into List
	if (count > 0) {
		m_dirList = new dirent[count];

		int i = 0;
		if ((dir = opendir(m_name.c_str())) != nullptr) {
			while ((temp = readdir(dir)) != nullptr && i < count) {
				if ((strCheckExt(temp->d_name) || temp->d_type == DT_DIR) && (lb_strncmp(temp->d_name, ".", 2) != 0))
					m_dirList[i++] = *temp;
			}
			closedir(dir);
		}
		else {
			std::cout << "invalid Directory";
		}
	}
	m_dirCount = count;

	return count;
}

const char* Directory::getFileName(const int index)
{
	char* re = nullptr;
	if (index < m_dirCount) {
		re = m_dirList[index].d_name;
	}
	
	return re;
}

const char * Directory::getFilePath(const int index)
{
	const char* temp = nullptr;

	if (index < m_dirCount) {
		m_buffer = m_name;
		m_buffer.append(slash); 
		m_buffer.append(m_dirList[index].d_name);
		if (m_dirList[index].d_type == DT_DIR)		
			m_buffer.append(slash); 
		temp = m_buffer.c_str();
	}

	return temp;
}

bool Directory::isDir(const int index)
{
	bool re = false;
	if (index < m_dirCount) {
		if (m_dirList[index].d_type == DT_DIR)
			re = true;
	}

	return re;
}

int Directory::strCheckExt(char * str)
{
	int re = 0;

	if (strCheckExt(str, ".gb"))
		re = 1;

	//Removed Untill Naming Convention is Decided/Implemented
	/*else if (strCheckExt(str, ".sav"))
		re = 2;
	else if (strCheckExt(str, ".state"))
		re = 3;*/

	return re;
}

bool Directory::strCheckExt(char* str, const char* ext)
{
	const char* temp = ext;
	while (*(str) != '\0') {
		if (*str != *temp)
			temp = ext;
		else
			++temp;
		++str;
	}

	return ((*temp) ? false : true);
}
