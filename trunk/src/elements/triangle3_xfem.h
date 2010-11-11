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
* along with this program.  If not, see < http://www.gnu.org/licenses/>.       *
*******************************************************************************/

// *****************************************************************************
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$
// $HeadURL$
// Author(s): F.E. Karaoulanis (fkar@nemesis-project.org)
// *****************************************************************************

#ifndef SRC_ELEMENTS_TRIANGLE3_XFEM_H_
#define SRC_ELEMENTS_TRIANGLE3_XFEM_H_

#include "elements/element.h"
#include "material/matpoint.h"

class Triangle3XFem: public Element {
 protected:
  double a1, a2, a3;
  double b1, b2, b3;
  double c1, c2, c3;
  double A;
  std::vector<MatPoint*> myMatPoints;
  public:
  // Constructors and Destructor
  Triangle3XFem();
  Triangle3XFem(int ID, int Node_1, int Node_2, int Node_3, int matID);
  ~Triangle3XFem();

  const Matrix& get_K();
  const Matrix& get_M();
  const Vector& get_R();

  void update();
  void commit();
  
  void addInitialStresses(InitialStresses* pInitialStresses);
  bool checkIfAllows(FEObject* f);
  void recoverStresses();
  int get_num_plastic_points();

  void enrich();
};

#endif  // SRC_ELEMENTS_TRIANGLE3_XFEM_H_
