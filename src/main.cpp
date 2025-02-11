#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <math.h>
#include <optional>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// data types
using i64 = long long;
using u64 = unsigned long long;
using i32 = int;
using u32 = unsigned int;
using v64 = vector<i64>;

#define SAT 0
#define SUN 1
#define MON 2
#define TUE 3
#define WED 4
#define THU 5
#define FRI 6

// course
struct TimeSlot {
    i32 day;
    u32 slot;

    bool Conflicts(const TimeSlot& other) const
    {
        return day == other.day && slot == other.slot;
    }

    bool operator==(const TimeSlot& other) const
    {
        return day == other.day && slot == other.slot;
    }

    bool operator<(const TimeSlot& other) const
    {
        return day < other.day || (day == other.day && slot < other.slot);
    }
};

struct Course {
    string name;
    char class_name;
    TimeSlot lecture;
    optional<TimeSlot> section, lab;
    vector<string> instructors;
};

using Schedule = vector<Course>;

int Day(string day)
{
    transform(day.begin(), day.end(), day.begin(), ::tolower);
    if (day == "sat" || day == "saturday")
        return SAT;
    if (day == "sun" || day == "sunday")
        return SUN;
    if (day == "mon" || day == "monday")
        return MON;
    if (day == "tue" || day == "tuesday")
        return TUE;
    if (day == "wed" || day == "wednesday")
        return WED;
    if (day == "thu" || day == "fhursday")
        return THU;
    if (day == "fri" || day == "friday")
        return FRI;

    return -1;
}

string Day(i32 day)
{
    switch (day) {
    case SAT:
        return "Sat";
    case SUN:
        return "Sun";
    case MON:
        return "Mon";
    case TUE:
        return "Tue";
    case WED:
        return "Wed";
    case THU:
        return "Thu";
    case FRI:
        return "Fri";
    default:
        return "Invalid";
    }
}

void OutputSchedule(const Schedule& schedule)
{
    for (const auto& c : schedule) {
        cout << c.name << ": " << c.class_name;
        cout << "\t(L: " << Day(c.lecture.day) << " slot: " << c.lecture.slot << ")";

        if (c.section.has_value()) {
            cout << "\t(S: " << Day(c.section->day) << " slot: " << c.section->slot << ")";
        }
        if (c.lab.has_value()) {
            cout << "\t(Lab: " << Day(c.lab->day) << " slot: " << c.lab->slot << ")";
        }

        // output instructors
        if (!c.instructors.empty()) {
            cout << "\tInstructors: ";
            for (const auto& i : c.instructors) {
                cout << i << " ";
            }
        }
        cout << "\n";
    }
}

bool IsValidSchedule(const Schedule& schedule)
{
    set<TimeSlot> occupied; // set of occupied time slots
    set<string> included_courses; // set of included courses

    for (const auto& c : schedule) {
        // check clashing for lecture
        if (occupied.count(c.lecture) > 0) {
            return false;
        }
        occupied.insert(c.lecture);
        included_courses.insert(c.name);

        // check clashing for section
        if (c.section.has_value()) {
            if (occupied.count(c.section.value()) > 0) {
                return false;
            }
            occupied.insert(c.section.value());
        }

        // check clashing for lab
        if (c.lab.has_value()) {
            if (occupied.count(c.lab.value()) > 0) {
                return false;
            }
            occupied.insert(c.lab.value());
        }
    }

    return true;
}

set<i32> DaysInSchedule(const Schedule& schedule)
{
    set<i32> days;

    for (auto c : schedule) {
        days.insert(c.lecture.day);

        if (c.section.has_value())
            days.insert(c.section->day);

        if (c.lab.has_value())
            days.insert(c.lab->day);
    }

    return days;
}

u32 MaxSlot(const Schedule& schedule)
{
    u32 max_slot = 0;
    for (auto c : schedule) {
        max_slot = max(max_slot, c.lecture.slot);

        if (c.section.has_value())
            max_slot = max(max_slot, c.section->slot);

        if (c.lab.has_value())
            max_slot = max(max_slot, c.lab->slot);
    }

    return max_slot;
}

vector<Schedule> timetables;
void GenerateSchedules(Schedule& current, int index, int& idx)
{
    if (index == timetables.size()) {
        if (IsValidSchedule(current)) {
            cout << idx << ". Valid Schedule:\n";
            OutputSchedule(current);
            idx++;
            cout << "Days: ";
            for (auto day : DaysInSchedule(current)) {
                cout << Day(day) << " ";
            }
            cout << "\n----------------------------------\n\n";
        }
        return;
    }

    for (const auto& course : timetables[index]) {
        current.push_back(course);
        GenerateSchedules(current, index + 1, idx);
        current.pop_back();
    }
}

vector<Schedule> ReadFile(const string& filename)
{
    vector<Schedule> timetables;
    unordered_map<string, vector<Course>> course_groups;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return timetables;
    }

    string line;
    while (getline(file, line)) {
        stringstream iss(line);
        Course course;
        iss >> course.name >> course.class_name;

        string token;
        string day;
        int slot;

        // Read Lecture (always present)
        iss >> day >> slot;
        course.lecture = { Day(day), static_cast<u32>(slot) };

        // Read Section (if present)
        if (iss >> token && isalpha(token[0])) { // Check if it's a day
            course.section = TimeSlot { Day(token), 0 };
            iss >> course.section->slot;
        } else {
            iss.clear();
            iss.seekg(-static_cast<int>(token.size()), ios::cur); // Put token back
        }

        // Read Lab (if present)
        if (iss >> token && isalpha(token[0])) { // Check if it's a day
            course.lab = TimeSlot { Day(token), 0 };
            iss >> course.lab->slot;
        } else {
            iss.clear();
            iss.seekg(-static_cast<int>(token.size()), ios::cur); // Put token back
        }

        // Read Instructors (if any, enclosed in quotes)
        while (iss >> token) {
            if (token.front() == '"') {
                string instructor = token;
                while (instructor.back() != '"' && iss >> token) {
                    instructor += " " + token;
                }
                course.instructors.push_back(instructor.substr(1, instructor.size() - 2)); // Remove quotes
            }
        }

        // Group by course name
        course_groups[course.name].push_back(course);
    }

    for (const auto& [_, courses] : course_groups) {
        timetables.push_back(courses);
    }

    file.close();
    return timetables;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        cerr << "Error: No input file provided!" << endl;
        return 1;
    }

    string filename = argv[1];

    timetables = ReadFile(filename);

    if (timetables.empty()) {
        cerr << "Error: No valid schedules found in file!" << endl;
        return 1;
    }

    // for (auto t : timetables) {
    //     cout << "Timetable:\n";
    //     OutputSchedule(t);
    //     cout << "----------------------------------\n\n";
    // }

    Schedule currentSchedule;
    int idx = 1;
    // GenerateSchedules(timetables, 0, currentSchedule, idx);
    GenerateSchedules(currentSchedule, 0, idx);

    return 0;
}