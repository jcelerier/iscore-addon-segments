#include <Segments/Visitor.hpp>
#include <iscore/actions/Menu.hpp>
#include <QInputDialog>
#include "iscore_addon_segments.hpp"

#include <iscore/actions/ActionManager.hpp>
#include <iscore/actions/MenuManager.hpp>

namespace Segments
{
struct ApplicationPlugin :
        public QObject,
        public iscore::GUIApplicationPlugin
{
    public:
        using iscore::GUIApplicationPlugin::GUIApplicationPlugin;

        GUIElements makeGUIElements() override
        {
            // This will add a button in File > Export.
            auto& m = context.menus.get().at(iscore::Menus::Export());
            QMenu* menu = m.menu();

            auto act = new QAction{QObject::tr("Segments example"), this};
            menu->addAction(act);
            connect(act, &QAction::triggered, this, [=] {
                if(auto doc = this->currentDocument())
                {
                    QInputDialog::getMultiLineText(
                                nullptr,
                                "Segments",
                                "Segments",
                                PrintScenario(doc->context()));
                }
            });

            return {};
        }
};
}

iscore_addon_segments::iscore_addon_segments() = default;

iscore::GUIApplicationPlugin* iscore_addon_segments::make_applicationPlugin(
        const iscore::GUIApplicationContext& app)
{
    return new Segments::ApplicationPlugin{app};
}

iscore::Version iscore_addon_segments::version() const
{
    return iscore::Version{1};
}

UuidKey<iscore::Plugin> iscore_addon_segments::key() const
{
    return_uuid("e64a847a-fdf6-453d-9902-608885d3707f");
}
