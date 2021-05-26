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
    ShapeComponent(const Shape& shape) : Shape(shape) { };
};

struct MassInfoComponent : MassInfo, Component<MassInfoComponent, ComponentType::MassInfo>
{
    using MassInfo::MassInfo;
    MassInfoComponent(const MassInfo& massInfo) : MassInfo(massInfo) { };
};

struct MaterialComponent : Material, Component<MaterialComponent, ComponentType::Material>
{
    using Material::Material;
    MaterialComponent(const Material& material) : Material(material) { };
};

struct PolygonComponent : Polygon, Component<PolygonComponent, ComponentType::Polygon>
{
    using Polygon::Polygon;
    PolygonComponent(const Polygon& polygon) : Polygon(polygon) { };
};

struct LocationComponent : Transform, Component<LocationComponent, ComponentType::Location>
{
    using Transform::Transform;
    LocationComponent(const Transform& transform) : Transform(transform) { };
};

struct VelocityComponent : Transform, Component<VelocityComponent, ComponentType::Velocity>
{
    using Transform::Transform;
    VelocityComponent(const Transform& transform) : Transform(transform) { };
};

struct AccelerationComponent : Transform, Component<AccelerationComponent, ComponentType::Acceleration>
{
    using Transform::Transform;
    AccelerationComponent(const Transform& transform) : Transform(transform) { };
};
#endif //ADJUSTABLEPHYSICS2D_COMPONENTS_H
