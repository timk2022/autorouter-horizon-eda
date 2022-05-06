// uuid.cpp and uuid.hpp pulled from horizon-eda
// https://github.com/horizon-eda/horizon/blob/master/src/util/uuid.hpp
#pragma once

#ifdef WIN32_UUID
#include "uuid_win32.hpp"
#else
#include <uuid/uuid.h>
#endif

#include <string>

class UUID {
    public:
        UUID();
        static UUID random();
        UUID(const char *str);
        UUID(const std::string &str);
        static UUID UUID5(const UUID &nsid, const unsigned char *name, size_t name_size);
        operator std::string() const
        {
            char str[40];
            uuid_unparse(uu, str);
            return std::string(str);
        }

        void to_uuid(const std::string& str) {
            uuid_parse(str.c_str() ,uu);
        }

        /**
         *  @return true if uuid is non-null, false otherwise
         */
        operator bool() const;
        const unsigned char *get_bytes() const
        {
            return uu;
        }
        static constexpr auto size = sizeof(uuid_t);

        friend bool operator==(const UUID &self, const UUID &other);
        friend bool operator!=(const UUID &self, const UUID &other);
        friend bool operator<(const UUID &self, const UUID &other);
        friend bool operator>(const UUID &self, const UUID &other);
        size_t hash() const
        {
            size_t r = 0;
            for (size_t i = 0; i < 16; i++) {
                r ^= ((size_t)uu[i]) << ((i % sizeof(size_t)) * 8);
            }
            return r;
        }

    private:
        uuid_t uu;
};
