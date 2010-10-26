/******************************************************************************
*   nemesis. an experimental finite element code.                             *
*   Copyright (C) 2004-2010 F.E.Karaoulanis [http://www.nemesis-project.org]  *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License version 3, as        *
*   published by the Free Software Foundation.                                *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
******************************************************************************/

//*****************************************************************************
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$
// $HeadURL$
// Author(s): F.E. Karaoulanis (fkar@nemesis-project.org)
//*****************************************************************************

#ifndef _QUAD4B_H
#define _QUAD4B_H

#include "elements/Quad4.h"

class Quad4b: public Quad4
{
private:
	static double shp[4][3][4];
	static double detJ[4];
	static std::vector<int> perm;
public:
	// Constructors and Destructor
	Quad4b();
	Quad4b(int ID,int Node_1,int Node_2,int Node_3,int Node_4,int MatID);
	~Quad4b();
	
	const Matrix& getK();
    const Matrix& getM();
	const Vector& getR();
	void getB(Matrix& B,int node,int gPoint);
	void shapeFunctions();
	void update();
};

#endif
