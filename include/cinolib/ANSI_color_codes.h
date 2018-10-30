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
#ifndef CINO_ANSI_COLOR_CODES_H
#define CINO_ANSI_COLOR_CODES_H

namespace cinolib
{
    // https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

    // ANSI FORMAT ATTRIBUTES
    const char ANSI_text_default[]           = "\033[0m";
    const char ANSI_text_bold[]              = "\033[1m";
    const char ANSI_text_dim[]               = "\033[2m";
    const char ANSI_text_underlined[]        = "\033[3m";
    const char ANSI_text_blink[]             = "\033[5m";
    const char ANSI_text_reverse[]           = "\033[7m";
    const char ANSI_text_hidden[]            = "\033[8m";
    // ANSI RESET FORMAT ATTRIBUTES
    const char ANSI_reset_all[]              = "\033[0m";
    const char ANSI_reset_bold[]             = "\033[21m";
    const char ANSI_reset_dim[]              = "\033[22m";
    const char ANSI_reset_underlined[]       = "\033[24m";
    const char ANSI_reset_blink[]            = "\033[25m";
    const char ANSI_reset_reverse[]          = "\033[27m";
    const char ANSI_reset_hidden[]           = "\033[28";
    // ANSI FOREGROUND COLORS
    const char ANSI_fg_color_default[]       = "\033[39m";
    const char ANSI_fg_color_black[]         = "\033[30m";
    const char ANSI_fg_color_red[]           = "\033[31m";
    const char ANSI_fg_color_green[]         = "\033[32m";
    const char ANSI_fg_color_yellow[]        = "\033[33m";
    const char ANSI_fg_color_blue[]          = "\033[34m";
    const char ANSI_fg_color_magenta[]       = "\033[35m";
    const char ANSI_fg_color_cyan[]          = "\033[36m";
    const char ANSI_fg_color_light_gray[]    = "\033[37m";
    const char ANSI_fg_color_dark_gray[]     = "\033[90m";
    const char ANSI_fg_color_light_red[]     = "\033[91m";
    const char ANSI_fg_color_light_green[]   = "\033[92m";
    const char ANSI_fg_color_light_yellow[]  = "\033[93m";
    const char ANSI_fg_color_light_blue[]    = "\033[94m";
    const char ANSI_fg_color_light_magenta[] = "\033[95m";
    const char ANSI_fg_color_light_cyan[]    = "\033[96m";
    const char ANSI_fg_color_white[]         = "\033[97m";
    // ANSI BACKGROUND COLORS
    const char ANSI_bg_color_default[]       = "\033[49m";
    const char ANSI_bg_color_black[]         = "\033[40m";
    const char ANSI_bg_color_red[]           = "\033[41m";
    const char ANSI_bg_color_green[]         = "\033[42m";
    const char ANSI_bg_color_yellow[]        = "\033[43m";
    const char ANSI_bg_color_blue[]          = "\033[44m";
    const char ANSI_bg_color_magenta[]       = "\033[45m";
    const char ANSI_bg_color_cyan[]          = "\033[46m";
    const char ANSI_bg_color_light_gray[]    = "\033[47m";
    const char ANSI_bg_color_dark_gray[]     = "\033[100m";
    const char ANSI_bg_color_light_red[]     = "\033[101m";
    const char ANSI_bg_color_light_green[]   = "\033[102m";
    const char ANSI_bg_color_light_yellow[]  = "\033[103m";
    const char ANSI_bg_color_light_blue[]    = "\033[104m";
    const char ANSI_bg_color_light_magenta[] = "\033[105m";
    const char ANSI_bg_color_light_cyan[]    = "\033[106m";
    const char ANSI_bg_color_white[]         = "\033[107m";
}

#endif // CINO_ANSI_COLOR_CODES_H
