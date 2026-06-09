#include "SonoBusAooMetadata.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstring>

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
        // Compatibility fallback: some older/internal builds stored layout as a raw string.
        const auto text = value.get<std::string>();
        result.assign(text.begin(), text.end());
    }

    return result;
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

bool fromAooData(const AooData& data, GroupMetadata& metadata)
{
    json value;
    if (!parseJson(data, value)) {
        return false;
    }

    if (value.contains("name") && value["name"].is_string()) {
        metadata.name = value["name"].get<std::string>();
    }

    if (value.contains("isPublic") && value["isPublic"].is_boolean()) {
        metadata.isPublic = value["isPublic"].get<bool>();
    } else if (value.contains("public") && value["public"].is_boolean()) {
        metadata.isPublic = value["public"].get<bool>();
    }

    return true;
}

bool fromAooData(const AooData& data, SourceMetadata& metadata)
{
    json value;
    if (!parseJson(data, value)) {
        return false;
    }

    if (value.contains("sendFormatIndex") && value["sendFormatIndex"].is_number_integer()) {
        metadata.sendFormatIndex = value["sendFormatIndex"].get<int>();
    } else if (value.contains("formatIndex") && value["formatIndex"].is_number_integer()) {
        metadata.sendFormatIndex = value["formatIndex"].get<int>();
    }

    if (value.contains("layout")) {
        metadata.layout = readByteArray(value["layout"]);
    }

    return true;
}

bool fromAooData(const AooData& data, SinkMetadata& metadata)
{
    json value;
    if (!parseJson(data, value)) {
        return false;
    }

    if (value.contains("preferredSendFormatIndex") && value["preferredSendFormatIndex"].is_number_integer()) {
        metadata.preferredSendFormatIndex = value["preferredSendFormatIndex"].get<int>();
    } else if (value.contains("sendFormatIndex") && value["sendFormatIndex"].is_number_integer()) {
        metadata.preferredSendFormatIndex = value["sendFormatIndex"].get<int>();
    }

    return true;
}

} // namespace sonobus
