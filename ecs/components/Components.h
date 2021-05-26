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
{
    using Shape::Shape;
};

struct MassInfoComponent : MassInfo, Component<MassInfoComponent, ComponentType::MassInfo>
{
    using MassInfo::MassInfo;
};

struct MaterialComponent : Material, Component<MaterialComponent, ComponentType::Material>
{
    using Material::Material;
};

struct PolygonComponent : Polygon, Component<PolygonComponent, ComponentType::Polygon>
{
    using Polygon::Polygon;
};

struct LocationComponent : Transform, Component<LocationComponent, ComponentType::Location>
{
    using Transform::Transform;
};

struct VelocityComponent : Transform, Component<VelocityComponent, ComponentType::Velocity>
{
    using Transform::Transform;
};

struct AccelerationComponent : Transform, Component<AccelerationComponent, ComponentType::Acceleration>
{
    using Transform::Transform;
};

struct ForceComponent : Transform, Component<ForceComponent, ComponentType::Force>
{
    using Transform::Transform;
};
#endif //ADJUSTABLEPHYSICS2D_COMPONENTS_H
