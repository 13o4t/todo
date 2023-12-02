#pragma once

#include <functional>
#include <optional>
#include <variant>

#include "task/task_export.h"
#include "task/state.h"

namespace task {

    template<typename Derived, typename State>
    class Fsm {
    public:
        const State current_state() const { return state_; }
    protected:
        using NextState = std::optional<State>;

        State& current() { return state_; }

        template<typename Event>
        void dispatch(Event&& event) {
            Derived& self = static_cast<Derived&>(*this);
            auto next = std::visit(
                [&](auto& s)->NextState { return self.on_event(s, std::forward<Event>(event)); },
                state_
            );
            if (next.has_value()) state_ = std::move(next.value());
        }

    private:
        State state_;
    };

    class TASK_EXPORT Parser : public Fsm<Parser, States> {
        friend class Fsm;
    public:
        Parser();
        ~Parser();

        std::optional<Task> parse(const std::string& line);
        std::string to_string(const Task& task);

    private:
        template<typename State, typename Event>
        NextState on_event(State&, const Event&) { return std::nullopt; }

        NextState on_event(StateStart&, const char&);

        NextState on_event(StateText&, const char&);
        NextState on_event(StateBlank&, const char&);

        NextState on_event(StateCompletion&, const char&);
        NextState on_event(StatePriority&, const char&);
        NextState on_event(StatePriorityEnd&, const char&);
        NextState on_event(StateProject&, const char&);
        NextState on_event(StateContext&, const char&);
        NextState on_event(StateSpecial&, const char&);

    private:
    };
}