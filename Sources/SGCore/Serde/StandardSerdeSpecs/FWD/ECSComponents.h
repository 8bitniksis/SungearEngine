//
// Created by stuka on 24.08.2025.
//

#pragma once

#include "SGCore/Motion/IK/IKRootJoint.h"
#include "SGCore/Serde/Serde.h"

namespace SGCore
{
    namespace ECS
    {
        struct ComponentBase;
    }

    struct Mesh;
    struct Atmosphere;
    struct EntityBaseInfo;
    class UniqueNameWrapper;
    struct TransformBase;
    struct Transform;
    struct RenderingBase;
    struct Rigidbody3D;
    struct MotionPlanner;
    struct OpaqueEntityTag;
    struct TransparentEntityTag;
    struct UICamera;
    struct Camera3D;
    struct Controllable3D;
    struct SpotLight;
    struct LightBase;
    struct MeshBase;
    struct LineGizmo;
    struct GizmoBase;
    struct BoxGizmo;
    struct SphereGizmo;
    struct AudioSource;
    struct Pickable;
    struct EnableBatchingPass;
    struct EnableDecalPass;
    struct EnableInstancingPass;
    struct EnableMeshPass;
    struct EnableTerrainPass;
    struct EnableVolumetricPass;
    struct MainCameraTag;

    template<typename ScalarT>
    requires(std::is_signed_v<ScalarT>)
    struct AABB;
}

namespace SGCore::Serde
{
    // ======================================================== EntityBaseInfo FWD

    template<FormatType TFormatType>
    struct SerdeSpec<ECS::ComponentBase, TFormatType> :
            BaseTypes<>,
            DerivedTypes<
                Mesh, Atmosphere,
                EntityBaseInfo, Transform,
                RenderingBase, Rigidbody3D,
                MotionPlanner, OpaqueEntityTag,
                TransparentEntityTag, UICamera,
                Controllable3D, SpotLight,
                LineGizmo, BoxGizmo,
                SphereGizmo, AudioSource,
                Pickable, EnableBatchingPass,
                EnableDecalPass, EnableInstancingPass,
                EnableMeshPass, EnableTerrainPass,
                EnableVolumetricPass, MainCameraTag
            >
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::ECS::ComponentBase")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const ECS::ComponentBase, TFormatType>& valueView) noexcept {}

        static void deserialize(DeserializableValueView<ECS::ComponentBase, TFormatType>& valueView) noexcept {}
    };

    // ======================================================== EntityBaseInfo FWD

    template<FormatType TFormatType>
    struct SerdeSpec<EntityBaseInfo, TFormatType> :
            BaseTypes<ECS::ComponentBase, UniqueNameWrapper>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::EntityBaseInfo")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const EntityBaseInfo, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<EntityBaseInfo, TFormatType>& valueView) noexcept;
    };

    // ======================================================== TransformBase FWD

    template<FormatType TFormatType>
    struct SerdeSpec<TransformBase, TFormatType> :
            BaseTypes<>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::TransformBase")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const TransformBase, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<TransformBase, TFormatType>& valueView) noexcept;
    };

    // ======================================================== Transform FWD

    template<FormatType TFormatType>
    struct SerdeSpec<Transform, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::Transform")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const Transform, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<Transform, TFormatType>& valueView) noexcept;
    };

    // ======================================================== Pickable FWD

    template<FormatType TFormatType>
    struct SerdeSpec<Pickable, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::Pickable")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const Pickable, TFormatType>& valueView,
                              const ECS::entity_t& deserializableEntity,
                              ECS::registry_t& toRegistry) noexcept;

        static void deserialize(DeserializableValueView<Pickable, TFormatType>& valueView,
                                const ECS::entity_t& deserializableEntity,
                                ECS::registry_t& toRegistry) noexcept;
    };

    // ======================================================== AABB FWD

    template<typename ScalarT, FormatType TFormatType>
    struct SerdeSpec<AABB<ScalarT>, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::AABB")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const AABB<ScalarT>, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<AABB<ScalarT>, TFormatType>& valueView) noexcept;
    };

    // ======================================================== RenderingBase FWD

    template<FormatType TFormatType>
    struct SerdeSpec<RenderingBase, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::RenderingBase")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const RenderingBase, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<RenderingBase, TFormatType>& valueView) noexcept;
    };

    // ======================================================== AudioSource FWD

    template<FormatType TFormatType>
    struct SerdeSpec<AudioSource, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::AudioSource")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const AudioSource, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<AudioSource, TFormatType>& valueView) noexcept;
    };

    // ======================================================== Atmosphere FWD

    template<FormatType TFormatType>
    struct SerdeSpec<Atmosphere, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::Atmosphere")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const Atmosphere, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<Atmosphere, TFormatType>& valueView) noexcept;
    };

    // ======================================================== SphereGizmo FWD

    template<FormatType TFormatType>
    struct SerdeSpec<SphereGizmo, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::SphereGizmo")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const SphereGizmo, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<SphereGizmo, TFormatType>& valueView) noexcept;
    };

    // ======================================================== BoxGizmo FWD

    template<FormatType TFormatType>
    struct SerdeSpec<BoxGizmo, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::BoxGizmo")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const BoxGizmo, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<BoxGizmo, TFormatType>& valueView) noexcept;
    };

    // ======================================================== GizmoBase FWD

    template<FormatType TFormatType>
    struct SerdeSpec<GizmoBase, TFormatType> :
            BaseTypes<>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::GizmoBase")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const GizmoBase, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<GizmoBase, TFormatType>& valueView) noexcept;
    };

    // ======================================================== LineGizmo FWD

    template<FormatType TFormatType>
    struct SerdeSpec<LineGizmo, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::LineGizmo")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const LineGizmo, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<LineGizmo, TFormatType>& valueView) noexcept;
    };

    // ======================================================== MeshBase FWD

    template<FormatType TFormatType>
    struct SerdeSpec<MeshBase, TFormatType> :
            BaseTypes<>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::MeshBase")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const MeshBase, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<MeshBase, TFormatType>& valueView) noexcept;
    };

    // ======================================================== Mesh FWD

    template<FormatType TFormatType>
    struct SerdeSpec<Mesh, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::Mesh")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const Mesh, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<Mesh, TFormatType>& valueView) noexcept;
    };

    // ======================================================== LightBase FWD

    template<FormatType TFormatType>
    struct SerdeSpec<LightBase, TFormatType> :
            BaseTypes<>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::LightBase")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const LightBase, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<LightBase, TFormatType>& valueView) noexcept;
    };

    // ======================================================== SpotLight FWD

    template<FormatType TFormatType>
    struct SerdeSpec<SpotLight, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::SpotLight")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const SpotLight, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<SpotLight, TFormatType>& valueView) noexcept;
    };

    // ======================================================== Controllable3D FWD

    template<FormatType TFormatType>
    struct SerdeSpec<Controllable3D, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::Controllable3D")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const Controllable3D, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<Controllable3D, TFormatType>& valueView) noexcept;
    };

    // ======================================================== Camera3D FWD

    template<FormatType TFormatType>
    struct SerdeSpec<Camera3D, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::Camera3D")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const Camera3D, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<Camera3D, TFormatType>& valueView) noexcept;
    };

    // ======================================================== UICamera FWD

    template<FormatType TFormatType>
    struct SerdeSpec<UICamera, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::UICamera")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const UICamera, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<UICamera, TFormatType>& valueView) noexcept;
    };

    // ======================================================== TransparentEntityTag FWD

    template<FormatType TFormatType>
    struct SerdeSpec<TransparentEntityTag, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::TransparentEntityTag")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const TransparentEntityTag, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<TransparentEntityTag, TFormatType>& valueView) noexcept;
    };

    // ======================================================== OpaqueEntityTag FWD

    template<FormatType TFormatType>
    struct SerdeSpec<OpaqueEntityTag, TFormatType> :
           BaseTypes<ECS::ComponentBase>,
           DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::OpaqueEntityTag")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const OpaqueEntityTag, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<OpaqueEntityTag, TFormatType>& valueView) noexcept;
    };

    // ======================================================== MotionPlanner FWD

    template<FormatType TFormatType>
    struct SerdeSpec<MotionPlanner, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::MotionPlanner")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const MotionPlanner, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<MotionPlanner, TFormatType>& valueView) noexcept;
    };

    // ======================================================== Rigidbody3D FWD

    template<FormatType TFormatType>
    struct SerdeSpec<Rigidbody3D, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::Rigidbody3D")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const Rigidbody3D, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<Rigidbody3D, TFormatType>& valueView) noexcept;
    };

    // ======================================================== IKRootJoint FWD

    template<FormatType TFormatType>
    struct SerdeSpec<IKRootJoint, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::IKRootJoint")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const IKRootJoint, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<IKRootJoint, TFormatType>& valueView) noexcept;
    };

    // ======================================================== IKJoint FWD

    template<FormatType TFormatType>
    struct SerdeSpec<IKJoint, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::IKJoint")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const IKJoint, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<IKJoint, TFormatType>& valueView) noexcept;
    };

    // ======================================================== EnableBatchingPass FWD

    template<FormatType TFormatType>
    struct SerdeSpec<EnableBatchingPass, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::EnableBatchingPass")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const EnableBatchingPass, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<EnableBatchingPass, TFormatType>& valueView) noexcept;
    };

    // ======================================================== EnableDecalPass FWD

    template<FormatType TFormatType>
    struct SerdeSpec<EnableDecalPass, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::EnableDecalPass")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const EnableDecalPass, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<EnableDecalPass, TFormatType>& valueView) noexcept;
    };

    // ======================================================== EnableInstancingPass FWD

    template<FormatType TFormatType>
    struct SerdeSpec<EnableInstancingPass, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::EnableInstancingPass")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const EnableInstancingPass, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<EnableInstancingPass, TFormatType>& valueView) noexcept;
    };

    // ======================================================== EnableMeshPass FWD

    template<FormatType TFormatType>
    struct SerdeSpec<EnableMeshPass, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::EnableMeshPass")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const EnableMeshPass, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<EnableMeshPass, TFormatType>& valueView) noexcept;
    };

    // ======================================================== EnableTerrainPass FWD

    template<FormatType TFormatType>
    struct SerdeSpec<EnableTerrainPass, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::EnableTerrainPass")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const EnableTerrainPass, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<EnableTerrainPass, TFormatType>& valueView) noexcept;
    };

    // ======================================================== EnableVolumetricPass FWD

    template<FormatType TFormatType>
    struct SerdeSpec<EnableVolumetricPass, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::EnableVolumetricPass")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const EnableVolumetricPass, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<EnableVolumetricPass, TFormatType>& valueView) noexcept;
    };

    // ======================================================== MainCameraTag FWD

    template<FormatType TFormatType>
    struct SerdeSpec<MainCameraTag, TFormatType> :
            BaseTypes<ECS::ComponentBase>,
            DerivedTypes<>
    {
        SG_SERDE_DEFINE_TYPE_NAME("SGCore::MainCameraTag")
        static inline constexpr bool is_pointer_type = false;

        static void serialize(SerializableValueView<const MainCameraTag, TFormatType>& valueView) noexcept;

        static void deserialize(DeserializableValueView<MainCameraTag, TFormatType>& valueView) noexcept;
    };
}
