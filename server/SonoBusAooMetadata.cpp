#include "SonoBusAooMetadata.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <initializer_list>
#include <utility>

namespace sonobus {
namespace {

using json = nlohmann::json;

std::vector<uint8_t> dumpJson(const json& value)
{
    const auto text = value.dump();
    return std::vector<uint8_t>(text.begin(), text.end());
}

bool parseJson(const AooData& data, json& value)
{
    if (data.data == nullptr || data.size == 0) {
        return false;
    }

    try {
        value = json::parse(reinterpret_cast<const char*>(data.data),
                            reinterpret_cast<const char*>(data.data) + data.size);
        return true;
    } catch (...) {
        return false;
    }
}

const json* findValue(const json& value, std::initializer_list<const char*> keys)
{
    if (!value.is_object()) {
        return nullptr;
    }

    for (const auto* key : keys) {
        auto it = value.find(key);
        if (it != value.end()) {
            return &(*it);
        }
    }

    return nullptr;
}

std::string toLower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}

std::string readString(const json& value, std::initializer_list<const char*> keys,
                       const std::string& fallback = {})
{
    if (const auto* item = findValue(value, keys)) {
        if (item->is_string()) {
            return item->get<std::string>();
        }
        if (item->is_number_integer() || item->is_number_unsigned()) {
            return std::to_string(item->get<long long>());
        }
    }

    return fallback;
}

int readInt(const json& value, std::initializer_list<const char*> keys, int fallback)
{
    if (const auto* item = findValue(value, keys)) {
        if (item->is_number_integer() || item->is_number_unsigned()) {
            return item->get<int>();
        }
        if (item->is_string()) {
            try {
                return std::stoi(item->get<std::string>());
            } catch (...) {
            }
        }
    }

    return fallback;
}

bool readBool(const json& value, std::initializer_list<const char*> keys, bool fallback)
{
    if (const auto* item = findValue(value, keys)) {
        if (item->is_boolean()) {
            return item->get<bool>();
        }
        if (item->is_number_integer() || item->is_number_unsigned()) {
            return item->get<int>() != 0;
        }
        if (item->is_string()) {
            const auto text = toLower(item->get<std::string>());
            return text == "1" || text == "true" || text == "yes" || text == "removed" || text == "deleted";
        }
    }

    return fallback;
}

std::vector<uint8_t> readByteArray(const json& value)
{
    std::vector<uint8_t> result;

    if (value.is_array()) {
        result.reserve(value.size());
        for (const auto& item : value) {
            if (item.is_number_integer()) {
                const auto n = item.get<int>();
                result.push_back(static_cast<uint8_t>(std::clamp(n, 0, 255)));
            }
        }
    } else if (value.is_string()) {
        // Compatibility fallback: older/internal builds may store layout as a raw string.
        const auto text = value.get<std::string>();
        result.assign(text.begin(), text.end());
    }

    return result;
}

void appendUser(std::vector<std::string>& users, const json& item)
{
    if (item.is_string()) {
        users.push_back(item.get<std::string>());
    } else if (item.is_number_integer() || item.is_number_unsigned()) {
        users.push_back(std::to_string(item.get<long long>()));
    } else if (item.is_object()) {
        auto name = readString(item, {"name", "userName", "username", "displayName", "id"});
        if (!name.empty()) {
            users.push_back(std::move(name));
        }
    }
}

std::vector<std::string> readUsers(const json& value)
{
    std::vector<std::string> users;

    const json* source = findValue(value, {"users", "userNames", "usernames", "members", "activeUsers", "active_users"});
    if (source == nullptr) {
        return users;
    }

    if (source->is_array()) {
        users.reserve(source->size());
        for (const auto& item : *source) {
            appendUser(users, item);
        }
    } else if (source->is_object()) {
        for (const auto& item : source->items()) {
            if (item.value().is_boolean()) {
                if (item.value().get<bool>()) {
                    users.push_back(item.key());
                }
            } else {
                appendUser(users, item.value());
            }
        }
    }

    return users;
}

bool actionLooksRemoved(const json& value)
{
    const auto action = toLower(readString(value, {"type", "op", "action", "event"}));
    return action.find("remove") != std::string::npos
        || action.find("delete") != std::string::npos
        || action.find("leave") != std::string::npos;
}

} // namespace

ScopedAooData::ScopedAooData()
    : data{ kAooDataUnspecified, nullptr, 0 }
{
}

void ScopedAooData::set(AooDataType type, std::vector<uint8_t> bytes)
{
    storage = std::move(bytes);
    data.type = type;
    data.data = storage.empty() ? nullptr : reinterpret_cast<const AooByte*>(storage.data());
    data.size = static_cast<AooSize>(storage.size());
}

void ScopedAooData::clear()
{
    storage.clear();
    data = { kAooDataUnspecified, nullptr, 0 };
}

bool toAooData(ScopedAooData& out, const GroupMetadata& metadata)
{
    json value;
    value["type"] = "group";
    value["name"] = metadata.name;
    value["isPublic"] = metadata.isPublic;
    value["public"] = metadata.isPublic;

    out.set(kAooDataJSON, dumpJson(value));
    return true;
}

bool toAooData(ScopedAooData& out, const SourceMetadata& metadata)
{
    json value;
    value["type"] = "source";
    value["sendFormatIndex"] = metadata.sendFormatIndex;
    value["layout"] = metadata.layout;

    out.set(kAooDataJSON, dumpJson(value));
    return true;
}

bool toAooData(ScopedAooData& out, const SinkMetadata& metadata)
{
    json value;
    value["type"] = "sink";
    value["preferredSendFormatIndex"] = metadata.preferredSendFormatIndex;

    out.set(kAooDataJSON, dumpJson(value));
    return true;
}

bool toAooData(ScopedAooData& out, const PublicGroupSubscribeRequestMetadata& metadata)
{
    json value;
    value["type"] = "public_group_subscribe";
    value["op"] = "public_group_subscribe";
    value["subscribe"] = metadata.subscribe;
    value["watch"] = metadata.subscribe;
    value["enabled"] = metadata.subscribe;

    out.set(kAooDataJSON, dumpJson(value));
    return true;
}

bool fromAooData(const AooData& data, GroupMetadata& metadata)
{
    json value;
    if (!parseJson(data, value)) {
        return false;
    }

    if (value.contains("group") && value["group"].is_object()) {
        value = value["group"];
    }

    metadata.name = readString(value, {"name", "groupName", "group_name"}, metadata.name);
    metadata.isPublic = readBool(value, {"isPublic", "public"}, metadata.isPublic);

    return true;
}

bool fromAooData(const AooData& data, SourceMetadata& metadata)
{
    json value;
    if (!parseJson(data, value)) {
        return false;
    }

    metadata.sendFormatIndex = readInt(value, {"sendFormatIndex", "formatIndex", "format_index"}, metadata.sendFormatIndex);

    if (const auto* layout = findValue(value, {"layout", "channelLayout", "channel_layout"})) {
        metadata.layout = readByteArray(*layout);
    }

    return true;
}

bool fromAooData(const AooData& data, SinkMetadata& metadata)
{
    json value;
    if (!parseJson(data, value)) {
        return false;
    }

    metadata.preferredSendFormatIndex = readInt(value,
                                                {"preferredSendFormatIndex", "sendFormatIndex", "formatIndex", "format_index"},
                                                metadata.preferredSendFormatIndex);

    return true;
}

bool fromAooData(const AooData& data, PublicGroupUpdateMetadata& metadata)
{
    json value;
    if (!parseJson(data, value)) {
        return false;
    }

    const json* group = &value;
    if (const auto* nested = findValue(value, {"group", "publicGroup", "public_group"})) {
        if (nested->is_object()) {
            group = nested;
        }
    }

    metadata.groupId = static_cast<AooId>(readInt(*group, {"groupId", "group_id", "id"}, metadata.groupId));
    metadata.groupName = readString(*group, {"groupName", "group_name", "name"}, metadata.groupName);
    metadata.removed = readBool(*group, {"removed", "deleted", "delete", "isRemoved"}, metadata.removed)
                    || readBool(value, {"removed", "deleted", "delete", "isRemoved"}, false)
                    || actionLooksRemoved(value);

    auto users = readUsers(*group);
    if (users.empty() && group != &value) {
        users = readUsers(value);
    }
    metadata.users = std::move(users);

    return metadata.groupId != kAooIdInvalid || !metadata.groupName.empty();
}

} // namespace sonobus
