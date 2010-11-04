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

#ifndef SRC_LOADCASE_GROUND_MOTION_SIN_H_
#define SRC_LOADCASE_GROUND_MOTION_SIN_H_

#include "elements/element.h"
#include "loadcase/load.h"

class GroundMotionSin: public Load {
 protected:
  int dof_;
  double a_;
  double omega_;
  double phi_;
 public:
  GroundMotionSin();
  GroundMotionSin(int dof, double a, double omega, double phi = 0.);

  void apply(double fact, double time);
};

#endif  // SRC_LOADCASE_GROUND_MOTION_SIN_H_
