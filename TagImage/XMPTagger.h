#pragma once
#include <exiv2/exiv2.hpp>


using namespace std;

class XMPTagger
{
public:
	XMPTagger(string fileName);
	~XMPTagger();
	bool hasXmpTags();
	void setSubject(vector <string> &keyWords);
	bool save();
	void print(string xmpKey);
	void printAll();
private:
	Exiv2::Image::AutoPtr image;
};