# Scheduler App.

## Usage:

1. build the app using a C++ compiler (MSVC, Clang, GCC)
```bash
g++ src/main.cpp -o out
```

2. run the app and pass the schedule data as the first arguement and stream it into a file to output the generated permutations to it
```bash
./out schedule.txt > generated.txt
```

## Data Format:
each row is as follows: [] is optional
```bash
course_name class lecture_day lecture_slot [section_day] [section_slot] [lab_day] [lab_slot] "Dr. John Doe" "Eng. Jane Doe"
```
NOTE: a course can have a lecture only withouut a section or a lab. however a lab requires the course to have a lecture and a section.

NOTE: slot is 1 through 5 which translates to: 1 is 8:30 to 10:10 ...... 5 is 4:30 to 6:10

### Example Data:

```txt
dsa B sun 2 mon 1 thu 2 "dr. Jane Doe"
web B sun 1 mon 2
swe B sun 3 mon 3 thu 1
arc B sun 4 mon 4 thu 3
```
