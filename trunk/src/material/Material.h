/******************************************************************************
*   nemesis. an experimental finite element code.                             *
*   Copyright (C) 2004-2007 F.E.Karaoulanis [http://www.nemesis-project.org]  *
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

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <Domain.h>
#include <DomainObject.h>
#include <Vector.h>
#include <Tracker.h>

class Domain;

/**
 * The Material Class.                                                
 */
class Material: public DomainObject
{
protected:
	static int counter;
	int index;
	Vector MatParams;
	Tracker* myTracker;
public:
	Material();
	Material(int ID,double rho,double aT);
	virtual ~Material();

	inline void   setParam(int i,double d)	{MatParams[i]=d;}
	inline double getParam(int i)			{return MatParams[i];}
	inline double getRho()					{return MatParams[30];}
	inline double getaT()					{return MatParams[31];}
	virtual void commit()=0;

	// Tracker member functions
	void addTracker();
	Tracker* getTracker();
	virtual void track();
};

#endif
