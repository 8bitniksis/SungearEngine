//
// Created by stuka on 27.05.2023.
//

#pragma once

#ifndef SUNGEARENGINE_FILESMARTPOINTER_H
#define SUNGEARENGINE_FILESMARTPOINTER_H

#include <iostream>

namespace Core::Utils
{
    class FileSmartPointer
    {
    private:
        FILE* m_filePtr = nullptr;
        std::string m_path;
        std::string m_mode;

    public:
        explicit FileSmartPointer(std::string, std::string);
        ~FileSmartPointer();

        FILE* get() noexcept;
    };
}

#endif //SUNGEARENGINE_FILESMARTPOINTER_H
