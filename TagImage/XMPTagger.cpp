#include "XMPTagger.h"
#include <exiv2/exiv2.hpp>
#include <exiv2/properties.hpp>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

XMPTagger::XMPTagger(string fileName)
{
	if(!fs::exists(fileName))
		throw fs::filesystem_error("File not found", error_code());
	Exiv2::XmpParser::initialize();
	image = Exiv2::ImageFactory::open(fileName);
	assert(image.get() != 0);
	assert(image->good());
	image->readMetadata();
}

XMPTagger::~XMPTagger()
{
	Exiv2::XmpParser::terminate();
}

bool XMPTagger::hasXmpTags()
{
	Exiv2::XmpData& xmpData = image->xmpData();
	if(xmpData.empty())
		return false;
	auto subject = xmpData["Xmp.dc.subject"];
	if (subject.count() > 0)
		return true;
	return false;
}

void XMPTagger::setSubject(vector<string>& keyWords)
{
	auto iptcData = image->iptcData();
	Exiv2::XmpData& xmpData = image->xmpData();
	Exiv2::Value::AutoPtr subjects = Exiv2::Value::create(Exiv2::xmpBag);
	/*auto keyKw = Exiv2::IptcKey("Iptc.Application2.Keywords");
	auto keySubj = Exiv2::IptcKey("Iptc.Application2.Subject");*/
	for each (auto kw in keyWords)
	{
		subjects->read(kw);
		/*Exiv2::Value::AutoPtr v = Exiv2::Value::create(Exiv2::TypeId::string);
		iptcData.add(keyKw, v.get());
		iptcData.add(keySubj, v.get());*/
	}
	xmpData.add(Exiv2::XmpKey("Xmp.dc.subject"), subjects.get());
}

bool XMPTagger::save()
{
	image->writeMetadata();
	return true;
}

void XMPTagger::print(string xmpKey)
{
	Exiv2::XmpData& xmpData = image->xmpData();
	cout << xmpKey << ": " << xmpData[xmpKey].value();
}

void XMPTagger::printAll()
{
	Exiv2::XmpData& xmpData = image->xmpData();
	for each(auto xd in xmpData)
		cout << xd.key() << ":" << xd.value();
}
