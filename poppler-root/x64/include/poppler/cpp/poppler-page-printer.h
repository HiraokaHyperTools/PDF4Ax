/*
 * Copyright (C) 2010, Pino Toscano <pino@kde.org>
 * Copyright (C) 2018, Zsombor Hollay-Horvath <hollay.horvath@gmail.com>
 * Copyright (C) 2022, kenjiuno
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef POPPLER_PAGE_PRINTER_H
#define POPPLER_PAGE_PRINTER_H

#ifdef WIN32

#include "poppler-global.h"
#include "poppler-image.h"

#include <cairo.h>

#define NOMINMAX
#include <windows.h>

namespace poppler {

typedef unsigned int argb;

class page;
class page_printer_private;

class POPPLER_CPP_EXPORT page_printer : public poppler::noncopyable
{
public:
    enum render_hint
    {
        antialiasing = 0x00000001,
        text_antialiasing = 0x00000002,
        text_hinting = 0x00000004
    };

    enum line_mode_enum
    {
        line_default,
        line_solid,
        line_shape
    };

    page_printer();
    ~page_printer();

    bool print_page(const page *p, HDC hdc, cairo_t *cr, bool printannot) const;

private:
    page_printer_private *d;
    friend class page_printer_private;
};

}

#endif

#endif
