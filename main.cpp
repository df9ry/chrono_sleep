#include "version.hpp"
#include "getopt.h"

#include <iostream>
#include <iomanip>
#include <regex>
#include <thread>
#include <ctime>
#include <chrono>
#include <ratio>

using namespace std;

static int verbose{0};

static void help(ostream &os, const char *name)
{
    os << "This is " << APP_NAME << " version " << APP_VERSION
       << ", Copyright (C) by " << APP_COPYRIGHT
       << " - see: " << APP_WEBSITE
       << endl;
    os << "Usage: " << name << endl
       << "\t-d <unsigned dec>[ms|s|m|h] . Sleep duration" << endl
       << "\t-h .......................... Print help (this message)" << endl
       << "\t-t [hh[:mm[:ss]]] ........... Sleep to timepoint" << endl
       << "\t-v .......................... Verbose" << endl;
}

static regex reg_duration { R"(^(\d*)(ms|s|m|h)$)" };

static void do_sleep_for(const string &duration)
{
    smatch sm;
    if (!regex_match(duration, sm, reg_duration))
        throw runtime_error("Invalid duration string \"" + duration + "\"!");
    int num{stoi(sm[1].str())};
    string base{sm[2].str()};
    if (verbose)
        cerr << "Sleep for " << num << base << endl;
    if (base == "ms")
        this_thread::sleep_for(chrono::milliseconds(num));
    else if (base == "s")
        this_thread::sleep_for(chrono::seconds(num));
    else if (base == "m")
        this_thread::sleep_for(chrono::minutes(num));
    else if (base == "h")
        this_thread::sleep_for(chrono::hours(num));
    else
        throw runtime_error("Invalid duration base \"" + base + "\"!");
}

static regex reg_timepoint { R"(^(\d{2})(:(\d{2}))?(:(\d{2}))?$)" };

static void do_sleep_until(const string &timepoint)
{
    smatch sm;
    if (!regex_match(timepoint, sm, reg_timepoint))
        throw runtime_error("Invalid timepoint string \"" + timepoint + "\"!");
    int hour{stoi(sm[1].str())};
    string _min{sm[3].str()};
    string _sec(sm[5].str());
    int min{_min.empty() ? 0 : stoi(_min)};
    int sec{_sec.empty() ? 0 : stoi(_sec)};
    auto now{chrono::system_clock::now()};
    const time_t tt{chrono::system_clock::to_time_t(now)};
    struct tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &tt);
#else
    localtime_r(&tt, &tm_now);
#endif
    struct tm tm_then{tm_now};
    tm_then.tm_hour = hour;
    tm_then.tm_min  = min;
    tm_then.tm_sec  = sec;
    std::time_t _then{mktime(&tm_then)};
    auto then{chrono::system_clock::from_time_t(_then)};
    if (!((hour > tm_now.tm_hour) || (min > tm_now.tm_min) || (sec > tm_now.tm_sec))) {
        then += chrono::days(1);
    }
    if (verbose) {
        const time_t tt_then{chrono::system_clock::to_time_t(then)};
#ifdef _WIN32
        localtime_s(&tm_then, &tt_then);
#else
        localtime_r(&tt_then, &tm_then);
#endif
        cerr << "Sleep until "
             << dec << setw(4) << setfill('0') << tm_then.tm_year + 1900 << "-"
             << dec << setw(2) << setfill('0') << tm_then.tm_mon + 1 << "-"
             << dec << setw(2) << setfill('0') << tm_then.tm_mday << " "
             << dec << setw(2) << setfill('0') << tm_then.tm_hour << ":"
             << dec << setw(2) << setfill('0') << tm_then.tm_min << ":"
             << dec << setw(2) << setfill('0') << tm_then.tm_sec << endl;
    }
    this_thread::sleep_until(then);
}

int main(int argc, char *argv[])
{
    int option{0};
    bool good{true};
    // Get options:
    while ((option = getopt(argc, argv, "d:ht:v")) >= 0) {
        try {
            switch (option) {
            case 'd':
                do_sleep_for(optarg);
                break;
            case 'h':
                help(cout, argv[0]);
                break;
            case 't':
                do_sleep_until(optarg);
                break;
            case 'v':
                ++verbose;
                break;
            default:
                help(cerr, argv[0]);
                throw runtime_error("Invalid option " + to_string(option));
            } // end switch //
        }
        catch (exception &ex) {
            cerr << ex.what() << endl;
            good = false;
        }
    } // end while //
    return good ? EXIT_SUCCESS : EXIT_FAILURE;
}
