#ifndef ADJUSTABLEPHYSICS2D_COMPONENTS_H
#define ADJUSTABLEPHYSICS2D_COMPONENTS_H

#include "../../common/Shape.h"
#include "../../common/MassInfo.h"
#include "../../common/Material.h"
#include "../../common/Polygon.h"
#include "../../common/Transform.h"
#include "Component.h"
#include "ComponentType.h"

struct ShapeComponent : Shape, Component<Shape, ComponentType::Shape>
{ };

struct MassInfoComponent : MassInfo, Component<MassInfo, ComponentType::MassInfo>
{ };

struct MaterialComponent : Material, Component<Material, ComponentType::Material>
{ };

struct PolygonComponent : Polygon, Component<Polygon, ComponentType::Polygon>
{ };

struct LocationComponent : Transform, Component<Transform, ComponentType::Location>
{ };

struct VelocityComponent : Transform, Component<Transform, ComponentType::Velocity>
{ };

struct AccelerationComponent : Transform, Component<Transform, ComponentType::Acceleration>
{ };

struct ForceComponent : Transform, Component<Transform, ComponentType::Force>
{ };
#endif //ADJUSTABLEPHYSICS2D_COMPONENTS_H
