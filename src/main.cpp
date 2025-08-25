#include "stdafx.h"

using namespace std;
using namespace pugi;
using namespace Gdiplus;
using namespace TCLAP;

Gdi _gdi;

void ConvertFiles(const wstring& pattern, const wstring& outputFolderPath, int g00Format);
void ConvertFile(const wstring& filePath, const wstring& outputFolderPath, int g00Format);
void ConvertG00(const wstring& g00FilePath, const wstring& outputFolderPath);
void ConvertPng(const wstring& pngFilePath, const wstring& outputFolderPath, int g00Format);

int wmain(int numArgs, const wchar_t** ppArgs)
{
    ValueArg<wstring> outputFolderArg(L"d", L"outdir", L"Folder for output files (defaults to current directory)", false, L"", L"folder");
    ValueArg<int> g00FormatArg(L"g", L"g00", L"G00 format to use for output (0 or 2)", false, -1, L"int");
    UnlabeledMultiArg<wstring> filePaths(L"files", L"The files to convert", true, L"file");

    try
    {
        CmdLine cmdLine(L"", L' ', L"1.0");
        cmdLine.add(outputFolderArg);
        cmdLine.add(g00FormatArg);
        cmdLine.add(filePaths);
        cmdLine.parse(numArgs, ppArgs);
    }
    catch (ArgException& e)
    {
        wcerr << e.error() << endl;
        return 1;
    }

    try
    {
        for (const wstring& filePath : filePaths)
        {
            ConvertFiles(filePath, outputFolderArg.isSet() ? outputFolderArg.getValue() : L"", (int)g00FormatArg.getValue());
        }
    }
    catch (exception& e)
    {
        wcerr << e.what() << endl;
        return 1;
    }
    return 0;
}

void ConvertFiles(const wstring& pattern, const wstring& outputFolderPath, int g00Format)
{
    wstring inputFolderPath = Path::GetDirectoryName(pattern);

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(pattern.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        throw exception("Input file does not exist");

    do
    {
        wstring filePath = Path::Combine(inputFolderPath, findData.cFileName);
        ConvertFile(filePath, outputFolderPath, g00Format);
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
}

void ConvertFile(const wstring& filePath, const wstring& outputFolderPath, int g00Format)
{
    if (!File::Exists(filePath))
        throw exception("Input file does not exist");

    if (!outputFolderPath.empty() && !Directory::Exists(outputFolderPath))
        throw exception("Output directory does not exist");

    wstring extension = Path::GetExtension(filePath);
    if (extension == L"g00")
        ConvertG00(filePath, outputFolderPath);
    else if (extension == L"png")
        ConvertPng(filePath, outputFolderPath, g00Format);
    else
        throw exception("Unknown input file format. Only .g00 and .png are supported");
}

void ConvertG00(const wstring& g00FilePath, const wstring& outputFolderPath)
{
    shared_ptr<G00File> pG00 = G00File::Load(g00FilePath);
    shared_ptr<Image> pImage = pG00->ToImage();

    wstring pngFilePath = Path::Combine(outputFolderPath, Path::ChangeExtension(Path::GetFileName(g00FilePath), L"png"));
    _gdi.SaveImage(pImage.get(), pngFilePath);

    G00File2* pG002 = dynamic_cast<G00File2*>(pG00.get());
    if (pG002 != nullptr)
    {
        shared_ptr<xml_document> pDoc = pG002->GetRegionXml();
        pDoc->save_file(Path::ChangeExtension(pngFilePath, L"xml").c_str(), L"  ");
    }
}

void ConvertPng(const wstring& pngFilePath, const wstring& outputFolderPath, int g00Format)
{
    shared_ptr<Bitmap> pImage = shared_ptr<Bitmap>(new Bitmap(pngFilePath.c_str()));

    bool xmlExists = File::Exists(Path::ChangeExtension(pngFilePath, L"xml"));
    if (g00Format < 0)
        g00Format = xmlExists ? 2 : 0;

    if (g00Format == 2 && !xmlExists)
        throw exception("No XML file exists for the image");

    shared_ptr<G00File> pG00 = G00File::Create(g00Format);
    G00File2* pG002 = dynamic_cast<G00File2*>(pG00.get());
    if (pG002 != nullptr)
    {
        xml_document doc;
        doc.load_file(Path::ChangeExtension(pngFilePath, L"xml").c_str());
        pG002->ApplyRegionXml(&doc);
    }

    pG00->FromImage(pImage.get());

    wstring g00FilePath = Path::Combine(outputFolderPath, Path::ChangeExtension(Path::GetFileName(pngFilePath), L"g00"));
    pG00->Save(g00FilePath);
}
