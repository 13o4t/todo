#include "task/parser.h"

#include <list>
#include <sstream>

#include "task/date.h"

namespace task {

    Parser::Parser() {}

    Parser::~Parser() {}

    std::optional<Task> Parser::parse(const std::string& line) {
        if (line.empty()) return std::nullopt;

        current() = StateStart{};
        for (auto& c : line) {
            dispatch(c);
        }
        dispatch('\n');

        auto& state = current_state();
        auto base = std::get_if<StateEnd>(&state);
        if (base) return base->task;
        return std::nullopt;
    }

    std::string Parser::to_string(const Task& task) {
        std::ostringstream ss;
        if (task.is_completion) ss << "x ";
        if (task.priority != ' ') ss << "(" << task.priority << ") ";
        if (!task.completion_date.empty()) ss << task.completion_date << " ";
        if (!task.creation_date.empty()) ss << task.creation_date << " ";
        ss << task.text;
        return ss.str();
    }

    Parser::NextState Parser::on_event(StateStart& state, const char& c) {
        if (c == 'x') return StateCompletion{std::move(state.task)};
        else if (c == '(') return StatePriority{std::move(state.task)};
        else if (c == '+') return StateProject{std::move(state.task)};
        else if (c == '@') return StateContext{std::move(state.task)};
        else if (c == '\n') return StateEnd{std::move(state.task)};
        else if (c == ' ') return state;

        state.task.text += c;
        return StateText{std::move(state.task)};
    }

    Parser::NextState Parser::on_event(StateText& state, const char& c) {
        if (c == ':') {
            if (state.task.text.empty()) {
                state.task.text += c;
                return state;
            } else {
                state.task.text += c;
                StateSpecial next{std::move(state.task)};
                std::size_t pos = next.task.text.find_last_of(" ");
                if (pos != std::string::npos) {
                    next.key = next.task.text.substr(pos + 1);
                    next.key = next.key.substr(0, next.key.length()-1);
                } else {
                    next.key = std::move(next.task.text);
                }
                return next;
            }
        } else if (c == '\n') {
            return StateEnd{std::move(state.task)};
        } else if (c != ' ') {
            state.task.text += c;
            return state;
        }

        if (!date::valid(state.task.text)) {
            state.task.text += c;
            return StateBlank{std::move(state.task)};
        }

        if (state.task.is_completion && state.task.completion_date.empty()) {
            state.task.completion_date = std::move(state.task.text);
            return StateBlank{std::move(state.task)};
        } else if (state.task.creation_date.empty()) {
            state.task.creation_date = std::move(state.task.text);
            return StateBlank{std::move(state.task)};
        } else {
            state.task.text += c;
            return StateBlank{std::move(state.task)};
        }
    }

    Parser::NextState Parser::on_event(StateBlank& state, const char& c) {
        if (c == '(') {
            if (state.task.priority == ' ' && 
                state.task.text.empty() && 
                state.task.completion_date.empty() &&
                state.task.creation_date.empty() &&
                state.task.projects.empty() &&
                state.task.contexts.empty() &&
                state.task.specials.empty())
                return StatePriority{std::move(state.task)};
            state.task.text = c;
            return StateText{std::move(state.task)};
        } else if (c == '+') {
            state.task.text += c;
            return StateProject{std::move(state.task)};
        } else if (c == '@') {
            state.task.text += c;
            return StateContext{std::move(state.task)};
        } else if (c == '\n') {
            return StateEnd{std::move(state.task)};
        } else if (c == ' ') {
            return state;
        }

        state.task.text += c;
        return StateText{std::move(state.task)};
    }

    Parser::NextState Parser::on_event(StateCompletion& state, const char& c) {
        if (c == ' ') {
            state.task.is_completion = true;
            return StateBlank{std::move(state.task)};
        }
        state.task.text = "x";
        if (c == '\n') return StateEnd{std::move(state.task)};
        return StateText{std::move(state.task)};
    }

    Parser::NextState Parser::on_event(StatePriority& state, const char& c) {
        if (c == ')') {
            if (state.task.text.length() == 1 && std::isupper(state.task.text[0])) {
                state.task.priority = state.task.text[0];
                state.task.text = "";
                return StatePriorityEnd{std::move(state.task)};
            }
        } else if (c == ' ') {
            state.task.text = "(" + state.task.text;
            return StateBlank{std::move(state.task)};
        } else if (c == '\n') {
            state.task.text = "(" + state.task.text;
            return StateEnd{std::move(state.task)};
        } else if (!state.task.text.empty()) {
            state.task.text = "(" + state.task.text;
            return StateText{std::move(state.task)};
        }

        state.task.text += c;
        return state;
    }

    Parser::NextState Parser::on_event(StatePriorityEnd& state, const char& c) {
        if (c == ' ') return StateBlank{std::move(state.task)};
        else if (c == '\n') return StateEnd{std::move(state.task)};
        state.task.text = "(";
        state.task.text += state.task.priority;
        state.task.text += ")";
        state.task.priority = ' ';
        return StateText{std::move(state.task)};
    }

    Parser::NextState Parser::on_event(StateProject& state, const char& c) {
        if (c == ' ') {
            state.task.text += c;
            state.task.projects.insert(state.project);
            return StateBlank{std::move(state.task)};
        } else if (c == '\n') {
            state.task.projects.insert(state.project);
            return StateEnd{std::move(state.task)};
        }
        state.project += c;
        state.task.text += c;
        return state;
    }

    Parser::NextState Parser::on_event(StateContext& state, const char& c) {
        if (c == ' ') {
            state.task.text += c;
            state.task.contexts.insert(state.context);
            return StateBlank{std::move(state.task)};
        } else if (c == '\n') {
            state.task.contexts.insert(state.context);
            return StateEnd{std::move(state.task)};
        }
        state.context += c;
        state.task.text += c;
        return state;
    }

    Parser::NextState Parser::on_event(StateSpecial& state, const char& c) {
        if (c == ' ') {
            state.task.text += c;
            state.task.specials.insert(std::make_pair(state.key, state.value));
            return StateBlank{std::move(state.task)};
        } else if (c == '\n') {
            state.task.specials.insert(std::make_pair(state.key, state.value));
            return StateEnd{std::move(state.task)};
        }
        state.value += c;
        state.task.text += c;
        return state;
    }
}