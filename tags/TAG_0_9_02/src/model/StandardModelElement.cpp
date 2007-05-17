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


#include <StandardModelElement.h>

//=============================================================================
// Constructors
//=============================================================================
/**
 * Default constructor.
 * Initializes only the ModelObject, which in turn initializes the FEObject.
 */
StandardModelElement::StandardModelElement()
	:ModelElement()
{
}
/**
 * Constructor.
 * Initializes the ModelObject, which in turn initializes the FEObject, passes
 * the FTable to the ModelObject and copies the address of it's element.
 */
StandardModelElement::StandardModelElement(const IDContainer& FTable,Element* pElement)
	:ModelElement(FTable,pElement,0)
{
	myMatrix=theStaticMatrices[FTable.size()];
	myVector=theStaticVectors[FTable.size()];
}
StandardModelElement::~StandardModelElement()
{
}
void StandardModelElement::add_K(double factor)
{
	myMatrix->add_cM(factor,myElement->getK());
}
void StandardModelElement::add_M(double factor)
{
	myMatrix->add_cM(factor,myElement->getM());
}
void StandardModelElement::add_C(double factor)
{
	myMatrix->add_cM(factor,myElement->getC());
}
void StandardModelElement::add_R(double factor)
{
	myVector->add_cV(factor,myElement->getR());
}
void StandardModelElement::add_Reff(double factor)
{
	myVector->add_cV(factor,myElement->getReff());
}
void StandardModelElement::add_Rgrad(double factor)
{
	myVector->add_cV(factor,myElement->getRgrad());
}
