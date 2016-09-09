#pragma once
#include <core/document/DocumentModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Loop/LoopProcessModel.hpp>
#include <ImageProcess/ImageModel.hpp>
#include <Audio/SoundProcess/SoundProcessModel.hpp>

#include <boost/variant.hpp>
namespace Segments
{
using SegmentsProcess =
    eggs::variant<
        const Scenario::ProcessModel*,
        const Loop::ProcessModel*,
        const Image::ProcessModel*,
        const Audio::Sound::ProcessModel*>;

template<typename Base, typename Variant_T>
auto make_dynamic_variant_impl(const Base& base)
{
    // The type does not match any
    // in the proposed types for the variant.
    return Variant_T{};
}

template<typename Base, typename Variant_T, typename Arg, typename... Args>
auto make_dynamic_variant_impl(const Base& base)
{
    if(auto derived = dynamic_cast<Arg>(&base))
    {
        return Variant_T{derived};
    }

    return make_dynamic_variant_impl<Base, Variant_T, Args...>(base);
}

template<typename Base, typename... Args>
auto make_dynamic_variant(const Base& base)
{
    return make_dynamic_variant_impl<Base, eggs::variant<Args...>, Args...>(base);
}

auto make_segments_process(const Process::ProcessModel& base)
{
    return make_dynamic_variant<Process::ProcessModel,
            const Scenario::ProcessModel*,
            const Loop::ProcessModel*,
            const Image::ProcessModel*,
            const Audio::Sound::ProcessModel*>(base);
}

struct SegmentsVisitor
{
        void operator()(const Scenario::ConstraintModel& constraint)
        {
            // A constraint has :
            // Temporal processes (ie what happens at execution).
            // For instance : automation, etc...
            auto& processes = constraint.processes;
            for(auto& process : processes)
            {
                auto var = make_segments_process(process);
                eggs::variants::apply(*this, var);
            }

            // And racks : racks are the visual organization of the processes.
            // Most of the time you only need to use the first one which is the
            // one shown by default.
            auto& racks = constraint.racks;
        }

        void operator()(const Scenario::EventModel& event)
        {

        }

        void operator()(const Scenario::StateModel& state)
        {

        }

        void operator()(const Scenario::TimeNodeModel& timenode)
        {

        }


        // Here we list some of the processes that can be useful for segments
        void operator()(const Scenario::ProcessModel* scenario)
        {

        }

        void operator()(const Loop::ProcessModel* loop)
        {

        }

        void operator()(const Image::ProcessModel* image)
        {

        }

        void operator()(const Audio::Sound::ProcessModel* sound)
        {

        }
};

inline void PrintScenario(const iscore::DocumentContext& doc)
{
    // First get the "root" of an i-score score.
    // A BaseScenario is the topmost hierarchy level.
    // Scenario::BaseScenario& base = get<Scenario::ScenarioDocumentModel>(doc).baseScenario();



}
}
