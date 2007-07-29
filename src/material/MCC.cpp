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

#include <MCC.h>

MCC::MCC()
{
}
MCC::MCC(double M_,double po_,double kappa_,double lambda_)
{
	M=M_;
	po=po_;
	kappa=kappa_;
	lambda=lambda_;
}
MCC::~MCC()
{
}
double MCC::get_f(const Vector& s,const Vector& e)
{
	double q=s.q();
	double p=s.p();
	double pc=po*exp(e.I1()/(kappa+lambda));
	//cout<<e.I1()<<endl;
	return q*q+M*M*p*(p-pc);
}
void MCC::find_C(const Vector& s,const Vector& e)
{
	C1=1/3.*M*M*(2/3.*s.I1()+po);
	C2=0.;
	C3=0.;
	C11=2./9.*M*M;
	C22=3.;
	C23=0.; C32=0.; C33=0.;
}
const double  MCC::get_dfdq(const Vector& s,const Vector& e)
{
	double dfda=(po/(kappa-lambda))*exp(e.I1()/(kappa-lambda));
	return num::d13*M*M*s.I1()*dfda;
}
const Vector& MCC::get_df2dsq(const Vector& s,const Vector& e)
{
	double dfda=(po/(kappa-lambda))*exp(e.I1()/(kappa-lambda));
	double c=num::d13*M*M*s.I1()*dfda;
	static Vector ret(6,0.);
	ret.clear();
	ret[0]=1.0; ret[1]=1.0; ret[2]=1.0;
	ret*=c;
    return ret;
}
const double  MCC::get_df2dqq(const Vector& s,const Vector& e)
{
	double dfda=(po/(kappa-lambda))*exp(e.I1()/(kappa-lambda));
	double d2fda2=(po/(kappa-lambda))*dfda;
	return num::d13*M*M*s.I1()*d2fda2;
}
const double  MCC::get_H(const Vector& s,const Vector& e)
{
	Vector r=this->get_dfds(s,e);
	double dfda=(po/(kappa-lambda))*exp(e.I1()/(kappa-lambda));
	return num::d13*M*M*dfda*(r[0]+r[1]+r[2]);
}
