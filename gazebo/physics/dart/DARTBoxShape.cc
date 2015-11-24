/*
 * Copyright (C) 2014-2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "gazebo/common/Console.hh"
#include "gazebo/physics/dart/DARTPhysics.hh"
#include "gazebo/physics/dart/DARTCollision.hh"
#include "gazebo/physics/dart/DARTBoxShape.hh"
#include "gazebo/util/system.hh"

#include "gazebo/physics/dart/DARTBoxShapePrivate.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
DARTBoxShape::DARTBoxShape(DARTCollisionPtr _parent)
  : BoxShape(_parent),
    dataPtr(new DARTBoxShapePrivate())
{
}

//////////////////////////////////////////////////
DARTBoxShape::~DARTBoxShape()
{
  delete this->dataPtr;
}

//////////////////////////////////////////////////
void DARTBoxShape::SetSize(const ignition::math::Vector3d &_size)
{
  if (_size.Min() < 0)
  {
    gzerr << "Box shape does not support negative size\n";
    return;
  }
  ignition::math::Vector3d size = _size;
  if (ignition::math::equal(size.X(), 0.0))
  {
    // Warn user, but still create shape with very small value
    // otherwise later resize operations using setLocalScaling
    // will not be possible
    gzwarn << "Setting box shape's x to zero is not supported in DART, "
           << "using 1e-4.\n";
    size.X(1e-4);
  }

  if (ignition::math::equal(size.Y(), 0.0))
  {
    gzwarn << "Setting box shape's y to zero is not supported in DART, "
           << "using 1e-4.\n";
    size.Y(1e-4);
  }

  if (ignition::math::equal(size.Z(), 0.0))
  {
    gzwarn << "Setting box shape's z to zero is not supported in DART "
           << "using 1e-4.\n";
    size.Z(1e-4);
  }

  BoxShape::SetSize(size);

  DARTCollisionPtr dartCollisionParent =
      boost::dynamic_pointer_cast<DARTCollision>(this->collisionParent);

  if (dartCollisionParent->GetDARTCollisionShape() == NULL)
  {
    dart::dynamics::BodyNode *dtBodyNode =
        dartCollisionParent->GetDARTBodyNode();
    dart::dynamics::BoxShape *dtBoxShape =
        new dart::dynamics::BoxShape(DARTTypes::ConvVec3(size));
    dtBodyNode->addCollisionShape(dtBoxShape);
    dartCollisionParent->SetDARTCollisionShape(dtBoxShape);
  }
  else
  {
    dart::dynamics::BoxShape *dtBoxShape =
        dynamic_cast<dart::dynamics::BoxShape*>(
          dartCollisionParent->GetDARTCollisionShape());
    dtBoxShape->setSize(DARTTypes::ConvVec3(size));
  }
}
