//
// Created by stuka on 07.05.2023.
//

#ifndef SUNGEARENGINE_ASSETMANAGER_H
#define SUNGEARENGINE_ASSETMANAGER_H

#include <iostream>
#include <memory>

#include "Assets/IAsset.h"

namespace SGCore
{
    class AssetManager
    {
    private:
        static inline std::unordered_map<std::string, std::shared_ptr<IAsset>> m_assets;

    public:
        AssetManager() = delete;

        static void init();

        /**
        * Adds asset with loading by path.
        * If asset already exists then return already loaded asset.
        * @tparam AssetT - Type of asset
        * @param path - Asset path
        * @return Added or already loaded asset
        */
        template<typename AssetT, typename... Args>
        requires(std::is_base_of_v<IAsset, AssetT>)
        static std::shared_ptr<AssetT> loadAsset(const std::string& path)
        {
            auto foundAssetPair = m_assets.find(path);

            if(foundAssetPair != m_assets.end())
            {
                return std::static_pointer_cast<AssetT>(foundAssetPair->second);
            }

            Ref<AssetT> newAsset = AssetT::template createRefInstance<AssetT>();

            newAsset->load(path);

            m_assets.emplace(path, newAsset);

            return newAsset;
        }

        template<typename AssetT, typename... Args>
        requires(std::is_base_of_v<IAsset, AssetT>)
        static std::shared_ptr<AssetT> loadAssetWithAlias(const std::string& alias, const std::string& path)
        {
            auto foundAssetPair = m_assets.find(alias);

            if(foundAssetPair != m_assets.end())
            {
                return std::static_pointer_cast<AssetT>(foundAssetPair->second);
            }

            Ref<AssetT> newAsset = AssetT::template createRefInstance<AssetT>();

            newAsset->load(path);

            m_assets.emplace(alias, newAsset);

            return newAsset;
        }

        static void addAsset(const std::string& alias, const Ref<IAsset>& asset)
        {
            auto foundAssetPair = m_assets.find(alias);

            if(foundAssetPair == m_assets.end())
            {
                m_assets[alias] = asset;
            }
        }

        static void addAsset(const Ref<IAsset>& asset)
        {
            auto foundAssetPair = m_assets.find(asset->getPath().string());

            if(foundAssetPair == m_assets.end())
            {
                m_assets[asset->getPath().string()] = asset;
            }
        }

        /**
         * Creates asset without loading by path.
         * If asset already exists then return found asset.
         * @tparam AssetT - Type of asset
         * @param path - Asset pseudonym
         * @return Created or found asset
         */
        template<typename AssetT, typename... Args>
        requires(std::is_base_of_v<IAsset, AssetT>)
        static std::shared_ptr<AssetT> createAsset(const std::string& path, const Args&... args)
        {
            auto foundAssetPair = m_assets.find(path);

            if(foundAssetPair != m_assets.end())
            {
                return std::static_pointer_cast<AssetT>(foundAssetPair->second);
            }

            std::shared_ptr<AssetT> newAsset = std::make_shared<AssetT>(args...);

            m_assets.emplace(path, newAsset);

            return newAsset;
        }
    };
}

#endif // SUNGEARENGINE_ASSETMANAGER_H
