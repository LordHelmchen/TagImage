# TagImage

A small tool that writes XMP-Tags describing the image subject into the Xmp.dc.subject field of image files. The tags are generated by uploading the file to imagga.com via their REST-API.
You need an account from imagga for this to work. They offer free ones for developers.

It is written for categorization of offline pictures. After they have been tagged they can be searched by keyword using XNView MP for example.

## Installation

Unzip, enter your API details in config.txt and you are ready to go. You can configure the minimum confidence in percent and the keyword language in the config.txt aswell.

## Usage

Just pass it an image file as argument.

```
TagImage.exe  [-?|-h|--help] <fileName> [-o]
OPTIONS, ARGUMENTS:
  -?, -h, --help
  <fileName>              File to tag
  -o <overwrite>          overwrite existing subject tag
```

The default behaviour is to skip files, that already have XMP subject data. You can force it to overwrite these with the -o option.

## Build

I just built it with Visual Studio 2019, but it is a Cmake project without operating system dependencies. The dependencies are available as source code.

### Dependencies

vcpkg install exiv2[xmp]:x64-windows
vcpkg install simpleini:x64-windows
vcpkg install cpprestsdk:x64-windows
vcpkg install lyra:x64-windows

## Credits

It uses the multipart form parser from Ye Yangang.