/******************************************************************************
*   nemesis. an experimental finite element code.                             *
*   Copyright (C) 2004-2007 F.E.Karaoulanis [http://www.nemesis-project.org]  *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License version 2, as        *
*   published by the Free Software Foundation.                                *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License along   *
*   with this program; if not, write to the Free Software Foundation, Inc.,   *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.               *
******************************************************************************/

//*****************************************************************************
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$
// $HeadURL$
// Author(s): F.E. Karaoulanis (fkar@nemesis-project.org)
//*****************************************************************************

#include <GroundMotionSin.h>

GroundMotionSin::GroundMotionSin()
{
}
GroundMotionSin::GroundMotionSin(int dof_,double a_,double omega_,double phi_)
	:Load()
{
	dof=dof_-1;
	a=a_;
	omega=omega_;
	phi=phi_;
}
void GroundMotionSin::apply(double fact,double time)
{
	const std::map<int,Element*>& c=pD->getElements();
	std::map<int,Element*>::const_iterator i;
	for(i=c.begin();i!=c.end();i++) i->second->addGroundMotion(dof,a*sin(omega*time+phi));
}
