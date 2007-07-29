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

#include <MultiaxialElastoPlastic.h>
#include <NemesisDebug.h>

Matrix MultiaxialElastoPlastic::C(6,6,0.);

MultiaxialElastoPlastic::MultiaxialElastoPlastic()
{
}
MultiaxialElastoPlastic::MultiaxialElastoPlastic(int ID,int elasticID)
:MultiaxialMaterial(ID,0.,0.)
{
	// Get the elastic part
	Material* p=pD->get<Material>(pD->getMaterials(),elasticID);
	if(p->getTag()!=TAG_MATERIAL_MULTIAXIAL_ELASTIC)
		throw SException("[nemesis:%d] %s",9999,"Multiaxial elastic material expected.");
	myElastic=static_cast<MultiaxialMaterial*>(p)->getClone();
	MatParams[30]=myElastic->getParam(30);
	MatParams[31]=myElastic->getParam(31);

	// Material state
	eTrial.resize(6,0.);
	ePTrial.resize(6,0.);	ePConvg.resize(6,0.);
	qTrial.resize(6,0.);	qConvg.resize(6,0.);
	aTrial=0.;				aConvg=0.;
	plastic=false;
	nHardeningVariables=0;
}
MultiaxialElastoPlastic::~MultiaxialElastoPlastic()
{
	delete myElastic;
	Containers::vector_delete(fSurfaces);
	Containers::vector_delete(gSurfaces);
}
/**
 * Update stresses given a total strain increment.
 * @param De Vector containing total strain increment.
 */ 
void MultiaxialElastoPlastic::setStrain(const Vector& De)
{
	this->returnMapSYS(De);
	//this->returnMapTest(De);
	//this->returnMapMYS2(De);
	//if(fSurfaces.size()==1)	this->returnMapSYS(De);
	//if(fSurfaces.size()==1)	this->returnMapTest(De);
	//else					this->returnMapMYS(De);
}
/**
 * Commit material state.
 */
void MultiaxialElastoPlastic::commit()
{
	//eTotal=eTrial; ///@todo
	ePConvg=ePTrial;
	sConvg=sTrial;
	qConvg=qTrial;
	aConvg=aTrial;
	this->track();
}
void MultiaxialElastoPlastic::returnMapTest(const Vector& De)
{
	static LogFile log("TYS.log");

	int nIter=20;
	double tol1=1e-6;
	double tol2=1e-6;
	Vector R(7,0.);
	Matrix Cel=myElastic->getC();
	Matrix invCel=Inverse(Cel);
	Matrix A(7,7,0.);
	ePTrial=ePConvg;
	aTrial=aConvg;
	double dg=0;
	Surface* fS=fSurfaces[0];
	Surface* gS=gSurfaces[0];
	double eta=1000.;
	double dt=pD->getTimeIncr();
	
	//=========================================================================
	// Step 1: Compute trial stress
	//=========================================================================
	sTrial=sConvg+Cel*De;
	Vector ss(6,0.);
	ss=sTrial;
	eTotal+=De;
	
	//=========================================================================
	// Step 2: Check for plastic process
	//=========================================================================
	if(fS->get_f(sTrial,ePTrial)<=0) return;
 	for(int k=0;k<nIter;k++)
	{
		// Residual Vector 
		Vector tmp(6,0.);
		tmp=De;
		tmp-=invCel*(sTrial-sConvg);
		tmp-=dg*(gS->get_dfds(sTrial,ePTrial));
		R.clear();
		R.append(tmp,0);
		R[6]=-(fS->get_f(sTrial,ePTrial));
		// Convergence check
		if(tmp.twonorm()<tol1 && abs(fS->get_f(sTrial,ePTrial))<tol2)
		{
			aTrial+=dg;
			break;
		}
		// Jacobian
		A.clear();
		A.append(invCel+dg*(gS->get_df2dss(sTrial,ePTrial)),0,0);
		for(int i=0;i<6;i++) A(6,i)=(fS->get_dfds(sTrial,ePTrial))[i];
		for(int i=0;i<6;i++) A(i,6)=(gS->get_dfds(sTrial,ePTrial))[i];
		Vector x(7,0.);
		A.solve(x,R);
		for(int i=0;i<6;i++) sTrial[i]+=x[i];
		dg+=x[6];
	}
//	sTrial=(ss+(dt/eta)*sTrial)/(1+(dt/eta));
}
/**
 * Single surface return mapping.
 * @param De Vector containing total strain increment.
 */
void MultiaxialElastoPlastic::returnMapSYS(const Vector& De)
{
	static LogFile log("SYS.log");
	//=========================================================================
	// Setup
	//=========================================================================
	int nIter=25;
	double tol1=1e-6;
	double tol2=1e-6;
	double ddg;
	Vector R(6,0.);
	Vector dEp(6,0.);
	Matrix Cel   =myElastic->getC();
	Matrix invCel=Inverse(Cel);
	
	ePTrial=ePConvg;
	aTrial=aConvg;

	//double& dg=aTrial[0];
	double dg=0.;
	Surface* fS=fSurfaces[0];
	Surface* gS=gSurfaces[0];
	//Surface* gS=gSurfaces[0];
	
	Vector enn=invCel*sConvg+ePConvg+De;
	eTotal+=De;
	//cout<<eTotal[1]<<'\t'<<(invCel*sConvg+ePConvg+De)[1]<<endl;
	//cout<<eTotal[1]<<endl;
	//Vector enn=eTotal;

	//=========================================================================
	// Step 1: Compute trial stress
	//=========================================================================
	Vector ss=sTrial;
	//sTrial=sConvg+Cel*De;
	sTrial=Cel*enn;
 
	//=========================================================================
	// Step 2: Check for plastic process
	//=========================================================================
	if(fS->get_f(sTrial,ePTrial)<=0) return;

	for(int k=0;k<nIter;k++)
	{
		//=====================================================================
		// Step 3: Evaluate flow rule, hardening law and residuals
		//=====================================================================
		sTrial=Cel*(enn-ePTrial);
		R=-ePTrial+ePConvg+dg*(gS->get_dfds(sTrial,ePTrial));
		//cout<<ePConvg[0]<<'\t'<<ePTrial[0]<<endl;
		//cout<<sTrial<<endl;
		//cout<<endl;
		//cout<<gS->get_df2dss(sTrial)<<endl;


		//=====================================================================
		// Step 4: Check convergence
		//=====================================================================
		if(R.twonorm()<tol1 && abs(fS->get_f(sTrial,ePTrial))<tol2) break;

		//=====================================================================
		// Step 5: Compute elastic moduli and consistent tangent moduli
		//=====================================================================
		// Matrix A 
		Matrix A(6+nHardeningVariables,6+nHardeningVariables,0.);
		A.append(invCel+dg*(gS->get_df2dss(sTrial,ePTrial)),0,0);
		if(nHardeningVariables>0)
		{
			Vector v=dg*(gS->get_df2dsq(sTrial,ePTrial));
			for(int i=0;i<6;i++) A(i,6)=v[i];
			for(int i=0;i<6;i++) A(6,i)=v[i];
			A(7,7)=dg*(gS->get_H(sTrial,ePTrial)+gS->get_df2dqq(sTrial,ePTrial));
		}
		A=Inverse(A);

		//=====================================================================
		// Step 6: Obtain increment to consistency parameter
		//=====================================================================
		ddg=(fS->get_f(sTrial,ePTrial)-fS->get_dfds(sTrial,ePTrial)*(A*R))/(fS->get_dfds(sTrial,ePTrial)*(A*(gS->get_dfds(sTrial,ePTrial))));

		//=====================================================================
		// Step 7: Obtain incremental plastic strains and internal variables
		//=====================================================================
		dEp=invCel*A*(R+ddg*(gS->get_dfds(sTrial,ePTrial)));

		//=====================================================================
		// Step 8: Update
		//=====================================================================
		ePTrial+=dEp;
		dg+=ddg;
		//cout<<sTrial.theta()<<endl;
		//cout<<((fS->get_dfds(sTrial)))<<endl;
	}
	//cout<<k<<endl;

	//cout<<endl;
	//double dt=pD->getTimeIncr();
	//double eta=1000.;
	//sTrial=((sConvg+Cel*De)+(dt/eta)*(sConvg+Cel*De))/(1+(dt/eta));
	//sTrial=(ss+(dt/eta)*sTrial)/(1+(dt/eta));
	//cout<<sConvg<<endl;
	//cout<<sConvg.I1()<<endl;

}
/**
 * Multiple surface return mapping.
 * @param De Vector containing total strain increment.
 */
void MultiaxialElastoPlastic::returnMapMYS(const Vector& De)
{
	//=========================================================================
	// Setup
	//=========================================================================
	int nIter=15;
	double tol1=1e-6;
	double tol2=1e-6;
	Vector R(6,0.);
	Vector dEp(6,0.);
	Matrix Cel   =myElastic->getC();
	Matrix invCel=Inverse(Cel);
	Vector ddg(12,0.);
	Vector dg(12,0.);
	
	ePTrial=ePConvg;//todo:CHECK!!!!
	//aTrial=aConvg;	//todo:CHECK!!!!
	//aTrial.clear();	//todo:CHECK!!!!
	eTotal+=De;
	Vector enn=invCel*sConvg+ePConvg+De;
	//Vector enn=eTotal;
	//=========================================================================
	// Step 1: Compute trial stress
	//=========================================================================
	//sTrial=sConvg+Cel*De;
	sTrial=Cel*(enn-ePTrial);
	Vector ss=sTrial;
	plastic=false;
   
	//=========================================================================
	// Step 2: Check for plastic process
	//=========================================================================
	int nActiveSurfaces=0;
	for(unsigned i=0;i<fSurfaces.size();i++)
		if(fSurfaces[i]->get_f(sTrial,ePTrial)>0)
		{
			fSurfaces[i]->setActive(true);
			nActiveSurfaces++;
		}
	
	if(nActiveSurfaces==0) return;
	//cout<<nActiveSurfaces<<endl;
	
	plastic=true;
	cout<<endl;
	for(int k=0;k<nIter;k++)
	{
		//=====================================================================
		// Step 3: Evaluate flow rule, hardening law and residuals
		//=====================================================================
		sTrial=Cel*(enn-ePTrial);
		R=-ePTrial+ePConvg;
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive())
				R+=dg[b]*(fSurfaces[b]->get_dfds(sTrial,ePTrial));

		nActiveSurfaces=0;
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive()) 
				nActiveSurfaces++;
		cout<<"active : "<<nActiveSurfaces<<endl;

		//=====================================================================
		// Step 4: Check convergence
		//=====================================================================
		bool converged=false;
		if(R.twonorm()>tol1) converged=false;
		for(unsigned a=0;a<fSurfaces.size();a++)
			if(fSurfaces[a]->isActive() && abs(fSurfaces[a]->get_f(sTrial,ePTrial))<tol2)
				converged=true;
		if(converged) break;

		//=====================================================================
		// Step 5: Compute elastic moduli and consistent tangent moduli
		//=====================================================================
		// Matrix A 
		Matrix A(6,6,0.);
		A.append(invCel,0,0);
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive())
				A+=dg[b]*(fSurfaces[b]->get_df2dss(sTrial,ePTrial));
		A=Inverse(A);

		// Matrix G
		Matrix Gab(4,4,0.);
		Gab(0,0)=1.0;Gab(1,1)=1.0;Gab(2,2)=1.0;Gab(3,3)=1.0;
		for(unsigned a=0;a<fSurfaces.size();a++)
			for(unsigned b=0;b<fSurfaces.size();b++)
				if(fSurfaces[a]->isActive() && fSurfaces[b]->isActive())
				{
					Vector va(6,0.);
					Vector vb(6,0.);
					va=fSurfaces[a]->get_dfds(sTrial,ePTrial);
					vb=fSurfaces[b]->get_dfds(sTrial,ePTrial);
	                Gab(a,b)=(va*(A*vb));
					//cout<<sTrial<<endl;
					//cout<<va<<endl;
					//cout<<vb<<endl;
					//cout<<A<<endl;
				}
		cout<<Gab<<endl;
		Gab=Inverse(Gab);

		//=====================================================================
		// Step 6: Obtain increment to consistency parameter
		//=====================================================================
		ddg.clear();
		for(unsigned a=0;a<fSurfaces.size();a++)
			for(unsigned b=0;b<fSurfaces.size();b++)
				if(fSurfaces[a]->isActive() && fSurfaces[b]->isActive())
				{
					Vector vb=fSurfaces[b]->get_dfds(sTrial,ePTrial);
					double fb=fSurfaces[b]->get_f(sTrial,ePTrial);
					ddg[a]+=Gab(a,b)*(fb-vb*(A*R));					
				}

		bool reset=false;
		for(unsigned a=0;a<fSurfaces.size();a++)
		{
			//cout<<"a = "<<a<<"  "<<dg[a]+ddg[a]<<endl;
			if(fSurfaces[a]->isActive()&& dg[a]+ddg[a]<0.)
			{
				fSurfaces[a]->setActive(false);
				dg[a]=0.;
				ddg[a]=0.;
				reset=true;
			}
		
		}
		if(reset) continue;
		cout<<k<<" dg "<<ddg<<endl;

		//=====================================================================
		// Step 7: Obtain incremental plastic strains and internal variables
		//=====================================================================
		static Vector temp(6);
		temp.clear();
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive())
				temp+=ddg[b]*(fSurfaces[b]->get_dfds(sTrial,ePTrial));

		//=====================================================================
		// Step 8: Update
		//=====================================================================
		dEp=invCel*A*(R+temp);
		ePTrial+=dEp;
		for(unsigned a=0;a<fSurfaces.size();a++)
			if(fSurfaces[a]->isActive()) dg[a]+=ddg[a];
	}
//	if(k==nIter) cout<<"FAILED"<<endl;
//	if(k==nIter)
//	{
//		cout.precision(12);
//		cout<<sConvg+Cel*De<<endl;
//		cout<<De<<endl;
//		cout<<sConvg<<endl;
//		cout<<ePConvg<<endl;
//		cout<<ePTrial<<endl;
//		cout<<"PASSED : "<<nActiveSurfaces<<endl;
//	}
	//cout<<endl;
	//double dt=pD->getTimeIncr();
	//double eta=1000.;
	//sTrial=((sConvg+Cel*De)+(dt/eta)*(sConvg+Cel*De))/(1+(dt/eta));
	//sTrial=(ss+(dt/eta)*sTrial)/(1+(dt/eta));
}
/**
 * Get tangent material matrix.
 * @todo fill it
 * @return A reference to the tangent material matrix.
 */
const Matrix& MultiaxialElastoPlastic::getC()
{
	return myElastic->getC();
}
/**
 * Add a record to the tracker.
 * If \a myTracker pointer is null (no tracker is added) just return.
 * Otherwise gather info and send them to the tracker.
 * The domain should be already updated!
 */
void MultiaxialElastoPlastic::track()
{
	if(myTracker==0) return;
	ostringstream s;
	s<<"DATA "	<<' ';
	s<<"sigm "	<<' '<<sConvg;
	s<<"epst "	<<' '<<eTotal;
	s<<"epsp "	<<' '<<ePConvg;
	s<<"epsv "	<<1020<<' '<<eTotal[0]+eTotal[1]+eTotal[2]<<' ';
	s<<"END "<<' ';
	myTracker->track(pD->getLambda(),pD->getTimeCurr(),s.str());
}



void MultiaxialElastoPlastic::returnMapMYS2(const Vector& De)
{
	//=========================================================================
	// Setup
	//=========================================================================
	int nIter=25;
	double tol1=1e-6;
	double tol2=1e-6;
	Vector R(6,0.);
	Vector dEp(6,0.);
	Matrix Cel   =myElastic->getC();
	Matrix invCel=Inverse(Cel);
	Vector dg(12,0.);
	
	ePTrial=ePConvg;//todo:CHECK!!!!
	//aTrial=aConvg;	//todo:CHECK!!!!
	//aTrial.clear();	//todo:CHECK!!!!
	eTrial=eTotal;
	eTrial+=De;
	Vector enn=invCel*sConvg+ePConvg+De;
	//=========================================================================
	// Step 1: Compute trial stress
	//=========================================================================
	//sTrial=sConvg+Cel*De;
	sTrial=Cel*(enn-ePTrial);
	Vector ss=sTrial;
	plastic=false;
   
	//=========================================================================
	// Step 2: Check for plastic process
	//=========================================================================
	int nActiveSurfaces=0;
	for(unsigned i=0;i<fSurfaces.size();i++)
		if(fSurfaces[i]->get_f(sTrial,ePTrial)>0)
		{
			fSurfaces[i]->setActive(true);
			nActiveSurfaces++;
		}
	
	if(nActiveSurfaces==0) return;
	//cout<<nActiveSurfaces<<endl;
	///cout<<enn-eTrial<<endl; ///@todo Check out which is better.
	
	plastic=true;
	///cout<<endl;
	for(int k=0;k<nIter;k++)
	{
		//=====================================================================
		// Step 3: Evaluate flow rule, hardening law and residuals
		//=====================================================================
		sTrial=Cel*(enn-ePTrial);
		R=-ePTrial+ePConvg;
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive())
				R+=dg[b]*(fSurfaces[b]->get_dfds(sTrial,ePTrial));

		nActiveSurfaces=0;
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive()) 
				nActiveSurfaces++;
		///cout<<"active : "<<nActiveSurfaces<<endl;

		//=====================================================================
		// Step 4: Check convergence
		//=====================================================================
		bool converged=false;
		if(R.twonorm()>tol1) converged=false;
		for(unsigned a=0;a<fSurfaces.size();a++)
			if(fSurfaces[a]->isActive() && abs(fSurfaces[a]->get_f(sTrial,ePTrial))<tol2)
				converged=true;
		if(converged) break;
		///cout<<"Res : "<<R.twonorm()<<' '<<abs(fSurfaces[0]->get_f(sTrial))<<endl;

		//=====================================================================
		// Step 5: Compute elastic moduli and consistent tangent moduli
		//=====================================================================
		// Matrix A 
		Matrix A(6,6,0.);
		A.append(invCel,0,0);
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive())
				A+=dg[b]*(fSurfaces[b]->get_df2dss(sTrial,ePTrial));
		A=Inverse(A);

		// Matrix G
		Matrix Gab(nActiveSurfaces,nActiveSurfaces,0.);
		for(unsigned a=0;a<fSurfaces.size();a++)
			for(unsigned b=0;b<fSurfaces.size();b++)
				if(fSurfaces[a]->isActive() && fSurfaces[b]->isActive())
				{
					Vector va(6,0.);
					Vector vb(6,0.);
					va=fSurfaces[a]->get_dfds(sTrial,ePTrial);
					vb=fSurfaces[b]->get_dfds(sTrial,ePTrial);
	                Gab(a,b)=(va*(A*vb));
					///cout<<a<<' '<<b<<endl;
					///cout<<sTrial<<endl;
					///cout<<va<<endl;
					///cout<<vb<<endl;
					//cout<<A<<endl;
				}

		//=====================================================================
		// Step 6: Obtain increment to consistency parameter
		//=====================================================================
		Vector ddg(nActiveSurfaces,0.);
		Vector rhs(nActiveSurfaces,0.);
		for(unsigned a=0;a<fSurfaces.size();a++)
			for(unsigned b=0;b<fSurfaces.size();b++)
				if(fSurfaces[a]->isActive() && fSurfaces[b]->isActive())
				{
					Vector vb=fSurfaces[b]->get_dfds(sTrial,ePTrial);
					double fb=fSurfaces[b]->get_f(sTrial,ePTrial);
					rhs[a]+=(fb-vb*(A*R));					
				}
		bool reset=false;
		if(abs(det(Gab))<1e-8)
		{
			fSurfaces[nActiveSurfaces-1]->setActive(false);
			reset=true;
		}
		///cout<<k<<"Gab "<<Gab<<endl;
		///cout<<k<<"rhs "<<rhs<<endl;
		///cout<<"det "<<det(Gab)<<endl;;
		Gab.solve(ddg,rhs);
		///cout<<k<<"ddg "<<ddg<<endl;

		for(unsigned a=0;a<fSurfaces.size();a++)
		{
			///cout<<"a = "<<a<<"  "<<dg[a]+ddg[a]<<endl;
			if(fSurfaces[a]->isActive()&& dg[a]+ddg[a]<0.)
			{
				fSurfaces[a]->setActive(false);
				dg[a]=0.;
				ddg[a]=0.;
				reset=true;
			}
		
		}
		if(reset) continue;

		//=====================================================================
		// Step 7: Obtain incremental plastic strains and internal variables
		//=====================================================================
		static Vector temp(6);
		temp.clear();
		for(unsigned b=0;b<fSurfaces.size();b++)
			if(fSurfaces[b]->isActive())
				temp+=ddg[b]*(fSurfaces[b]->get_dfds(sTrial,ePTrial));

		//=====================================================================
		// Step 8: Update
		//=====================================================================
		dEp=invCel*A*(R+temp);
		ePTrial+=dEp;
		for(unsigned a=0;a<fSurfaces.size();a++)
			if(fSurfaces[a]->isActive()) dg[a]+=ddg[a];
	}
//	if(k==nIter) cout<<"FAILED"<<endl;
}
