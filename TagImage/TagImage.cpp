﻿// TagImage
// A small tool that writes XMP - Tags describing the image subject into the Xmp.dc.subject field of image files.The tags are generated by uploading the file to imagga.com via their REST - API.
// You need an account from imagga for this to work.They offer free ones for developers.
// 
// It is written for categorization of offline pictures.After they have been tagged they can be searched by keyword using XNView MP for example.
// Copyright 2020 Jonas Tampier

#include "TagImage.h"
#include "XMPTagger.h"
#include <lyra/lyra.hpp>
#include "ImaggaClient.h"
#include <codecvt>
#include <simpleini.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

int main(int argc, char* argv[])
{
	string fileName;
    wstring apiKey, apiSecret, langCode;
    double minConfidence = 30.0;
    using convert_type_utf8 = std::codecvt_utf8<wchar_t>;
    wstring_convert<convert_type_utf8, wchar_t> utf8converter;
    CSimpleIni ini;
    ini.SetUnicode(true);
    SI_Error rc = ini.LoadFile("config.txt");
    if (rc < 0) {
        cerr << "config.txt could not be opened." << endl;
        return 1;
    };
    const char* pv;
    pv = ini.GetValue("", "imagga_apikey", "<replace-with-your-api-key>");
    apiKey = utf8converter.from_bytes(pv);
    pv = ini.GetValue("", "imagga_apisecret", "<replace-with-your-api-secret>");
    apiSecret = utf8converter.from_bytes(pv);
    if(apiKey == L"<replace-with-your-api-key>" || apiSecret == L"<replace-with-your-api-secret>")
    {
        cerr << "'imagga_apikey' or 'imagga_apisecret' is not set in config.txt." << endl;
        return 1;
    }
    minConfidence = ini.GetDoubleValue("", "imagga_minimum_confidence", 30.0);
    pv = ini.GetValue("", "language_code", "de");
    langCode = utf8converter.from_bytes(pv);

    bool overwriteTags = false, show_help = false;
    auto cli = lyra::cli();
    cli.add_argument(lyra::help(show_help));
    cli.add_argument(lyra::arg(fileName, "fileName").required().help("File to tag"));
    cli.add_argument(lyra::opt(overwriteTags, "overwrite").name("-o").help("overwrite existing subject tag").optional());
    // Parse the program arguments:
    auto result = cli.parse({ argc, argv });
    // Check that the arguments where valid:
    if (!result)
    {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        cout << cli << endl;
        return 1;
    }
    if (show_help)
    {
        cout << cli << endl;
        return 0;
    }
	cout << "Opening file:" << fileName << endl;
    if (!fs::exists(fileName))
    {
        cerr << "File " << fileName << " could not be found.";
        return 3;
    }
    auto xt = new XMPTagger(fileName);
    if (!overwriteTags && xt->hasXmpTags())
    {
        std::cerr << "Xmp.dc.subject is not empty.";
        return 2;
    }
    else
    {
        fs::path p = fileName;
        auto ftime = fs::last_write_time(p);

        ImaggaClient ic(apiKey, apiSecret, langCode);
        try
        {
            int callsLeft = ic.apiCallsLeft();
            cout << "You have " << callsLeft << " API calls left." << endl;
            if (callsLeft <= 0)
            {
                cerr << "No API calls left." << endl;
                return 5;
            }
        }
        catch(runtime_error &e){
            cerr << e.what() << endl;
            return 4;
        }
        ic.fetch(fileName);
        ic.scrub(minConfidence);
        vector<string> keyWords;
        for each (auto t in ic.tags())
            keyWords.push_back(utf8converter.to_bytes(t));
        xt->setSubject(keyWords);
        xt->save();
        fs::last_write_time(p, ftime);
    }
	return 0;
}
