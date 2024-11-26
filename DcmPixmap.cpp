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


#include "DcmPixmap.h"

#include <config/osconfig.h> // DCMTK configuration
#include <dcmimgle/dcmimage.h>
#include <ofstd/ofbmanip.h>
#include <dcmdata/dcfilefo.h>
#include <dcmimage/diregist.h> // support for color images

#include <iostream>
#include <fstream>
//Added by qt3to4:
#include <QPixmap>

DcmPixmap::DcmPixmap(const QPixmap& pm) : QPixmap(pm)
{ }

DcmPixmap::DcmPixmap(string dcmFile, unsigned long newWidth, unsigned long newHeight)
{
	loadDicom(dcmFile, newWidth, newHeight);
}

DcmPixmap::DcmPixmap() { }

void DcmPixmap::loadDicom(string dcmFile, unsigned long newWidth, unsigned long newHeight)
{
	//cout << dcmFile << endl;
	DicomImage *image = new DicomImage(dcmFile.c_str(), CIF_MayDetachPixelData | CIF_TakeOverExternalDataset);
	EI_Status status = image->getStatus();
	if (status == EIS_Normal)
	{
		
		//cout << "Status OK" << endl;
	    /* get image extension */
	    
	    if (newHeight > 0 && newWidth > 0)
	    { // rescale image
	    	DicomImage *newImg = image->createScaledImage(newWidth, newHeight);
	    	delete image;
	    	image = newImg;
	    }
	    
	    const int width = (int)(image->getWidth());
	    const int height = (int)(image->getHeight());
	   
	    double min, max;
	    
	    //cout << "Width: " << width << " Height: " << height << endl;
	    
	    image->getMinMaxValues(min, max);
	    
	    //cout << "Max: " << (int)max << endl;
	    
	    const int originalBitN = image->getDepth();
	    
	    unsigned int maxLevels = 1 << originalBitN;
	    
	    //cout << "MaxLevels: " << maxLevels << endl;
	    
	    const int realMax = (int)((double)max * 255 / maxLevels);
	    
	    //cout << "Real max: " << realMax << endl;
	   
	    char header[32];
	    char format[] = "PGM";
	    int bytesPerPixel;
	   
	   	if (image->isMonochrome())
	   	{ // Image is monochrome   
		    
		    // create PGM header
		    sprintf(header, "P5\n%i %i\n%i\n", width, height, realMax);
		    // format = "PGM"; Format is already equal to PGM
		    bytesPerPixel = 1;
		    
	   	} else // image is in colour: use PPM
	   	{
	   		sprintf(header, "P6\n%i %i\n255\n", width, height);
	   		format[1] = 'P'; // changes PGM to PPM
	   		bytesPerPixel = 3; // 3 bytes: R,G,B
	   	}
	    const int offset = strlen(header);
	    const unsigned int length = width * height * bytesPerPixel + offset;
	    // create output buffer for DicomImage class
	    Uint8 *buffer = new Uint8[length];
	    if (buffer != NULL)
	    {
	        // copy PGM header to buffer 
	        OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
	        if (image->getOutputData((void *)(buffer + offset), length, 8))
	        {
	        	//cout << "Storage successful" << endl;
                //setOptimization(QPixmap::BestOptim);
                loadFromData((const unsigned char *)buffer, length, format, Qt::AvoidDither);
	        }
	        // delete temporary pixel buffer 
	        delete[] buffer;
	    }
	} // Status is not OK!
	else
		cerr << "Error reading DICOM! " << status << " " << DicomImage::getString(status) << endl;
	
	delete image;
}
