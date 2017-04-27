#pragma once
#include <core/document/DocumentModel.hpp>
#include <Scenario/Document/Constraint/Slot.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <ImageProcess/ImageModel.hpp>

#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Scenario/Process/Algorithms/ContainersAccessors.hpp>
#include <iscore/tools/SubtypeVariant.hpp>

// Note : the audio plug-in is put in #ifdefs because
// it requires Faust for now which can be a pain to compile.
// Normal code shouldn't be that ugly :)
#if defined(ISCORE_PLUGIN_AUDIO)
#include <Audio/SoundProcess/SoundProcessModel.hpp>
#endif
namespace Segments
{

/** For the sake of convenience
 * we define here a type that allows to eaisly visit
 * the processes that we are interested in.
 *
 * There can be a lot of processes since they can be
 * implemented via plug-ins but most won't be useful
 * for segments (midi notes, automations, etc...)
 */
using SegmentsProcess =
    iscore::SubtypeVariant<
        const Process::ProcessModel,
        const Scenario::ProcessModel,
        const Loop::ProcessModel,
        const Image::ProcessModel
#if defined(ISCORE_PLUGIN_AUDIO)
        , const Audio::Sound::ProcessModel
#endif
>;

struct DepthVisitor
{
        QDebug stream;
        int hierarchy_depth = 0;

        auto& print()
        {
            return stream << QString(hierarchy_depth, ' ');
        }

        void operator()(const Scenario::ConstraintModel& constraint)
        {
            print() << "Constraint: \n";
            // First print the user metadata : name, label, etc.
            (*this)(constraint.metadata());

            // Then the durations :
            print() << constraint.duration.defaultDuration() << " ";
            print() << constraint.duration.minDuration() << " ";
            print() << constraint.duration.maxDuration() << " \n";

            // A constraint has :
            // Temporal processes (ie what happens at execution).
            // For instance : automation, etc...
            print() << "Processes: \n";
            auto& processes = constraint.processes;
            hierarchy_depth++;
            for(SegmentsProcess process : processes)
            {
                process.apply(*this);
                print() << "\n";
            }
            hierarchy_depth--;

            // Finally, a constraint is between two states :
            constraint.startState();
            constraint.endState();
        }

        void operator()(const Scenario::EventModel& event)
        {
            print() << "Event: \n";
            (*this)(event.metadata());

            // Events carry a condition
            print() << "Condition:" << event.condition().toString() << "\n";

            // And ids of states
            // (they will be visited in operator()({Scenario,Loop}::ProcessModel))
            for(auto& state_id : event.states())
            {
                (void) state_id;
                // ...
            }
        }

        void operator()(const Scenario::StateModel& state)
        {
            print() << "State: \n";
            (*this)(state.metadata());

            // States carry messages
            print() << "Messages: " << Process::flatten(state.messages().rootNode()) << "\n";

            // And instantaneous processes.
            // The only process currently is a javascript code one, it's not very useful for
            // Segments I guess.
            hierarchy_depth++;
            for(auto& state_process : state.stateProcesses)
            {
                print() << "State process: " << state_process.prettyName() << "\n";
                print() << "\n";
            }
            hierarchy_depth--;
        }

        void operator()(const Scenario::TimeNodeModel& timenode)
        {
            print() << "TimeNode: \n";
            (*this)(timenode.metadata());

            // Time nodes are used for synchronization and also carry a condition
            print() << "Condition:" << timenode.trigger()->expression().toString();

            // And have ids of events
            for(auto& event_id : timenode.events())
            {
                (void) event_id;
                // ...
            }
        }

        void operator()(const Scenario::BaseScenario& base)
        {
            // The top-level scenario
            print() << "Base Scenario: \n";

            hierarchy_depth++;
            (*this)(base.startTimeNode());
            (*this)(base.startEvent());
            (*this)(base.startState());
            (*this)(base.constraint());
            (*this)(base.endState());
            (*this)(base.endEvent());
            (*this)(base.endTimeNode());
            hierarchy_depth--;
        }

        // Here we list some of the processes that can be useful for Segments
        void operator()(const Scenario::ProcessModel& scenario)
        {
            print() << "Scenario: \n";
            (*this)(scenario.metadata());
            // Scenario is the "main" process in i-score.
            // Here we explore it "depth-first" : we check all of its elements.
            print() << "Constraints: \n";
            hierarchy_depth++;
            for(auto& e : scenario.getConstraints())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;

            print() << "TimeNodes: \n";
            hierarchy_depth++;
            for(auto& e : scenario.getTimeNodes())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;

            print() << "Events: \n";
            hierarchy_depth++;
            for(auto& e : scenario.getEvents())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;

            print() << "States: \n";
            hierarchy_depth++;
            for(auto& e : scenario.getStates())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;

        }

        void operator()(const Loop::ProcessModel& loop)
        {
            // Very similar to a scenario but has only one constraint
            // and two time nodes, two events, two states

            print() << "Loop: \n";
            (*this)(loop.metadata());

            print() << "Constraints: \n";
            hierarchy_depth++;
            for(auto& e : loop.getConstraints())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;

            print() << "TimeNodes: \n";
            hierarchy_depth++;
            for(auto& e : loop.getTimeNodes())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;

            print() << "Events: \n";
            hierarchy_depth++;
            for(auto& e : loop.getEvents())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;

            print() << "States: \n";
            hierarchy_depth++;
            for(auto& e : loop.getStates())
            {
                (*this)(e);
                print() << "\n";
            }
            hierarchy_depth--;
        }


        void operator()(const iscore::ModelMetadata& metadata)
        {
            print() << "Metadata: "
                    << metadata.getName() << ", "
                    << metadata.getLabel() << ", "
                    << metadata.getComment() << ", "
                    << metadata.getColor().getColor() << ", "
                    << metadata.getExtendedMetadata() << "\n";
        }

        void operator()(const Image::ProcessModel& image)
        {
            print() << "Image: " << image.imagePath() << "\n";
        }

#if defined(ISCORE_PLUGIN_AUDIO)
        void operator()(const Audio::Sound::ProcessModel& sound)
        {
            print() << "Sound: " << sound.file().name() << "\n";
        }
#endif
        void operator()()
        {

        }

};


struct BreadthVisitor
{
        QDebug stream;
        int hierarchy_depth = 0;

        auto& print()
        {
            return stream << QString(hierarchy_depth, ' ');
        }

        void operator()(const Scenario::ConstraintModel& constraint)
        {
            print() << "Constraint: \n";
            // First print the user metadata : name, label, etc.
            (*this)(constraint.metadata());

            print() << "Processes: \n";
            auto& processes = constraint.processes;
            hierarchy_depth++;
            for(SegmentsProcess process : processes)
            {
                process.apply(*this);
                print() << "\n";
            }
            hierarchy_depth--;

            // Visit the next time node :
            auto& scenar = Scenario::parentScenario(constraint);
            (*this)(Scenario::parentTimeNode(
                        scenar.state(constraint.endState()),
                        scenar));

            // <!> <!> <!>
            // if two constraints end up rejoining themselves,
            // what's after them *WILL* be visited twice.
            // To prevent this it is necessary to add some kind of
            // marking or list of already-visited timenodes.
            // for instance std::unordered_set<Id<TimeNodeModel>> and check
            // for existance before visiting.

            // Since there are no cycles there cannot
            // be infinite loops however.
        }

        void operator()(const Scenario::EventModel& event)
        {
            print() << "Event: \n";
            (*this)(event.metadata());

            // Visit the states
            auto& scenar = Scenario::parentScenario(event);
            for(auto& state_id : event.states())
            {
                (*this)(scenar.state(state_id));
            }
        }

        void operator()(const Scenario::StateModel& state)
        {
            print() << "State: \n";
            (*this)(state.metadata());

            // Visit the next constraint
            if(state.nextConstraint())
            {
                auto& scenar = Scenario::parentScenario(state);
                (*this)(scenar.constraint(*state.nextConstraint()));
            }
        }

        void operator()(const Scenario::TimeNodeModel& timenode)
        {
            print() << "TimeNode: \n";
            (*this)(timenode.metadata());

            // Visit all the events
            auto& scenar = Scenario::parentScenario(timenode);
            for(auto& event_id : timenode.events())
            {
                (*this)(scenar.event(event_id));
            }
        }


        void operator()(const Scenario::BaseScenario& base)
        {
            // The top-level scenario
            print() << "Base Scenario: \n";

            hierarchy_depth++;
            (*this)(base.startTimeNode());
            hierarchy_depth--;
        }

        // Processes
        void operator()(const Scenario::ProcessModel& scenario)
        {
            print() << "Scenario: \n";
            (*this)(scenario.metadata());

            // Scenario is the "main" process in i-score.
            // Here we explore it "breadth-first" : we start from
            // the first time node and go through all the elements in the "temporal order".
            hierarchy_depth++;
            (*this)(scenario.startTimeNode());
            hierarchy_depth--;
        }

        void operator()(const Loop::ProcessModel& loop)
        {
            print() << "Loop: \n";
            (*this)(loop.metadata());

            hierarchy_depth++;
            (*this)(loop.startTimeNode());
            hierarchy_depth--;
        }

        void operator()(const iscore::ModelMetadata& metadata)
        {
            print() << "Metadata: "
                    << metadata.getName() << ", "
                    << metadata.getLabel() << ", "
                    << metadata.getComment() << ", "
                    << metadata.getColor().getColor() << ", "
                    << metadata.getExtendedMetadata() << "\n";
        }

        void operator()(const Image::ProcessModel& image)
        {
            print() << "Image: " << image.imagePath() << "\n";
        }

#if defined(ISCORE_PLUGIN_AUDIO)
        void operator()(const Audio::Sound::ProcessModel& sound)
        {
            print() << "Sound: " << sound.file().name() << "\n";
        }
#endif

        void operator()()
        {

        }


};

inline QString PrintScenario(const iscore::DocumentContext& doc)
{
    // First get the "root" of an i-score score.
    // A BaseScenario is the topmost hierarchy level.
    Scenario::BaseScenario& base = iscore::IDocument::get<Scenario::ScenarioDocumentModel>(doc.document).baseScenario();

    QString s;
    s.reserve(1024 * 8);
    QDebug dbg(&s);

    DepthVisitor vis{dbg.noquote().nospace()};
    vis(base);

    dbg << "\n\n\n============================\n\n\n";
    BreadthVisitor{dbg.noquote().nospace()};
    vis(base);
    return s;
}


}
