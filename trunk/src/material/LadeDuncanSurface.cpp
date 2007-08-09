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

#include <LadeDuncanSurface.h>

LadeDuncanSurface::LadeDuncanSurface()
{
}
LadeDuncanSurface::LadeDuncanSurface(double K_)
{
	K=K_;
}
LadeDuncanSurface::~LadeDuncanSurface()
{
}
double LadeDuncanSurface::get_f(const Vector& s,const double kappa)
{
	//cout<<s<<endl;
	//cout<<s.I3()<<endl;
	double sI3=s.I3();
	if(abs(sI3)<1e-4) sI3=0.;
	double f=pow(1.*sI3,num::d13)-s.I1();
	cout<<sI3<<'\t'<<f<<endl;
	return f;
}
void LadeDuncanSurface::find_C(const Vector& s,const double a)
{
	C1=0.;
	C2=sqrt(0.75)/sqrt(s.J2());
	C3=0.;
	C11=0.;
	C22=-0.25*num::sq3*pow(s.J2(),-1.5);
	C23=0.; C32=0.; C33=0.;
}
const double  LadeDuncanSurface::get_dfda(const Vector& s,const double a)
{
	return -K;
}
const Vector& LadeDuncanSurface::get_df2dsa(const Vector& s,const double a)
{
	static Vector ret(6,0.);
	ret.clear();
    return ret;
}
const double  LadeDuncanSurface::get_df2daa(const Vector& s,const double a)
{
	return 0.;
}
