/*******************************************************************************
* nemesis. an experimental finite element code.                                *
* Copyright (C) 2004-2010 F.E.Karaoulanis [http://www.nemesis-project.org]     *
*                                                                              *
* This program is free software; you can redistribute it and/or modify         *
* it under the terms of the GNU General Public License version 3, as           *
* published by the Free Software Foundation.                                   *
*                                                                              *
* This program is distributed in the hope that it will be useful,              *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU General Public License for more details.                                 *
*                                                                              *
* You should have received a copy of the GNU General Public License            *
* along with this program.  If not, see < http://www.gnu.org/licenses/>.        *
*******************************************************************************/

// *****************************************************************************
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$
// $HeadURL$
// Author(s): F.E. Karaoulanis (fkar@nemesis-project.org)
// *****************************************************************************

#include "model/lagrange_model_element.h"
#include "main/nemesis_debug.h"

/**
 * Default constructor
 */
LagrangeModelElement::LagrangeModelElement()
  :ModelElement() {
}
/**
 * Constructor
 */
LagrangeModelElement::LagrangeModelElement(const IDContainer& FTable,
                       Constraint* pConstraint)
  :ModelElement(FTable, 0, pConstraint) {
  ///@todo check for maximum number of the available matrices
  myMatrix = theStaticMatrices[FTable.size()];
  myVector = theStaticVectors[FTable.size()];
}
/**
 * Destructor
 */
LagrangeModelElement::~LagrangeModelElement() {
}
/**
 * 
 */
void LagrangeModelElement::add_K(double /*factor*/) {
  myMatrix->clear();
  for (unsigned i = 0; i < theFTable.size()-1; i++) {
    double ci = myConstraint->getcDof(i).coeff;
    (*myMatrix)(i, theFTable.size()-1)=ci;
  }
  for (unsigned i = 0; i < theFTable.size()-1; i++) {
    double ci = myConstraint->getcDof(i).coeff;
    (*myMatrix)(theFTable.size()-1, i)=ci;
  }
}
/**
 * 
 */
void LagrangeModelElement::add_M(double /*factor*/) {
}
/**
 * 
 */
void LagrangeModelElement::add_C(double /*factor*/) {
}
/**
 * 
 */
void LagrangeModelElement::add_R(double /*factor*/) {
  ///@todo Make this work for the non-linear multi constraint
  // Find constraint violation
  double c = myConstraint->getcVal();
  double cv = 0.;
  for (unsigned i = 0; i < theFTable.size()-1; i++) {
    double ci = myConstraint->getcDof(i).coeff;
    double ui = myConstraint->getDisp(i);
    cv+=ci*ui;
  }
  cv-=c;
  for (unsigned i = 0; i < theFTable.size()-1; i++) {
    double ci = myConstraint->getcDof(i).coeff;
    double Fi = myConstraint->getF();
    (*myVector)[i]=ci*Fi;
  }
  (*myVector)[theFTable.size()-1]=cv;
}
/**
 * 
 */
void LagrangeModelElement::add_Reff(double /*factor*/) {
  ///@todo Make this work for the non-linear multi constraint
  double c = myConstraint->getcVal();
  double u = myConstraint->getDisp(0); //wrong
  for (unsigned i = 0;i < theFTable.size()-1;i++) (*myVector)[i]=myConstraint->getF();
  (*myVector)[theFTable.size()-1]=u-c;
}
