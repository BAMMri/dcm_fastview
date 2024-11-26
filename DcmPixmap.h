/*
    This file is part of dcm_fastview.

    dcm_fastview is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    dcm_fastview is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with dcm_fastview.  If not, see <http://www.gnu.org/licenses/>.
    
    Copyright 2007, 2009 Francesco Santini (francesco.santini@unibas.ch)

*/


#ifndef _DCMPIXMAP_H_
#define _DCMPIXMAP_H_

#include <qpixmap.h>
#include <string>
using namespace std;

class DcmPixmap : public QPixmap
{
public:
	DcmPixmap(const QPixmap&);
	DcmPixmap(string dcmFile, unsigned long newWidth=0, unsigned long newHeight=0);
	DcmPixmap();
	void loadDicom(string dcmFile, unsigned long newWidth=0, unsigned long newHeight=0);
};

#endif //_DCMPIXMAP_H_
