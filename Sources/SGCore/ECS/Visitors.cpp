//
// Created by stuka on 12.08.2026.
//

#include "Visitors.h"

#include "SGCore/AI/GOAP/State.h"
#include "SGCore/Animation/AnimationsTree.h"
#include "SGCore/Audio/AudioSource.h"
#include "SGCore/Math/AABB.h"
#include "SGCore/Motion/IK/IKJoint.h"
#include "SGCore/Motion/IK/IKRootJoint.h"
#include "SGCore/Motion/MotionPlanner.h"
#include "SGCore/Navigation/NavGrid3D.h"
#include "SGCore/Navigation/NavMesh/NavMesh.h"
#include "SGCore/Navigation/NavObstacle.h"
#include "SGCore/Particles/ParticlesEmitter.h"
#include "SGCore/Physics/Ragdoll3D.h"
#include "SGCore/Physics/Rigidbody3D.h"
#include "SGCore/Physics/VehicleWheel.h"
#include "SGCore/Physics/WheeledVehicle.h"
#include "SGCore/Render/Alpha/OpaqueEntityTag.h"
#include "SGCore/Render/Alpha/TransparentEntityTag.h"
#include "SGCore/Render/Atmosphere/Atmosphere.h"
#include "SGCore/Render/Batching/Batch.h"
#include "SGCore/Render/Camera3D.h"
#include "SGCore/Render/Decals/Decal.h"
#include "SGCore/Render/Gizmos/BoxGizmo.h"
#include "SGCore/Render/Gizmos/LineGizmo.h"
#include "SGCore/Render/Gizmos/SphereGizmo.h"
#include "SGCore/Render/Instancing/Instancing.h"
#include "SGCore/Render/LayeredFrameReceiver.h"
#include "SGCore/Render/Lighting/SpotLight.h"
#include "SGCore/Render/MainCameraTag.h"
#include "SGCore/Render/Mesh.h"
#include "SGCore/Render/Picking/Pickable.h"
#include "SGCore/Render/RenderAbilities/EnableBatchingPass.h"
#include "SGCore/Render/RenderAbilities/EnableDecalPass.h"
#include "SGCore/Render/RenderAbilities/EnableInstancingPass.h"
#include "SGCore/Render/RenderAbilities/EnableMeshPass.h"
#include "SGCore/Render/RenderAbilities/EnableTerrainPass.h"
#include "SGCore/Render/RenderAbilities/EnableVolumetricPass.h"
#include "SGCore/Render/RenderingBase.h"
#include "SGCore/Render/ShadowMapping/CSM/CSMTarget.h"
#include "SGCore/Render/ShadowMapping/ShadowCaster.h"
#include "SGCore/Render/SpacePartitioning/IgnoreOctrees.h"
#include "SGCore/Render/SpacePartitioning/ObjectsCullingOctree.h"
#include "SGCore/Render/SpacePartitioning/Octree.h"
#include "SGCore/Render/SpacePartitioning/OctreeCullable.h"
#include "SGCore/Render/Terrain/Terrain.h"
#include "SGCore/Render/UICamera.h"
#include "SGCore/Render/Volumetric/VolumetricFog.h"
#include "SGCore/Scene/EntityBaseInfo.h"
#include "SGCore/Scene/RootEntityTag.h"
#include "SGCore/Serde/Components/NonSavable.h"
#include "SGCore/Transformations/Controllable3D.h"
#include "SGCore/Transformations/Transform.h"
#include "SGCore/UI/UIComponent.h"

void SGCore::ECS::addStandardVisitors(VisitorsRegistry& registry) noexcept
{
    registry.registerVisitor(MakeRef<TransformVisitor>());
    registry.registerVisitor(MakeRef<EntityBaseInfoVisitor>());
    registry.registerVisitor(MakeRef<AnimationsTreeVisitor>());
    registry.registerVisitor(MakeRef<AudioSourceVisitor>());
    registry.registerVisitor(MakeRef<GOAPEntityStateVisitor>());
    registry.registerVisitor(MakeRef<AABBFloatVisitor>());
    registry.registerVisitor(MakeRef<AABBDoubleVisitor>());
    registry.registerVisitor(MakeRef<MotionPlannerVisitor>());
    registry.registerVisitor(MakeRef<MeshVisitor>());
    registry.registerVisitor(MakeRef<AtmosphereVisitor>());
    registry.registerVisitor(MakeRef<EnableTerrainPassVisitor>());
    registry.registerVisitor(MakeRef<EnableVolumetricPassVisitor>());
    registry.registerVisitor(MakeRef<EnableInstancingPassVisitor>());
    registry.registerVisitor(MakeRef<EnableBatchingPassVisitor>());
    registry.registerVisitor(MakeRef<EnableDecalPassVisitor>());
    registry.registerVisitor(MakeRef<EnableMeshPassVisitor>());
    registry.registerVisitor(MakeRef<SpotLightVisitor>());
    registry.registerVisitor(MakeRef<SphereGizmoVisitor>());
    registry.registerVisitor(MakeRef<BoxGizmoVisitor>());
    registry.registerVisitor(MakeRef<LineGizmoVisitor>());
    registry.registerVisitor(MakeRef<TerrainVisitor>());
    registry.registerVisitor(MakeRef<BatchVisitor>());
    registry.registerVisitor(MakeRef<NavMeshVisitor>());
    registry.registerVisitor(MakeRef<ParticlesEmitterVisitor>());
    registry.registerVisitor(MakeRef<InstancingVisitor>());
    registry.registerVisitor(MakeRef<Controllable3DVisitor>());
    registry.registerVisitor(MakeRef<NavGrid3DVisitor>());
    registry.registerVisitor(MakeRef<OpaqueEntityTagVisitor>());
    registry.registerVisitor(MakeRef<TransparentEntityTagVisitor>());
    registry.registerVisitor(MakeRef<CSMTargetVisitor>());
    registry.registerVisitor(MakeRef<Rigidbody3DVisitor>());
    registry.registerVisitor(MakeRef<VehicleWheelVisitor>());
    registry.registerVisitor(MakeRef<WheeledVehicleVisitor>());
    registry.registerVisitor(MakeRef<Ragdoll3DVisitor>());
    registry.registerVisitor(MakeRef<UICameraVisitor>());
    registry.registerVisitor(MakeRef<Camera3DVisitor>());
    registry.registerVisitor(MakeRef<LayeredFrameReceiverVisitor>());
    registry.registerVisitor(MakeRef<PickableVisitor>());
    registry.registerVisitor(MakeRef<OctreeVisitor>());
    registry.registerVisitor(MakeRef<ObjectsCullingOctreeVisitor>());
    registry.registerVisitor(MakeRef<IgnoreOctreesVisitor>());
    registry.registerVisitor(MakeRef<OctreeCullableVisitor>());
    registry.registerVisitor(MakeRef<RenderingBaseVisitor>());
    registry.registerVisitor(MakeRef<ShadowCasterVisitor>());
    registry.registerVisitor(MakeRef<MainCameraTagVisitor>());
    registry.registerVisitor(MakeRef<DecalVisitor>());
    registry.registerVisitor(MakeRef<VolumetricFogVisitor>());
    registry.registerVisitor(MakeRef<IKRootJointVisitor>());
    registry.registerVisitor(MakeRef<IKJointVisitor>());
    registry.registerVisitor(MakeRef<NavObstacleVisitor>());
    registry.registerVisitor(MakeRef<RootEntityTagVisitor>());
    registry.registerVisitor(MakeRef<NonSavableVisitor>());
    registry.registerVisitor(MakeRef<UIComponentVisitor>());
}

SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::TransformVisitor, SGCore::Transform);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EntityBaseInfoVisitor, SGCore::EntityBaseInfo);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AnimationsTreeVisitor, SGCore::AnimationsTree);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AudioSourceVisitor, SGCore::AudioSource);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::GOAPEntityStateVisitor, SGCore::GOAP::EntityState);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AABBFloatVisitor, SGCore::AABB<float>);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AABBDoubleVisitor, SGCore::AABB<double>);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::MotionPlannerVisitor, SGCore::MotionPlanner);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::MeshVisitor, SGCore::Mesh);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::AtmosphereVisitor, SGCore::Atmosphere);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableTerrainPassVisitor, SGCore::EnableTerrainPass);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableVolumetricPassVisitor, SGCore::EnableVolumetricPass);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableInstancingPassVisitor, SGCore::EnableInstancingPass);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableBatchingPassVisitor, SGCore::EnableBatchingPass);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableDecalPassVisitor, SGCore::EnableDecalPass);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::EnableMeshPassVisitor, SGCore::EnableMeshPass);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::SpotLightVisitor, SGCore::SpotLight);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::SphereGizmoVisitor, SGCore::SphereGizmo);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::BoxGizmoVisitor, SGCore::BoxGizmo);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::LineGizmoVisitor, SGCore::LineGizmo);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::TerrainVisitor, SGCore::Terrain);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::BatchVisitor, SGCore::Batch);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NavMeshVisitor, SGCore::Navigation::NavMesh);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::ParticlesEmitterVisitor, SGCore::ParticlesEmitter);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::InstancingVisitor, SGCore::Instancing);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Controllable3DVisitor, SGCore::Controllable3D);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NavGrid3DVisitor, SGCore::Navigation::NavGrid3D);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::OpaqueEntityTagVisitor, SGCore::OpaqueEntityTag);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::TransparentEntityTagVisitor, SGCore::TransparentEntityTag);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::CSMTargetVisitor, SGCore::CSMTarget);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Rigidbody3DVisitor, SGCore::Rigidbody3D);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::VehicleWheelVisitor, SGCore::VehicleWheel);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::WheeledVehicleVisitor, SGCore::WheeledVehicle);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Ragdoll3DVisitor, SGCore::Ragdoll3D);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::UICameraVisitor, SGCore::UICamera);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::Camera3DVisitor, SGCore::Camera3D);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::LayeredFrameReceiverVisitor, SGCore::LayeredFrameReceiver);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::PickableVisitor, SGCore::Pickable);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::OctreeVisitor, SGCore::Octree);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::ObjectsCullingOctreeVisitor, SGCore::ObjectsCullingOctree);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::IgnoreOctreesVisitor, SGCore::IgnoreOctrees);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::OctreeCullableVisitor, SGCore::OctreeCullable);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::RenderingBaseVisitor, SGCore::RenderingBase);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::ShadowCasterVisitor, SGCore::ShadowCaster);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::MainCameraTagVisitor, SGCore::MainCameraTag);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::DecalVisitor, SGCore::Decal);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::VolumetricFogVisitor, SGCore::VolumetricFog);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::IKRootJointVisitor, SGCore::IKRootJoint);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::IKJointVisitor, SGCore::IKJoint);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NavObstacleVisitor, SGCore::Navigation::NavObstacle);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::RootEntityTagVisitor, SGCore::RootEntityTag);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::NonSavableVisitor, SGCore::NonSavable);
SG_IMPLEMENT_ECS_COMPONENT_DEFAULT_VISITOR(SGCore::ECS::UIComponentVisitor, SGCore::UI::UIComponent);
