#pragma once

#include "aoo.h"

#include <cstdint>
#include <string>
#include <vector>

namespace sonobus {

struct GroupMetadata
{
    std::string name;
    bool isPublic = false;
};

struct SourceMetadata
{
    int sendFormatIndex = -1;
    std::vector<uint8_t> layout;
};

struct SinkMetadata
{
    int preferredSendFormatIndex = -1;
};

struct PublicGroupSubscribeRequestMetadata
{
    PublicGroupSubscribeRequestMetadata() = default;
    explicit PublicGroupSubscribeRequestMetadata(bool shouldSubscribe)
        : subscribe(shouldSubscribe)
    {
    }

    bool subscribe = false;
};

struct PublicGroupUpdateMetadata
{
    AooId groupId = kAooIdInvalid;
    std::string groupName;
    std::vector<std::string> users;
    bool removed = false;
};

class ScopedAooData
{
public:
    ScopedAooData();

    AooData& get() noexcept { return data; }
    const AooData& get() const noexcept { return data; }

    void set(AooDataType type, std::vector<uint8_t> bytes);
    void clear();

private:
    std::vector<uint8_t> storage;
    AooData data;
};

bool toAooData(ScopedAooData& out, const GroupMetadata& metadata);
bool toAooData(ScopedAooData& out, const SourceMetadata& metadata);
bool toAooData(ScopedAooData& out, const SinkMetadata& metadata);
bool toAooData(ScopedAooData& out, const PublicGroupSubscribeRequestMetadata& metadata);

bool fromAooData(const AooData& data, GroupMetadata& metadata);
bool fromAooData(const AooData& data, SourceMetadata& metadata);
bool fromAooData(const AooData& data, SinkMetadata& metadata);
bool fromAooData(const AooData& data, PublicGroupUpdateMetadata& metadata);

} // namespace sonobus
