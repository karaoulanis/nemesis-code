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

#include <SDofElement.h>

/**
 * Default constructor.
 */
SDofElement::SDofElement()	
{
}
/**
 * Constructor.
 */
SDofElement::SDofElement(int ID,int NodeID,int dofID,int matID)	
	:Element(ID,matID)
{
	// The dofs needed for this element
	myNodalIDs.resize(1);
	myNodalIDs[0]=NodeID;
	myLocalNodalDofs.resize(1);
	myLocalNodalDofs[0]=dofID-1;
	// Handle common info
    this->handleCommonInfo();
	mySDofMaterial=static_cast<SDofMaterial*>(myMaterial)->getClone();
}
SDofElement::~SDofElement()
{
}
const Matrix& SDofElement::getK()
{
	Matrix& K=*myMatrix;
	double facK=1e-7;
	if(myGroup->isActive()) facK=myGroup->getFacK();
	K(0,0)=facK*(mySDofMaterial->getParam(0));
	return K;
}
const Matrix& SDofElement::getM()
{
	Matrix& M=*myMatrix;
	M(0,0)=mySDofMaterial->getRho();
	return M;
}
const Vector& SDofElement::getR()
{	
	Vector& R=*myVector;
	R.clear();
	return R;
}
bool SDofElement::checkIfAllows(FEObject* f)
{
	return true;
}
