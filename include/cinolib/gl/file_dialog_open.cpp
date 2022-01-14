/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2021: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/gl/file_dialog_open.h>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#undef max
#undef min
#include <Commdlg.h>
#endif

namespace cinolib
{

CINO_INLINE
std::string file_dialog_open()
{
  char buffer[1024];
  buffer[0       ] = '\0';
  buffer[1024 - 1] = 'x'; // Initialize last character with a char != '\0'

#ifdef __APPLE__
    // For apple use applescript hack
    FILE * output = popen("osascript -e \""
                          "   tell application \\\"System Events\\\"\n"
                          "           activate\n"
                          "           set existing_file to choose file\n"
                          "   end tell\n"
                          "   set existing_file_path to (POSIX path of (existing_file))\n"
                          "\" 2>/dev/null | tr -d '\n' ","r");
    if(output)
    {
        auto ret = fgets(buffer, 1024, output);
        if(ret == NULL || ferror(output))
        {
            // I/O error
            buffer[0] = '\0';
        }
        if(buffer[1024 - 1] == '\0')
        {
            // File name too long, buffer has been filled, so we return empty string instead
            buffer[0] = '\0';
        }
    }

#elif defined _WIN32

    // Use native windows file dialog box
    // (code contributed by Tino Weinkauf)

    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = new char[100];
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "*.*\0";//off\0*.off\0obj\0*.obj\0mp\0*.mp\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.
    int pos = 0;
    if(GetOpenFileName(&ofn)==TRUE)
    {
        while(ofn.lpstrFile[pos] != '\0')
        {
            buffer[pos] = (char)ofn.lpstrFile[pos];
            pos++;
        }
    }
    buffer[pos] = 0;

#else

    // For linux use zenity
    FILE * output = popen("/usr/bin/zenity --file-selection","r");
    if(output)
    {
        auto ret = fgets(buffer, 1024, output);
        if(ret == NULL || ferror(output))
        {
            // I/O error
            buffer[0] = '\0';
        }
        if(buffer[1024 - 1] == '\0')
        {
            // File name too long, buffer has been filled, so we return empty string instead
            buffer[0] = '\0';
        }
    }

    // Replace last '\n' by '\0'
    if(strlen(buffer) > 0)
    {
        buffer[strlen(buffer)-1] = '\0';
    }

#endif

    return std::string(buffer);
}

}
