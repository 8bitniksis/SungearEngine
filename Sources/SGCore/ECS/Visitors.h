//
// Created by stuka on 12.08.2026.
//

#pragma once

#include "SGCore/ECS/IComponentVisitor.h"

// Forward-declare all default component visitors.
namespace SGCore::ECS
{
    struct TransformVisitor;
    struct EntityBaseInfoVisitor;
    struct AnimationsTreeVisitor;
    struct AudioSourceVisitor;
    struct GOAPEntityStateVisitor;
    struct AABBFloatVisitor;
    struct AABBDoubleVisitor;
    struct MotionPlannerVisitor;
    struct MeshVisitor;
    struct AtmosphereVisitor;
    struct EnableTerrainPassVisitor;
    struct EnableVolumetricPassVisitor;
    struct EnableInstancingPassVisitor;
    struct EnableBatchingPassVisitor;
    struct EnableDecalPassVisitor;
    struct EnableMeshPassVisitor;
    struct SpotLightVisitor;
    struct SphereGizmoVisitor;
    struct BoxGizmoVisitor;
    struct LineGizmoVisitor;
    struct TerrainVisitor;
    struct BatchVisitor;
    struct NavMeshVisitor;
    struct ParticlesEmitterVisitor;
    struct InstancingVisitor;
    struct Controllable3DVisitor;
    struct NavGrid3DVisitor;
    struct OpaqueEntityTagVisitor;
    struct TransparentEntityTagVisitor;
    struct CSMTargetVisitor;
    struct Rigidbody3DVisitor;
    struct VehicleWheelVisitor;
    struct WheeledVehicleVisitor;
    struct Ragdoll3DVisitor;
    struct UICameraVisitor;
    struct Camera3DVisitor;
    struct LayeredFrameReceiverVisitor;
    struct PickableVisitor;
    struct OctreeVisitor;
    struct ObjectsCullingOctreeVisitor;
    struct IgnoreOctreesVisitor;
    struct OctreeCullableVisitor;
    struct RenderingBaseVisitor;
    struct ShadowCasterVisitor;
    struct MainCameraTagVisitor;
    struct DecalVisitor;
    struct VolumetricFogVisitor;
    struct IKRootJointVisitor;
    struct IKJointVisitor;
    struct NavObstacleVisitor;
    struct RootEntityTagVisitor;
    struct NonSavableVisitor;
    struct UIComponentVisitor;

    void addStandardVisitors(VisitorsRegistry& registry) noexcept;
}

SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::TransformVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EntityBaseInfoVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AnimationsTreeVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AudioSourceVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::GOAPEntityStateVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AABBFloatVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AABBDoubleVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::MotionPlannerVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::MeshVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AtmosphereVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableTerrainPassVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableVolumetricPassVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableInstancingPassVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableBatchingPassVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableDecalPassVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableMeshPassVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::SpotLightVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::SphereGizmoVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::BoxGizmoVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::LineGizmoVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::TerrainVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::BatchVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NavMeshVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::ParticlesEmitterVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::InstancingVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Controllable3DVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NavGrid3DVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::OpaqueEntityTagVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::TransparentEntityTagVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::CSMTargetVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Rigidbody3DVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::VehicleWheelVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::WheeledVehicleVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Ragdoll3DVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::UICameraVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Camera3DVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::LayeredFrameReceiverVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::PickableVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::OctreeVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::ObjectsCullingOctreeVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::IgnoreOctreesVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::OctreeCullableVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::RenderingBaseVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::ShadowCasterVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::MainCameraTagVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::DecalVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::VolumetricFogVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::IKRootJointVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::IKJointVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NavObstacleVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::RootEntityTagVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NonSavableVisitor);
SG_DECLARE_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::UIComponentVisitor);
