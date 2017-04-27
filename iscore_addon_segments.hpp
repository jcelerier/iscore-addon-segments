#pragma once
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <QObject>

#include <iscore/plugins/application/GUIApplicationPlugin.hpp>

class iscore_addon_segments final:
        public QObject,
        public iscore::Plugin_QtInterface,
        public iscore::ApplicationPlugin_QtInterface
{
        Q_OBJECT
        Q_PLUGIN_METADATA(IID ApplicationPlugin_QtInterface_iid)
        Q_INTERFACES(
                iscore::Plugin_QtInterface
                iscore::ApplicationPlugin_QtInterface
                )

    public:
        iscore_addon_segments();
        virtual ~iscore_addon_segments() = default;

        iscore::GUIApplicationPlugin* make_guiApplicationPlugin(
                const iscore::GUIApplicationContext& app) override;

        iscore::Version version() const override;
        UuidKey<iscore::Plugin> key() const override;
};
