#include <stdexcept>
#include "Entity.h"
#include "PhysicsEngine.h"
#include <cmath>

Entity::Entity(Context &entityContext, EntityId entityId) : context(entityContext), id(entityId) { }

Entity Entity::create(Context &context) {
    return {context, context.createEntity()};
}

bool Entity::exists() const {
    return context.hasEntity(id);
}

bool Entity::hasComponent(ComponentType type) const {
    return context.hasComponent(id, type);
}

void Entity::removeComponent(ComponentType type) {
    return context.removeComponent(id, type);
}

#ifndef USE_PRIMITIVES_ONLY
void Entity::makeConvex(const Vector2 globalVertices[], size_t count) {
    if(count < 3) throw std::invalid_argument("Wrong vertices count!");

    size_t lastIndex = count - 1;
    auto first = globalVertices[lastIndex], second = globalVertices[0];
    auto crossProduct = Vector2::crossProduct(first, second);
    real area = crossProduct;
    Vector2 centroid = (first + second) * crossProduct;
    for (size_t i = 0; i < lastIndex; ++i) {
        first = globalVertices[i], second = globalVertices[i + 1];
        crossProduct = Vector2::crossProduct(first, second);
        centroid += (first + second) * crossProduct;
        area += crossProduct;
    }
    area *= real(0.5);
    centroid /= (6 * area);

    auto vertices = new Vector2[count];
    auto normals = new Vector2[count];
    vertices[0] = globalVertices[0] - centroid;
    auto maxSqrMagnitude = vertices[0].getSqrMagnitude();
    Vector2 edges[count];
#ifdef USE_INERTIA
    auto inertia = real(0);
#endif
    for (size_t i = 1; i < count; ++i) {
        vertices[i] = globalVertices[i] - centroid;
#ifdef USE_INERTIA
        first = vertices[i - 1], second = vertices[i];
        crossProduct = Vector2::crossProduct(first, second);
        inertia += crossProduct * (Vector2::dotProduct(first, first) + Vector2::dotProduct(first, second) + Vector2::dotProduct(second, second));
#endif
        auto sqrMagnitude = vertices[i].getSqrMagnitude();
        if(sqrMagnitude > maxSqrMagnitude) maxSqrMagnitude = sqrMagnitude;
        edges[i - 1] = second - first;
    }
    edges[lastIndex] = vertices[0] - vertices[lastIndex];
#ifdef USE_INERTIA
    first = vertices[lastIndex], second = vertices[0];
    crossProduct = Vector2::crossProduct(first, second);
    inertia += crossProduct * (Vector2::dotProduct(first, first) + Vector2::dotProduct(first, second) + Vector2::dotProduct(second, second));
    inertia /= area;
#endif

    bool isConcave = false;
    int sign = 0;
    auto &prevEdge = edges[lastIndex];
    normals[lastIndex] = (Vector2{-prevEdge.y, prevEdge.x}).getNormalized();
    for (size_t i = 0; i < lastIndex; ++i) {
        auto &edge = edges[i];
        normals[i] = (Vector2{-edge.y, edge.x}).getNormalized();
        if(!isConcave) {
            crossProduct = Vector2::crossProduct(prevEdge, edge);
            int newSign;
            if(crossProduct > 0) {
                newSign = 1;
            } else if(crossProduct < 0) {
                newSign = -1;
            } else continue;
            if(sign == 0) {
                sign = newSign;
            } else if(sign != newSign) isConcave = true;
        }
        prevEdge = edge;
    }
    if(isConcave) throw std::invalid_argument("Concave figures are prohibited!");

    context.addComponent<PolygonComponent>(id, count, vertices, normals);
    auto &polygon = context.getComponent<PolygonComponent>(id);
    Shape shape{};
    shape.shapeType = ShapeType::Complex;
    shape.centroid = centroid;
    shape.radius = polygon.getRadius();
    shape.boundingBox = polygon.getAABB();
    context.addComponent<ShapeComponent>(id, shape);
#ifdef USE_DENSITY
    if(context.hasComponent<MaterialComponent>(id) && context.hasComponent<MassInfoComponent>(id)) {
        auto &material = context.getComponent<MaterialComponent>(id);
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        area = std::abs(area);
        massInfo.mass = material.density * area;
        massInfo.inverseMass = real(1) / massInfo.mass;
    }
#endif
#ifdef USE_INERTIA
    if(context.hasComponent<MassInfoComponent>(id)) {
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        inertia *= massInfo.mass / real(12);
        massInfo.inertia = inertia;
        massInfo.inverseInertia = real(1) / inertia;
    }
#endif
#ifdef USE_COLLISION_FILTER
    if(!context.hasComponent<CollisionFilterComponent>(id)) context.addComponent<CollisionFilterComponent>(id);
#endif
}

void Entity::makeComplexFromAABB(Vector2 min, Vector2 max) {
    if(max.x < min.x || max.y < min.y) throw std::invalid_argument("Wrong AABB vertices!");
    Shape shape{};
    shape.boundingBox = {min, max};
    shape.shapeType = ShapeType::Complex;
    shape.centroid = (max + min) * real(0.5);
    shape.radius = (max - min).getMagnitude() * real(0.5);
    context.addComponent<ShapeComponent>(id, shape);
    if(!context.hasComponent<LocationComponent>(id)) context.addComponent<LocationComponent>(id, shape.centroid);
    auto vertices = new Vector2[4] {max - shape.centroid, Vector2{min.x, max.y} - shape.centroid, min - shape.centroid, Vector2{max.x, min.y} - shape.centroid};
    auto normals = new Vector2[4] {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    context.addComponent<PolygonComponent>(id, 4, vertices, normals);
#ifdef USE_DENSITY
    if(context.hasComponent<MaterialComponent>(id) && context.hasComponent<MassInfoComponent>(id)) {
        auto &material = context.getComponent<MaterialComponent>(id);
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        auto size = max - min;
        auto area = size.x * size.y;
        massInfo.mass = material.density * area;
        massInfo.inverseMass = real(1) / massInfo.mass;
#ifdef USE_INERTIA
        massInfo.inertia = massInfo.mass * size.getSqrMagnitude() / real(12);
        massInfo.inverseInertia = real(1) / massInfo.inertia;
#endif
    }
#endif
#ifdef USE_COLLISION_FILTER
    if(!context.hasComponent<CollisionFilterComponent>(id)) context.addComponent<CollisionFilterComponent>(id);
#endif
}

void Entity::makeComplexFromAABB() {
    if(!context.hasComponent<ShapeComponent>(id)) throw std::logic_error("Figure is not AABB!");
    auto &shape = context.getComponent<ShapeComponent>(id);
    if(shape.shapeType != ShapeType::AABB) throw std::logic_error("Figure is not AABB!");
    makeComplexFromAABB(shape.boundingBox.min, shape.boundingBox.max);
}
#endif

#ifndef USE_CIRCLES_ONLY
void Entity::makeAABB(Vector2 min, Vector2 max) {
    if(max.x < min.x || max.y < min.y) throw std::invalid_argument("Wrong AABB vertices!");
    Shape shape{};
    shape.boundingBox = {min, max};
#ifndef USE_AABB_ONLY
    shape.shapeType = ShapeType::AABB;
    shape.centroid = (max + min) * real(0.5);
    shape.radius = (max - min).getMagnitude() * real(0.5);
#endif
    context.addComponent<ShapeComponent>(id, shape);
#ifdef USE_DENSITY
    if(context.hasComponent<MaterialComponent>(id) && context.hasComponent<MassInfoComponent>(id)) {
        auto &material = context.getComponent<MaterialComponent>(id);
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        auto size = max - min;
        auto area = size.x * size.y;
        massInfo.mass = material.density * area;
        massInfo.inverseMass = real(1) / massInfo.mass;
    }
#endif
#ifdef USE_COLLISION_FILTER
    if(!context.hasComponent<CollisionFilterComponent>(id)) context.addComponent<CollisionFilterComponent>(id);
#endif
}
#endif

#ifndef USE_AABB_ONLY
void Entity::makeCircle(real radius) {
    if(radius < 0) throw std::invalid_argument("Negative radius is prohibited!");
    Shape shape{};
    shape.radius = radius;
    shape.centroid = {0, 0};
#ifndef USE_CIRCLES_ONLY
    shape.shapeType = ShapeType::Circle;
    shape.boundingBox = {{-radius, -radius}, {radius, radius}};
#endif
    context.addComponent<ShapeComponent>(id, shape);
#ifdef USE_DENSITY
    if(context.hasComponent<MaterialComponent>(id) && context.hasComponent<MassInfoComponent>(id)) {
        auto &material = context.getComponent<MaterialComponent>(id);
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        auto area = real(M_PI) * radius * radius;
        massInfo.mass = material.density * area;
        massInfo.inverseMass = real(1) / massInfo.mass;
    }
#endif
#ifdef USE_INERTIA
    if(context.hasComponent<MassInfoComponent>(id)) {
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        auto inertia = PhysicsEngine::DefaultInertiaCoefficient * massInfo.mass * radius * radius;
        massInfo.inertia = inertia;
        massInfo.inverseInertia = real(1) / inertia;
    }
#endif
#ifdef USE_COLLISION_FILTER
    if(!context.hasComponent<CollisionFilterComponent>(id)) context.addComponent<CollisionFilterComponent>(id);
#endif
}
#endif

#ifndef USE_EQUAL_MASSES
void Entity::setMass(real mass) {
    if(mass < 0) throw std::invalid_argument("Negative mass is prohibited!");
#ifdef USE_INERTIA
    if(context.hasComponent<MassInfoComponent>(id)) {
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        massInfo.mass = mass;
        massInfo.inverseMass = real(1) / mass;
    } else {
        context.addComponent<MassInfoComponent>(id, mass, mass);
    }
#else
    context.addComponent<MassInfoComponent>(id, mass);
#endif
}
#endif

#ifdef USE_INERTIA
void Entity::setInertia(real inertia) {
    if(inertia < 0) throw std::invalid_argument("Negative inertia is prohibited!");
#ifdef USE_EQUAL_MASSES
    context.addComponent<MassInfoComponent>(id, inertia);
#else
    if(context.hasComponent<MassInfoComponent>(id)) {
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        massInfo.inertia = inertia;
        massInfo.inverseInertia = real(1) / inertia;
    } else {
        context.addComponent<MassInfoComponent>(id, 1, inertia);
    }
#endif
}

#ifndef USE_EQUAL_MASSES
void Entity::setMassInfo(real mass, real inertia) {
    if(mass < 0) throw std::invalid_argument("Negative mass is prohibited!");
    if(inertia < 0) throw std::invalid_argument("Negative inertia is prohibited!");
    context.addComponent<MassInfoComponent>(id, mass, inertia);
}
#endif
#endif

#ifdef USE_MATERIAL
void Entity::setMaterial(Material material) {
#ifdef USE_FRICTION
    if(material.friction < 0) throw std::invalid_argument("Negative friction is prohibited!");
#endif
#ifdef USE_BOUNCINESS
    if(material.bounciness < 0) throw std::invalid_argument("Negative bounciness is prohibited!");
#endif
#ifdef USE_DENSITY
    if(material.density < 0) throw std::invalid_argument("Negative density is prohibited!");
    if(context.hasComponent<MaterialComponent>(id) && context.hasComponent<MassInfoComponent>(id)) {
        auto oldDensity = context.getComponent<MaterialComponent>(id).density;
        auto &massInfo = context.getComponent<MassInfoComponent>(id);
        auto k = material.density / oldDensity;
        massInfo.mass *= k;
        massInfo.inverseMass = real(1) / massInfo.mass;
#ifdef USE_INERTIA
        massInfo.inertia *= k;
        massInfo.inverseInertia = real(1) / massInfo.inertia;
#endif
    } else if(context.hasComponent<ShapeComponent>(id)) {
        auto &shape = context.getComponent<ShapeComponent>(id);
        real area = 0, mass = 1;
#ifdef USE_INERTIA
        real inertia = PhysicsEngine::DefaultInertiaCoefficient;
#endif
#ifdef USE_PRIMITIVES_ONLY
        shape.tryGetArea(area);
        mass = area * material.density;
#ifdef USE_INERTIA
        shape.tryGetInertia(mass, inertia);
#endif
#else
        if(!shape.tryGetArea(area)) {
            if(context.hasComponent<PolygonComponent>(id)) {
                auto &polygon = context.getComponent<PolygonComponent>(id);
                area = polygon.getArea();
                mass = area * material.density;
#ifdef USE_INERTIA
                inertia = polygon.getInertia(mass);
#endif
            } else {
                throw std::logic_error("Not primitive shape has no polygon!");
            }
        }
        else {
            mass = area * material.density;
#ifdef USE_INERTIA
            shape.tryGetInertia(mass, inertia);
#endif
        }
#endif
        MassInfo massInfo{};
        massInfo.mass = mass;
        massInfo.inverseMass = real(1) / mass;
#ifdef USE_INERTIA
        massInfo.inertia = inertia;
        massInfo.inverseInertia = real(1) / inertia;
#endif
        context.addComponent<MassInfoComponent>(id, massInfo);
    }
#endif
    context.addComponent<MaterialComponent>(id, material);
}
#endif

void Entity::setLocation(Transform location) {
    context.addComponent<LocationComponent>(id, location);
}

void Entity::setVelocity(Transform velocity) {
    context.addComponent<VelocityComponent>(id, velocity);
#ifdef USE_ACCELERATION
    if(!context.hasComponent<AccelerationComponent>(id)) context.addComponent<AccelerationComponent>(id);
#endif
}

void Entity::addImpulse(Vector2 linear
#if defined(USE_ROTATION) && defined(USE_INERTIA)
        , real angular
#endif
) {
    if(!context.hasComponent<MassInfoComponent>(id)) throw std::logic_error("Entity has no mass!");
    if(!context.hasComponent<VelocityComponent>(id)) throw std::logic_error("Entity has no velocity!");
    auto &massInfo = context.getComponent<MassInfoComponent>(id);
    auto inverseMass = massInfo.inverseMass;
    auto &velocity = context.getComponent<VelocityComponent>(id);
    velocity.linear += linear * inverseMass;
#if defined(USE_ROTATION) && defined(USE_INERTIA)
    auto inverseInertia = massInfo.inverseInertia;
    velocity.angular += angular * inverseInertia;
#endif
}

#ifdef USE_ACCELERATION
void Entity::setAcceleration(Transform acceleration) {
    context.addComponent<AccelerationComponent>(id, acceleration);
}

void Entity::addForce(Vector2 force
#if defined(USE_ROTATION) && defined(USE_INERTIA)
        , real torque
#endif
        ) {
    if(!context.hasComponent<MassInfoComponent>(id)) throw std::logic_error("Entity has no mass!");
    if(!context.hasComponent<AccelerationComponent>(id)) throw std::logic_error("Entity has no acceleration!");
    auto &massInfo = context.getComponent<MassInfoComponent>(id);
    auto inverseMass = massInfo.inverseMass;
    auto &acceleration = context.getComponent<AccelerationComponent>(id);
    acceleration.linear += force * inverseMass;
#if defined(USE_ROTATION) && defined(USE_INERTIA)
    auto inverseInertia = massInfo.inverseInertia;
    acceleration.angular += torque * inverseInertia;
#endif
}
#endif
