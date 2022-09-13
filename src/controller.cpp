#include "controller.hpp"

#include <fstream>

namespace detail {

    enum class extensions {
        none = 0,
        png,
        jpg
    };

    std::string extensionToString(extensions ext) {
        switch (ext) {
            default:
            case detail::extensions::none:
                return "";
            case detail::extensions::png:
                return ".png";
            case detail::extensions::jpg:
                return ".jpg";
        }
    }

    std::tuple<bool, extensions> isAvatarExist(const std::string& id) {
        if (std::filesystem::exists(globals::path / id)) {
            return { true, extensions::none };
        }

        if (std::filesystem::exists(globals::path / (id + ".png"))) {
            return { true, extensions::png };
        }

        if (std::filesystem::exists(globals::path / (id + ".jpg"))) {
            return { true, extensions::jpg };
        }
        
        return { false, extensions::none };
    }

    oatpp::String readWholeFile(const std::filesystem::path& filename) {
        const std::string stdString = filename.generic_string();
        return { oatpp::base::StrBuffer::loadFromFile(stdString.c_str()) };
    }

    oatpp::String getAvatar(const std::string& id) {
        auto [exist, type] = isAvatarExist(id);
        if (!exist) {
            return globals::default_image;
        }

        if (oatpp::String image = readWholeFile(globals::path / (id + extensionToString(type)))) {
            if (globals::cache_state) {
                globals::cache.insert(id, image);
            }

            return image;
        }

        // Should not be the case because we already verified that file exist, but this will happend if user cannot read this file
        return globals::default_image;
    }

}

oatpp::String StaticManager::readFile(const std::string& id, bool overwrite) {
    if (id == "-1") {
        return globals::default_image;
    }

    if (globals::cache_state && !overwrite) {
        if (oatpp::String image = globals::cache.get(id)) {
            return image;
        }
    }

    return detail::getAvatar(id);
}
