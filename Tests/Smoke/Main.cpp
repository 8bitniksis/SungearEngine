//
// Created by 8bitniksis on 16.08.2026.
//

#include <charconv>
#include <cstdio>
#include <string_view>
#include <vector>

#include "SGCore/Graphics/API/GAPISelector.h"

#include "SmokeApp.h"

namespace
{
    void printUsage() noexcept
    {
        std::puts(
            "SGSmokeTest — reference scene for comparing graphics backends.\n"
            "\n"
            "Options:\n"
            "  --gapi <name>          force backend: gl4, gl46, vulkan, dx12 (same as SG_GAPI env var)\n"
            "  --frames <n>           frame index to capture (default 60)\n"
            "  --output <file.png>    where to write the captured frame (default smoke_<gapi>.png)\n"
            "  --reference <file.png> compare against this image; exit code 1 on mismatch\n"
            "  --threshold <0..255>   per-channel difference tolerated per pixel (default 8)\n"
            "  --max-diff <fraction>  max fraction of differing pixels to still pass (default 0.01)\n"
            "  --help\n"
            "\n"
            "Exit codes: 0 — captured (and matched, if reference given); 1 — mismatch; 2 — capture failed.");
    }

    template<typename T>
    bool parseNumber(std::string_view text, T& out) noexcept
    {
        const auto result = std::from_chars(text.data(), text.data() + text.size(), out);
        return result.ec == std::errc { } && result.ptr == text.data() + text.size();
    }
}

int main(int argc, char** argv)
{
    SGSmoke::SmokeOptions options;

    for(int i = 1; i < argc; ++i)
    {
        const std::string_view arg = argv[i];
        const bool hasValue = i + 1 < argc;

        if(arg == "--help")
        {
            printUsage();
            return 0;
        }
        else if(arg == "--gapi" && hasValue)
        {
            const auto type = SGCore::gapiTypeFromString(argv[++i]);
            if(!type)
            {
                std::printf("Unknown graphics API '%s'.\n", argv[i]);
                return 2;
            }
            SGCore::GAPISelector::setPreference({ *type });
        }
        else if(arg == "--frames" && hasValue)
        {
            if(!parseNumber(argv[++i], options.m_captureFrame)) { printUsage(); return 2; }
        }
        else if(arg == "--output" && hasValue)
        {
            options.m_outputPath = argv[++i];
        }
        else if(arg == "--reference" && hasValue)
        {
            options.m_referencePath = std::filesystem::path(argv[++i]);
        }
        else if(arg == "--threshold" && hasValue)
        {
            unsigned int threshold = 0;
            if(!parseNumber(argv[++i], threshold) || threshold > 255) { printUsage(); return 2; }
            options.m_channelThreshold = static_cast<std::uint8_t>(threshold);
        }
        else if(arg == "--max-diff" && hasValue)
        {
            if(!parseNumber(argv[++i], options.m_maxDifferingPixelsFraction)) { printUsage(); return 2; }
        }
        else
        {
            std::printf("Unknown or incomplete option '%s'.\n", argv[i]);
            printUsage();
            return 2;
        }
    }

    SGSmoke::SmokeApp app(options);
    app.start(true);

    return app.getExitCode();
}
