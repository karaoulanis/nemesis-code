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
//            C.G. Panagiotopoulos (pchr@civil.auth.gr)
//*****************************************************************************

#include "analysis/EigenAnalysis.h"

EigenAnalysis::EigenAnalysis()
	:AnalysisType()
{
	myTag=TAG_NONE;
	// defaults
	pA->setImposer(new EliminationImposer());
	pA->setControl(new EigenControl());
	pA->setSOE(new EigenSOE());
}
EigenAnalysis::~EigenAnalysis()
{
}
bool EigenAnalysis::checkIfAllows(FEObject* f)
{
	return false;
}
int EigenAnalysis::run(int nLC,int nLoadSteps)
{
	// Create model by applying the constraints
	pA->getImposer()->impose();

	// Now that model is complete, reorder the model
	if(pA->getReorderer()!=0) pA->getReorderer()->reorder();

	// Now that model is complete, the SOE can be initialized
	pA->getSOE()->setTheSize();
	
	// Initialize the control
	pA->getControl()->formTangent();
//	pA->getSOE()->print();
	pA->getSOE()->solve();
	pA->getDomain()->setEigenValues(pA->getSOE()->getX());

	return 0;
}
