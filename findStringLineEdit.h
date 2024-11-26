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

#include <qlineedit.h>
#include <qtextedit.h>

class FindStringLineEdit : public QLineEdit
{
	public:
		FindStringLineEdit(QTextEdit *);
	protected: 
		// override
		virtual void keyPressEvent(QKeyEvent*);
	private:
		QTextEdit *myTextEdit;
};
