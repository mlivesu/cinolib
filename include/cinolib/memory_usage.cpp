/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
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
#include <cinolib/memory_usage.h>

// Resources:
// https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-runtime-using-c/19770392#19770392
// https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

#ifdef __linux__
#include <sys/resource.h>
#endif

#ifdef __APPLE__
#include <mach/mach.h>
#endif

namespace cinolib
{

CINO_INLINE
size_t memory_usage_in_bytes()
{
#ifdef _WIN32
    assert(false && "THIS CODE HASN'T BEEN TESTED YET!");
    return 0;
    //PROCESS_MEMORY_COUNTERS_EX pmc;
    //GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    //return (size_t) pmc.WorkingSetSize;
#endif

#ifdef __linux__
    assert(false && "THIS CODE HASN'T BEEN TESTED YET!");
    return 0;
    //long rss = 0L;
    //FILE* fp = NULL;
    //if((fp = fopen("/proc/self/statm","r")) == NULL)
    //{
    //    std::cout << "Cinolib::memory_usage_in_bytes() => Failed to query LinuxOS!" << std::endl;
    //    return (size_t)0L;
    //}
    //if(fscanf(fp,"%*s%ld",&rss)!=1)
    //{
    //    fclose(fp);
    //    std::cout << "Cinolib::memory_usage_in_bytes() => Failed to query LinuxOS!" << std::endl;
    //    return (size_t)0L;
    //}
    //fclose(fp);
    //return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
#endif

#ifdef __APPLE__
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if(task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS)
    {
        std::cout << "Cinolib::memory_usage_in_bytes() => Failed to query MacOS!" << std::endl;
        return (size_t)0L;
    }
    return (size_t)info.resident_size;
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
float memory_usage_in_kilo_bytes()
{
    static const double KByte = 1024.0;
    return memory_usage_in_bytes() / KByte;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
float memory_usage_in_mega_bytes()
{
    static const double MByte = 1048576.0;
    return memory_usage_in_bytes() / MByte;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
float memory_usage_in_giga_bytes()
{
    static const double GByte = 1073741824.0;
    return memory_usage_in_bytes() / GByte;
}

}
