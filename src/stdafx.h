#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <Windows.h>
#include <gdiplus.h>
#undef min
#undef max

typedef BYTE byte;
typedef WORD word;
typedef DWORD dword;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#include "tclap\CmdLine.h"

#include "pugi\pugiconfig.hpp"
#include "pugi\pugixml.hpp"

#include "IO\Path.h"
#include "IO\Stream.h"
#include "IO\FileStream.h"
#include "IO\MemoryStream.h"
#include "IO\Directory.h"
#include "IO\File.h"

#include "Compression\lzcomp.h"
#include "Compression\G00Compression.h"

#include "Gdi.h"
#include "G00File.h"
#include "G00File0.h"
#include "G00File2.h"
