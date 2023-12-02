#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "task/date.h"
#include "task/parser.h"
#include "task/manager.h"

#include "test_utils.hpp"

TEST_CASE("parser") {
    auto [data, want] = GENERATE(table<std::string, task::Task>({
        { 
            "x (A) Call Mom @phone +family", 
            task::Task{
                .text = "Call Mom @phone +family",
                .is_completion = true,
                .priority = 'A',
                .projects = {"family"},
                .contexts = {"phone"},
            }
        },
        {
            "1900-01-01 Call Mom @phone +family",
            task::Task{
                .text = "Call Mom @phone +family",
                .creation_date = "1900-01-01",
                .projects = {"family"},
                .contexts = {"phone"},
            }
        },
        {
            "x 1900-01-02 1900-01-01 Call Mom @phone +family",
            task::Task{
                .text = "Call Mom @phone +family",
                .is_completion = true,
                .completion_date = "1900-01-02",
                .creation_date = "1900-01-01",
                .projects = {"family"},
                .contexts = {"phone"},
            }
        },
        {
            "Call Mom @phone +family due:1900-01-02",
            task::Task{
                .text = "Call Mom @phone +family due:1900-01-02",
                .projects = {"family"},
                .contexts = {"phone"},
                .specials = {{"due", "1900-01-02"}},
            }
        },
    }));
    task::Parser parser;
    auto res = parser.parse(data);
    REQUIRE(res.has_value());

    auto task = std::move(res.value());
    REQUIRE(task.text == want.text);
    REQUIRE(task.is_completion == want.is_completion);
    REQUIRE(task.priority == want.priority);
    REQUIRE(task.completion_date == want.completion_date);
    REQUIRE(task.creation_date == want.creation_date);
    std::for_each(want.projects.begin(), want.projects.end(), 
        [&task](auto s) { REQUIRE(task.projects.contains(s)); });
    std::for_each(want.contexts.begin(), want.contexts.end(), 
        [&task](auto s) { REQUIRE(task.contexts.contains(s)); });
    std::for_each(want.specials.begin(), want.specials.end(), 
        [&task](auto s) { REQUIRE(task.specials[s.first] == s.second); });
}

std::optional<std::tm> parse_time(const std::string& str) {
    std::tm date{};
    std::istringstream ss{str};
    ss >> std::get_time(&date, task::date::FORMAT.c_str());
    if (ss.fail()) return std::nullopt;
    return date;
}

TEST_CASE("date") {
    std::time_t now_tt = std::time(0);
    std::tm now = *std::localtime(&now_tt);

    SECTION("valid") {
        CHECK(task::date::valid("1970-01-01"));
        CHECK(!task::date::valid("1970/01/01"));
    }

    SECTION("today") {
        auto opt = parse_time(task::date::today());
        REQUIRE(opt.has_value());
        auto tod = std::move(opt.value());
        CHECK(tod.tm_year == now.tm_year);
        CHECK(tod.tm_mon == now.tm_mon);
        CHECK(tod.tm_mday == now.tm_mday);
    }

    SECTION("tomorrow") {
        auto opt = parse_time(task::date::tomorrow());
        REQUIRE(opt.has_value());
        auto tom = std::move(opt.value());
        CHECK(tom.tm_year == now.tm_year);
        CHECK(tom.tm_mon == now.tm_mon);
        CHECK(tom.tm_mday == now.tm_mday + 1);
    }

    SECTION("sunday") {
        auto opt = parse_time(task::date::sunday());
        REQUIRE(opt.has_value());
        auto sun = std::move(opt.value());
        CHECK(sun.tm_wday == 0);
    }

    SECTION("month last day") {
        auto opt = parse_time(task::date::month_last_day());
        REQUIRE(opt.has_value());
        auto mld = std::move(opt.value());
        CHECK(mld.tm_year == now.tm_year);
        CHECK(mld.tm_mon == now.tm_mon);
        CHECK(28 <= mld.tm_mday);
        CHECK(mld.tm_mday <= 31);
    }
}

TEST_CASE("[smoke] add task") {
    TempFile file(".txt");
    task::Manager::instance().config().file = file.filename().string();

    std::string data = "new task";
    std::string want = task::date::today() + " " + data + "\n";

    bool ok = task::Manager::instance().add_task(data);
    REQUIRE(ok);

    CHECK(file.contents() == want);
}

TEST_CASE("[smoke] done task") {
    std::string today = task::date::today();
    std::string data = today + " new task\n";

    TestFile file;
    file.main().open();
    file.main().file() << data;
    file.main().close();
    task::Manager::instance().config().file = file.main().filename().string();

    bool ok = task::Manager::instance().done_task(1);
    REQUIRE(ok);

    CHECK(file.main().contents() == "x " + today + " " + data);
    CHECK(file.bak().contents() == data);
}

TEST_CASE("[smoke] delete task") {
    std::string data = "new task\n";

    TestFile file;
    file.main().open();
    file.main().file() << data;
    file.main().close();
    task::Manager::instance().config().file = file.main().filename().string();

    bool ok = task::Manager::instance().delete_task(1);
    REQUIRE(ok);

    CHECK(file.main().contents() == "");
    CHECK(file.bak().contents() == data);
}

TEST_CASE("[smoke] archive task") {
    std::string data = "x new task\n";

    TestFile file;
    file.main().open();
    file.main().file() << data;
    file.main().close();
    task::Manager::instance().config().file = file.main().filename().string();

    bool ok = task::Manager::instance().archive_task();
    REQUIRE(ok);

    CHECK(file.main().contents() == "");
    CHECK(file.bak().contents() == data);
}

TEST_CASE("[smoke] list task") {
    TempFile file(".txt");
    file.open();
    file.file() << "task 1\n";
    file.file() << "(B) task 2\n";
    file.file() << "(A) task 3\n";
    file.file() << "(A) task 4\n";
    file.close();
    task::Manager::instance().config().file = file.filename().string();

    std::string want;
    want += "3 (A) task 3\n";
    want += "4 (A) task 4\n";
    want += "2 (B) task 2\n";
    want += "1 task 1\n";

    CapturingStdout out;
    bool ok = task::Manager::instance().list_task();
    REQUIRE(ok);

    CHECK(out.contents() == want);
}