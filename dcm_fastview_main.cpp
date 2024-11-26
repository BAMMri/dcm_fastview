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
#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <iostream>
#include <sstream>
#include <string>
#include <QPixmap>
#include <QResizeEvent>

// dcmtk includes
#include <config/osconfig.h>
#include <dcmdata/dcfilefo.h>

#include "findStringLineEdit.h"

using namespace std;

class MyLabel : public QLabel
{
protected:
    QPixmap *originalPixmap;
    
public:
    MyLabel(QWidget *parent, const char *name) : QLabel(name, parent), originalPixmap(NULL)
    { }
    
    void setPixmap(QPixmap &p)
    {
        originalPixmap = &p;
        //int w = width();
        int h = height();
	QPixmap scaledPixmap = originalPixmap->scaledToHeight(h);
        
        QLabel::setPixmap(scaledPixmap);
        
        setMinimumWidth(scaledPixmap.width());
    }
    
    void resizePixmap()
    {
        setPixmap(*originalPixmap);
    }
};
    
class MyMainWidget : public QWidget
{
private:
    MyLabel *m_myLabel;
    
public:
    MyMainWidget() : QWidget(), m_myLabel(NULL)
    {}
    
    void setLabel(MyLabel *m)
    {
        m_myLabel = m;
    }
protected:
    virtual void resizeEvent(QResizeEvent *event);
};

void MyMainWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_myLabel)
        m_myLabel->resizePixmap();
}

int main(int argc, char *argv[])
{

	if (argc < 2)
	{
		cerr << "Application needs at least one argument! (DCM file name)" << endl;
		exit(-1);
	}

	// save file name into string
	string dcmname=argv[1];
	cout << dcmname << endl;

	// read the dicom header
	DcmFileFormat curFileFormat;
	OFCondition status = curFileFormat.loadFile(dcmname.c_str());

	// check for error while loading file
	if (status.bad())
	{
		cerr << "Invalid DICOM file" << endl;
		exit(-1);
	}

	// print dataset information to a stringstream
	stringstream dicomInfoStringStream;

	size_t printFlags = DCMTypes::PF_shortenLongTagValues;
        DcmDataset *dataset = curFileFormat.getDataset();
	dataset->print(dicomInfoStringStream, printFlags);
        
        Uint8 *tempBuf;
	unsigned long returnedBytes=0;
	
	// First, get the sequence at (0x5200,0x9229)
	DcmSequenceOfItems* perFrameSeq = nullptr;
	if (dataset->findAndGetSequence(DcmTagKey(0x5200, 0x9229), perFrameSeq).good() && perFrameSeq != nullptr)
	{
	    // Get the first item from this sequence
	    DcmItem* firstFrameItem = perFrameSeq->getItem(0);
	    if (firstFrameItem != nullptr)
	    {
		// Get the nested sequence at (0x0021, 0x10fe)
		DcmSequenceOfItems* nestedSeq = nullptr;
		if (firstFrameItem->findAndGetSequence(DcmTagKey(0x0021, 0x10fe), nestedSeq).good() && nestedSeq != nullptr)
		{
		    // Get the first item from the nested sequence
		    DcmItem* nestedItem = nestedSeq->getItem(0);
		    if (nestedItem != nullptr)
		    {
			// Finally, get the value at (0x0021,0x1019)
			nestedItem->findAndGetUint8Array(DcmTagKey(0x0021, 0x1019), 
						(const Uint8*&)tempBuf, 
						&returnedBytes, 
						OFFalse);
		    }
		}
	    }
	}
	if (!returnedBytes)
	{
	    // If this is a non-enhanced DICOM, try the original direct access
	    dataset->findAndGetUint8Array(DcmTagKey(0x0021, 0x1019), 
					(const Uint8*&)tempBuf, 
					&returnedBytes, 
					OFFalse);
	}
	
	
	//dataset->findAndGetUint8Array(DcmTagKey(0x0029, 0x1020), (const Uint8*&)tempBuf, &returnedBytes, OFFalse);
	
        std::string siemensPrivateField;
        
	if (returnedBytes > 0)
	{
		// removes non-printable characters: keep newline character and the ones between 0x32 (space) and 0x7E (tilde)
		// cycles through the returned array using a pointer increment
		for (unsigned char *curChar = tempBuf; curChar < tempBuf + returnedBytes - 1; curChar++)
			if (*curChar != '\n'&& (*curChar < ' ' || *curChar >  '~'))
				*curChar = ' ';
		
		tempBuf[returnedBytes-1] = 0; // terminate byte array with a \0 character
		
		siemensPrivateField = (char *)tempBuf; // convert byte array into string. 
		
		delete tempBuf; //free memory
	}
	else // no field returned
	{
		siemensPrivateField = "";
	}

	//cout << ss.str() << endl;

	QApplication a( argc, argv );

	// main widget
	MyMainWidget mainWidget;
	
	//a.setMainWidget(&mainWidget);
	// hbox layout for the main widget
	QHBoxLayout hbox(&mainWidget);
	
	MyLabel dicomDisplayLabel(0, "Dicom Display Label");
	dicomDisplayLabel.setScaledContents(true);
	// loads image in pixmap
	DcmPixmap dcm(dcmname);
	dicomDisplayLabel.setPixmap(dcm);
        
        mainWidget.setLabel(&dicomDisplayLabel);
        
	QSizePolicy labelPolicy(QSizePolicy::Preferred,QSizePolicy::Ignored);
	labelPolicy.setHeightForWidth(true);
	dicomDisplayLabel.setSizePolicy(labelPolicy);
	hbox.addWidget(&dicomDisplayLabel);

	// Layout on the right
	QVBoxLayout vbox;
	// dicom info text edit
	QTextEdit dicomInfoEdit;
	dicomInfoEdit.setReadOnly(true); // we don't want to modify it
	dicomInfoEdit.setText(QString::fromStdString(dicomInfoStringStream.str() + "\n --- Siemens header ---\n" + siemensPrivateField));
	vbox.addWidget(&dicomInfoEdit);

	// search box
	FindStringLineEdit findEdit(&dicomInfoEdit);
	vbox.addWidget(&findEdit);

	hbox.addLayout(&vbox);

	mainWidget.show();
	
	return a.exec();
}
