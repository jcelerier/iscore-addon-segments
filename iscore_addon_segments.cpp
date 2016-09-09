#include <Segments/Visitor.hpp>
#include "iscore_addon_segments.hpp"

iscore_addon_segments::iscore_addon_segments() :
    QObject {}
{
}

iscore::GUIApplicationContextPlugin* iscore_addon_segments::make_applicationPlugin(
        const iscore::GUIApplicationContext& app)
{
    return nullptr;
}

iscore::Version iscore_addon_segments::version() const
{
    return iscore::Version{1};
}

UuidKey<iscore::Plugin> iscore_addon_segments::key() const
{
    return_uuid("e64a847a-fdf6-453d-9902-608885d3707f");
}
