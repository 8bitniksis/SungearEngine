//
// Created by Ilya on 25.11.2023.
//

#include "CamerasCollector.h"
#include "Camera.h"

SGCore::CamerasCollector::CamerasCollector()
{
    m_componentsCollector.configureCachingFunction<Camera, Transform>();
}