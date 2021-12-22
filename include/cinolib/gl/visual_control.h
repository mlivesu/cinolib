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
#ifndef CINO_VISUAL_CONTROL_H
#define CINO_VISUAL_CONTROL_H

#include <string>

namespace cinolib
{

/*
 * This class models a generic ImGui widget to be displayed inside a cinolib::GLcanvas
 *
 * Each VisualControl must be associated with a unique name, whch is internally used by ImGui
 * to distinguish between the various items. Ideally, each VisualControl models a window with
 * some controls (buttons, sliders,...). Pushing two visual controls with same name in the same
 * canvas will result in a window containing both controls.
 *
 * All controls and relative event handling must be implemented in the draw() method.
 * This function will be called by the GLcanvas from within a
 *
 *     ImGui::NewFrame();
 *     {
 *          for each visual control vc
 *          {
 *              if(vc->show)
 *              {
 *                  ImGui::Begin(vc->name, vc->show);
 *                  {
 *                      vc->draw();
 *                  }
 *                  ImGui::End();
 *              }
 *          }
 *     }
 *     ImGui::Render();
 *
 * block. Therefore, draw() is expected to only contain code for the controls themselves,
 * and not for window/frame handling.
*/

class VisualControl
{
    public :

        std::string name;
        bool        show       = true;
        float       rel_pos[2] = {0,0};
        float       rel_width  = 0.25;  // this is meant to be a LEFT side bar in the window
        float       rel_height = 1.0;
        float       alpha      = 1.0;

        explicit  VisualControl(const std::string & name) : name(name) {}
        virtual  ~VisualControl() {}

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        virtual void draw() = 0;
};

}

#endif // CINO_VISUAL_CONTROL_H
