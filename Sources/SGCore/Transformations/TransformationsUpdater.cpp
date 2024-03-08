//
// Created by stuka on 02.05.2023.
//

#include <glm/gtc/type_ptr.hpp>
#include <execution>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "TransformationsUpdater.h"
#include "TransformBase.h"
#include "SGCore/Scene/Scene.h"
#include "SGCore/Scene/EntityBaseInfo.h"
#include "SGCore/Graphics/API/IUniformBuffer.h"
#include "SGCore/Physics/Rigidbody3D.h"
#include "SGCore/Physics/PhysicsWorld3D.h"
#include "Transform.h"
#include <glm/gtx/string_cast.hpp>

SGCore::TransformationsUpdater::TransformationsUpdater()
{
    Ref<TimerCallback> callback = MakeRef<TimerCallback>();
    
    m_updaterTimer.setTargetFrameRate(80);
    m_updaterTimer.m_cyclic = true;
    m_updaterTimer.addCallback(callback);

    callback->setUpdateFunction([this](const double& dt, const double& fixedDt) {
        updateTransformations(dt, fixedDt);
    });
    
    m_updaterThread = std::thread([this, callback]() {
        while(m_isAlive)
        {
            if(m_active)
            {
               m_updaterTimer.startFrame();
            }
        }
    });
    
    // m_updaterThread.detach();
}

SGCore::TransformationsUpdater::~TransformationsUpdater()
{
    // m_scene;
    m_isAlive = false;
    m_updaterThread.join();
}

void SGCore::TransformationsUpdater::setScene(const SGCore::Ref<SGCore::Scene>& scene) noexcept
{
    m_scene = scene;
    m_sharedScene = scene;
}

void SGCore::TransformationsUpdater::updateTransformations(const double& dt, const double& fixedDt) noexcept
{
    if(!m_sharedScene) return;

    if(m_changedModelMatrices.isLocked() || m_entitiesForPhysicsUpdateToCheck.isLocked()) return;

    entt::registry& registry = m_sharedScene->getECSRegistry();
    
    auto transformsView = registry.view<Ref<Transform>>();
    
    transformsView.each([&registry, this](const entt::entity& entity, Ref<Transform>& transform) {
        EntityBaseInfo* entityBaseInfo = registry.try_get<EntityBaseInfo>(entity);
        Ref<Transform> parentTransform;
        
        if(entityBaseInfo)
        {
            auto* tmp = registry.try_get<Ref<Transform>>(entityBaseInfo->m_parent);
            parentTransform = (tmp ? *tmp : nullptr);
        }
        
        bool translationChanged = false;
        bool rotationChanged = false;
        bool scaleChanged = false;
        
        TransformBase& ownTransform = transform->m_ownTransform;
        TransformBase& finalTransform = transform->m_finalTransform;
        
        // translation ==============================================
        
        if(ownTransform.m_lastPosition != ownTransform.m_position)
        {
            ownTransform.m_translationMatrix = glm::translate(glm::mat4(1.0),
                                                              ownTransform.m_position
            );
            
            // std::cout << "pos : " << ownTransform.m_position.x << ", " << ownTransform.m_position.y << ", " << ownTransform.m_position.z << std::endl;
            
            ownTransform.m_lastPosition = ownTransform.m_position;
            
            translationChanged = true;
        }
        
        if(parentTransform && transform->m_followParentTRS.x)
        {
            // std::cout << "dfdff" << std::endl;
            finalTransform.m_translationMatrix =
                    parentTransform->m_finalTransform.m_translationMatrix * ownTransform.m_translationMatrix;
        }
        else
        {
            finalTransform.m_translationMatrix = ownTransform.m_translationMatrix;
        }
        
        // rotation ================================================
        
        if(ownTransform.m_lastRotation != ownTransform.m_rotation)
        {
            auto q = glm::identity<glm::quat>();
            
            q = glm::rotate(q, glm::radians(ownTransform.m_rotation.z), { 0, 0, 1 });
            q = glm::rotate(q, glm::radians(ownTransform.m_rotation.y), { 0, 1, 0 });
            q = glm::rotate(q, glm::radians(ownTransform.m_rotation.x), { 1, 0, 0 });
            
            ownTransform.m_rotationMatrix = glm::toMat4(q);
            
            // rotating directions vectors
            ownTransform.m_left = glm::rotate(MathUtils::left3,
                                              glm::radians(-ownTransform.m_rotation.y), glm::vec3(0, 1, 0));
            ownTransform.m_left = glm::rotate(ownTransform.m_left,
                                              glm::radians(-ownTransform.m_rotation.z), glm::vec3(0, 0, 1));
            
            ownTransform.m_left *= -1.0f;
            
            ownTransform.m_forward = glm::rotate(MathUtils::forward3,
                                                 glm::radians(-ownTransform.m_rotation.x), glm::vec3(1, 0, 0));
            ownTransform.m_forward = glm::rotate(ownTransform.m_forward,
                                                 glm::radians(-ownTransform.m_rotation.y), glm::vec3(0, 1, 0));
            
            ownTransform.m_forward *= -1.0f;
            
            ownTransform.m_up = glm::rotate(MathUtils::up3,
                                            glm::radians(-ownTransform.m_rotation.x), glm::vec3(1, 0, 0));
            ownTransform.m_up = glm::rotate(ownTransform.m_up,
                                            glm::radians(-ownTransform.m_rotation.z), glm::vec3(0, 0, 1));
            
            ownTransform.m_up *= -1.0f;
            
            //transformComponent->m_rotationMatrix = glm::toMat4(rotQuat);
            
            ownTransform.m_lastRotation = ownTransform.m_rotation;
            
            rotationChanged = true;
        }
        
        if(parentTransform && transform->m_followParentTRS.y)
        {
            finalTransform.m_rotationMatrix =
                    parentTransform->m_finalTransform.m_rotationMatrix * ownTransform.m_rotationMatrix;
        }
        else
        {
            finalTransform.m_rotationMatrix = ownTransform.m_rotationMatrix;
        }
        
        // scale ========================================================
        
        if(ownTransform.m_lastScale != ownTransform.m_scale)
        {
            ownTransform.m_scaleMatrix = glm::scale(glm::mat4(1.0),
                                                    ownTransform.m_scale
            );
            ownTransform.m_lastScale = ownTransform.m_scale;
            
            scaleChanged = true;
        }
        
        if(parentTransform && transform->m_followParentTRS.z)
        {
            finalTransform.m_scaleMatrix = parentTransform->m_finalTransform.m_scaleMatrix * ownTransform.m_scaleMatrix;
        }
        else
        {
            finalTransform.m_scaleMatrix = ownTransform.m_scaleMatrix;
        }
        
        // model matrix =================================================
        
        bool modelMatrixChanged = translationChanged || rotationChanged || scaleChanged;
        
        // О ТАК ВЕРНО
        transform->m_transformChanged = modelMatrixChanged || (parentTransform && parentTransform->m_transformChanged);
        
        if(transform->m_transformChanged)
        {
            ownTransform.m_modelMatrix =
                    ownTransform.m_translationMatrix * ownTransform.m_rotationMatrix * ownTransform.m_scaleMatrix;
            
            if(parentTransform)
            {
                finalTransform.m_modelMatrix =
                        parentTransform->m_finalTransform.m_modelMatrix * ownTransform.m_modelMatrix;
            }
            else
            {
                finalTransform.m_modelMatrix = ownTransform.m_modelMatrix;
            }

            m_changedModelMatrices.getObject().push_back({ entity, finalTransform.m_modelMatrix });
            m_calculatedNotPhysicalEntities.getObject().m_vector.push_back({ entity, transform });
        }
        else
        {
            m_entitiesForPhysicsUpdateToCheck.getObject().push_back(entity);
        }
    });

    m_changedModelMatrices.lock();
    m_entitiesForPhysicsUpdateToCheck.lock();
    
    m_calculatedNotPhysicalEntities.getObject().m_vectorLastSize.store(m_calculatedNotPhysicalEntities.getObject().m_vector.size());
}

void SGCore::TransformationsUpdater::fixedUpdate(const double& dt, const double& fixedDt) noexcept
{
    // updateTransformations(dt, fixedDt);
    // dispatch transformations
    
    if(!m_sharedScene) return;
    
    const size_t calculatedNotPhysEntitiesLastSize = m_calculatedNotPhysicalEntities.getObject().m_vectorLastSize.load();
    if(calculatedNotPhysEntitiesLastSize > 0)
    {
        auto& vec = m_calculatedNotPhysicalEntities.getObject().m_vector;
        for(size_t i = 0; i < calculatedNotPhysEntitiesLastSize; ++i)
        {
            (*m_transformChangedEvent)(m_sharedScene->getECSRegistry(), vec[i].m_owner, vec[i].m_memberValue);
        }
        // std::cout << "calculatedNotPhysEntitiesLastSize - 1: " << (calculatedNotPhysEntitiesLastSize - 1) << std::endl;
        
        vec.erase(vec.begin(), vec.begin() + calculatedNotPhysEntitiesLastSize - 1);
        m_calculatedNotPhysicalEntities.getObject().m_vectorLastSize.store(0);
    }
    
    const size_t calculatedPhysEntitiesLastSize = m_calculatedPhysicalEntities.getObject().m_vectorLastSize.load();
    if(calculatedPhysEntitiesLastSize > 0)
    {
        auto& vec = m_calculatedPhysicalEntities.getObject().m_vector;
        for(size_t i = 0; i < calculatedPhysEntitiesLastSize; ++i)
        {
            (*m_transformChangedEvent)(m_sharedScene->getECSRegistry(), vec[i].m_owner, vec[i].m_memberValue);
        }
        vec.erase(vec.begin(), vec.begin() + calculatedPhysEntitiesLastSize - 1);
        m_calculatedPhysicalEntities.getObject().m_vectorLastSize.store(0);
    }
}
