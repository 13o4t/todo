#include "task/date.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace task::date {

    bool valid(const std::string& date) {
        std::tm tm{};
        std::istringstream ss{date};
        ss >> std::get_time(&tm, FORMAT.c_str());
        return !ss.fail();
    }

    std::string format_time(const std::tm& tm) {
        std::ostringstream ss;
        ss << std::put_time(&tm, FORMAT.c_str());
        return ss.str();
    }

    std::optional<std::string> convert(const std::string& date) {
        if (valid(date)) return date;

        if (date == "today" || date == "tod") return today();
        if (date == "tomorrow" || date == "tom") return tomorrow();
        if (date == "sunday" || date == "sun") return sunday();
        if (date == "monday" || date == "mon") return monday();
        if (date == "tuesday" || date == "tue") return tuesday();
        if (date == "wednesday" || date == "wed") return wednesday();
        if (date == "thursday" || date == "thu") return thursday();
        if (date == "friday" || date == "fri") return friday();
        if (date == "saturday" || date == "sat") return saturday();
        if (date == "week") return sunday();
        if (date == "month") return month_last_day();

        return std::nullopt;
    }

    std::string today() {
        std::time_t now = std::time(0);
        return format_time(*std::localtime(&now));
    }

    std::string tomorrow() {
        std::time_t now = std::time(0);
        std::time_t tom = now + 24 * 60 * 60;
        return format_time(*std::localtime(&tom));
    }

    std::string next_day_in_week(const int index) {
        std::time_t now = std::time(0);
        std::tm now_tm = *std::localtime(&now);

        int diff = index - now_tm.tm_wday;
        if (diff <= 0) diff += 7;
        std::time_t day = now + diff * 24 * 60 * 60;
        return format_time(*std::localtime(&day));
    }

    std::string sunday() {
        return next_day_in_week(0);
    }

    std::string monday() {
        return next_day_in_week(1);
    }

    std::string tuesday() {
        return next_day_in_week(2);
    }

    std::string wednesday() {
        return next_day_in_week(3);
    }

    std::string thursday() {
        return next_day_in_week(4);
    }

    std::string friday() {
        return next_day_in_week(5);
    }

    std::string saturday() {
        return next_day_in_week(6);
    }

    std::string month_last_day() {
        std::time_t now = std::time(0);
        std::tm now_tm = *std::localtime(&now);

        int year = now_tm.tm_year + 1900;
        int month = now_tm.tm_mon + 1;
        int last_day = 0;

        if (month == 2) {
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                last_day = 29;
            } else {
                last_day = 28;
            }
        } else if (month == 4 || month == 6 || month == 9 || month == 11) {
            last_day = 30;
        } else {
            last_day = 31;
        }

        std::tm last_day_tm{};
        last_day_tm.tm_year = now_tm.tm_year;
        last_day_tm.tm_mon = now_tm.tm_mon;
        last_day_tm.tm_mday = last_day;
        return format_time(last_day_tm);
    }
}