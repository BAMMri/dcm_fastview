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


#include "findStringLineEdit.h"
#include <QTextCursor>
#include <QMessageBox>
#include <QTextDocument>
#include <QKeyEvent>

FindStringLineEdit::FindStringLineEdit(QTextEdit *myTextEdit) : QLineEdit()
{
	this->myTextEdit = myTextEdit;
}

void FindStringLineEdit::keyPressEvent(QKeyEvent *evt)
{
	QLineEdit::keyPressEvent(evt);
	if (evt->key() == Qt::Key_Return) // if return was pressed and there is some text then search for it
		if (text().length() > 0)
		{
                    if (evt->modifiers() & Qt::ShiftModifier)
                    { // search backwards if shift is pressed
                        QTextCursor curCursor = myTextEdit->textCursor();
                        if (!curCursor.atStart())
                        {
                            // move cursor at the start of the word, in case a word is already selected. This might 
                            curCursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
                        }
                        if (!myTextEdit->find(text(), QTextDocument::FindBackward))
                        {
                            QTextCursor curCursor = myTextEdit->textCursor();
                            curCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
                            myTextEdit->setTextCursor(curCursor);
                            if (!myTextEdit->find(text(), QTextDocument::FindBackward))
                                    QMessageBox::warning(this, "Warning", "String not found");
                        }
                            
                    } else
                    {
			if (!myTextEdit->find(text()))
			{ // if the text is not found, search again from beginning
				QTextCursor curCursor = myTextEdit->textCursor();
				curCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
				myTextEdit->setTextCursor(curCursor);
				if (!myTextEdit->find(text()))
					QMessageBox::warning(this, "Warning", "String not found");
			}
                    }
		}
}
