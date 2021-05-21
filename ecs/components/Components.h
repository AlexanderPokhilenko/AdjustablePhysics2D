#ifndef ADJUSTABLEPHYSICS2D_COMPONENTS_H
#define ADJUSTABLEPHYSICS2D_COMPONENTS_H

#include "../../common/Shape.h"
#include "../../common/MassInfo.h"
#include "../../common/Material.h"
#include "../../common/Polygon.h"
#include "../../common/Transform.h"
#include "Component.h"
#include "ComponentType.h"

struct ShapeComponent : Shape, Component<ShapeComponent, ComponentType::Shape>
{ };

struct MassInfoComponent : MassInfo, Component<MassInfoComponent, ComponentType::MassInfo>
{ };

struct MaterialComponent : Material, Component<MaterialComponent, ComponentType::Material>
{ };

struct PolygonComponent : Polygon, Component<PolygonComponent, ComponentType::Polygon>
{ };

struct LocationComponent : Transform, Component<LocationComponent, ComponentType::Location>
{ };

struct VelocityComponent : Transform, Component<VelocityComponent, ComponentType::Velocity>
{ };

struct AccelerationComponent : Transform, Component<AccelerationComponent, ComponentType::Acceleration>
{ };

struct ForceComponent : Transform, Component<ForceComponent, ComponentType::Force>
{ };
#endif //ADJUSTABLEPHYSICS2D_COMPONENTS_H
