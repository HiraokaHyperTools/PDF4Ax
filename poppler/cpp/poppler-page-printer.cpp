/*
 * Copyright (C) 2010, Pino Toscano <pino@kde.org>
 * Copyright (C) 2015 William Bader <williambader@hotmail.com>
 * Copyright (C) 2018, Zsombor Hollay-Horvath <hollay.horvath@gmail.com>
 * Copyright (C) 2019, Julián Unrrein <junrrein@gmail.com>
 * Copyright (C) 2020, Albert Astals Cid <aacid@kde.org>
 * Copyright (C) 2021, Hubert Figuiere <hub@figuiere.net>
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

/**
 \file poppler-page-printer.h
 */
#include "poppler-page-printer.h"

#ifdef WIN32

#include "poppler-document-private.h"
#include "poppler-page-private.h"

#include <config.h>
#include <poppler-config.h>

#include "PDFDoc.h"
#include "CairoOutputDev.h"

using namespace poppler;

class poppler::page_printer_private
{
public:
    page_printer_private() { }
};

/**
 Constructs a new %page renderer.
 */
page_printer::page_printer() : d(new page_printer_private()) { }

/**
 Destructor.
 */
page_printer::~page_printer()
{
    delete d;
}

bool page_printer::print_page(const page *p, HDC hdc, cairo_t *cr) const
{
    if (!p) {
        return false;
    }

    page_private *pp = page_private::get(p);
    PDFDoc *pdfdoc = pp->doc->doc;

    const bool printing = true;
    const bool useCropBox = false;

    std::unique_ptr<CairoOutputDev> cairoOut(new CairoOutputDev());
    cairoOut->startDoc(pdfdoc);
    cairoOut->setCairo(cr);
    cairoOut->setPrinting(printing);
    pdfdoc->displayPageSlice(cairoOut.get(), pp->index + 1, 72.0, 72.0, 0, /* rotate */
                             !useCropBox, /* useMediaBox */
                             false, /* Crop */
                             printing, -1, -1, -1, -1);
    cairoOut->setCairo(nullptr);
    return true;
}

#endif
