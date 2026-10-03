// Name: Jessica Coppola
// Course: CS 300
// Assignment: Project Two
// Date: August 11, 2026
// Description: ABCU Advising Assistance Program

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

// Stores the information for one course.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Removes whitespace from the beginning and end of a string.
string trim(const string& text) {
    const string whitespace = " \t\r\n";
    size_t first = text.find_first_not_of(whitespace);

    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

// Converts course numbers to uppercase.
string toUpper(string text) {
    transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char character) {
            return static_cast<char>(toupper(character));
        }
    );

    return text;
}

// Separates a comma-delimited line into individual fields.
vector<string> splitLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream lineStream(line);

    while (getline(lineStream, field, ',')) {
        fields.push_back(trim(field));
    }

    return fields;
}

// Stores courses in alphanumeric order by course number.
class BinarySearchTree {
private:
    struct Node {
        Course course;
        unique_ptr<Node> left;
        unique_ptr<Node> right;

        explicit Node(const Course& courseData)
            : course(courseData), left(nullptr), right(nullptr) {
        }
    };

    unique_ptr<Node> root;

    // Recursively inserts a course into the tree.
    void addNode(unique_ptr<Node>& currentNode,
                 const Course& course) {
        if (currentNode == nullptr) {
            currentNode = unique_ptr<Node>(new Node(course));
        } else if (course.courseNumber <
                   currentNode->course.courseNumber) {
            addNode(currentNode->left, course);
        } else {
            addNode(currentNode->right, course);
        }
    }

    // In-order traversal prints courses from lowest to highest.
    void printInOrder(const Node* currentNode) const {
        if (currentNode == nullptr) {
            return;
        }

        printInOrder(currentNode->left.get());

        cout << currentNode->course.courseNumber
             << ", "
             << currentNode->course.courseTitle
             << endl;

        printInOrder(currentNode->right.get());
    }

public:
    // Adds one course to the tree.
    void insert(const Course& course) {
        addNode(root, course);
    }

    // Removes all courses from the tree.
    void clear() {
        root.reset();
    }

    // Determines whether the tree contains any courses.
    bool empty() const {
        return root == nullptr;
    }

    // Prints all courses in alphanumeric order.
    void printCourseList() const {
        printInOrder(root.get());
    }

    // Searches for a course by course number.
    const Course* findCourse(const string& courseNumber) const {
        const Node* currentNode = root.get();

        while (currentNode != nullptr) {
            if (courseNumber == currentNode->course.courseNumber) {
                return &currentNode->course;
            }

            if (courseNumber < currentNode->course.courseNumber) {
                currentNode = currentNode->left.get();
            } else {
                currentNode = currentNode->right.get();
            }
        }

        return nullptr;
    }
};

// Reads and validates course data before loading it into the tree.
bool loadCourses(const string& fileName,
                 BinarySearchTree& courses) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: The course data file could not be opened."
             << endl;
        return false;
    }

    vector<Course> parsedCourses;
    unordered_set<string> courseNumbers;
    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        lineNumber++;

        // Ignore blank lines.
        if (trim(line).empty()) {
            continue;
        }

        vector<string> fields = splitLine(line);

        // Every line must contain a course number and title.
        if (fields.size() < 2 ||
            fields[0].empty() ||
            fields[1].empty()) {

            cout << "Error: Invalid course data on line "
                 << lineNumber
                 << ". Each course requires a number and title."
                 << endl;

            return false;
        }

        Course course;
        course.courseNumber = toUpper(fields[0]);
        course.courseTitle = fields[1];

        // Do not allow duplicate course numbers.
        if (courseNumbers.find(course.courseNumber) !=
            courseNumbers.end()) {

            cout << "Error: Duplicate course number "
                 << course.courseNumber
                 << " on line "
                 << lineNumber
                 << "."
                 << endl;

            return false;
        }

        courseNumbers.insert(course.courseNumber);

        // Remaining nonempty fields contain prerequisite numbers.
        for (size_t index = 2; index < fields.size(); index++) {
            if (!fields[index].empty()) {
                course.prerequisites.push_back(
                    toUpper(fields[index])
                );
            }
        }

        parsedCourses.push_back(course);
    }

    inputFile.close();

    if (parsedCourses.empty()) {
        cout << "Error: The course data file contains no courses."
             << endl;
        return false;
    }

    // Validate prerequisites before modifying the current tree.
    for (const Course& course : parsedCourses) {
        unordered_set<string> seenPrerequisites;

        for (const string& prerequisite :
             course.prerequisites) {

            if (prerequisite == course.courseNumber) {
                cout << "Error: "
                     << course.courseNumber
                     << " cannot be its own prerequisite."
                     << endl;

                return false;
            }

            if (courseNumbers.find(prerequisite) ==
                courseNumbers.end()) {

                cout << "Error: Prerequisite "
                     << prerequisite
                     << " for "
                     << course.courseNumber
                     << " was not found."
                     << endl;

                return false;
            }

            if (seenPrerequisites.find(prerequisite) !=
                seenPrerequisites.end()) {

                cout << "Error: Duplicate prerequisite "
                     << prerequisite
                     << " for "
                     << course.courseNumber
                     << "."
                     << endl;

                return false;
            }

            seenPrerequisites.insert(prerequisite);
        }
    }

    // Replace existing data only after validation succeeds.
    courses.clear();

    for (const Course& course : parsedCourses) {
        courses.insert(course);
    }

    cout << "Course data loaded successfully." << endl;
    return true;
}

// Prints one course and its prerequisite numbers and titles.
void printCourseInformation(const BinarySearchTree& courses,
                            string requestedNumber) {
    requestedNumber = toUpper(trim(requestedNumber));

    const Course* course =
        courses.findCourse(requestedNumber);

    if (course == nullptr) {
        cout << "Course "
             << requestedNumber
             << " was not found."
             << endl;

        return;
    }

    cout << course->courseNumber
         << ", "
         << course->courseTitle
         << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;

    for (const string& prerequisiteNumber :
         course->prerequisites) {

        const Course* prerequisite =
            courses.findCourse(prerequisiteNumber);

        if (prerequisite != nullptr) {
            cout << prerequisite->courseNumber
                 << ", "
                 << prerequisite->courseTitle
                 << endl;
        }
    }
}

// Displays the available menu options.
void displayMenu() {
    cout << endl;
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    BinarySearchTree courses;
    bool dataLoaded = false;
    int menuChoice = 0;

    cout << "Welcome to the Course Planner." << endl;

    while (menuChoice != 9) {
        displayMenu();

        // Reject input that is not an integer.
        if (!(cin >> menuChoice)) {
            if (cin.eof()) {
                cout << "\nInput ended. Exiting course planner." << endl;
                break;
            }
            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            cout << "Invalid input. Please enter 1, 2, 3, or 9."
                 << endl;

            continue;
        }

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        switch (menuChoice) {
            case 1: {
                string fileName;

                cout << "Enter the course data file name: ";
                if (!getline(cin, fileName)) {
                    cout << "\nInput ended. Exiting course planner." << endl;
                    return 0;
                }

                if (trim(fileName).empty()) {
                    cout << "Error: A file name is required."
                         << endl;
                } else {
                    bool loadSuccessful =
                        loadCourses(trim(fileName), courses);

                    if (loadSuccessful) {
                        dataLoaded = true;
                    }
                }

                break;
            }

            case 2:
                if (!dataLoaded) {
                    cout << "Please load course data first."
                         << endl;
                } else {
                    cout << "Here is a sample schedule:"
                         << endl;

                    courses.printCourseList();
                }

                break;

            case 3: {
                if (!dataLoaded) {
                    cout << "Please load course data first."
                         << endl;
                } else {
                    string courseNumber;

                    cout << "What course would you like to know about? ";
                    if (!getline(cin, courseNumber)) {
                        cout << "\nInput ended. Exiting course planner." << endl;
                        return 0;
                    }

                    if (trim(courseNumber).empty()) {
                        cout << "Error: A course number is required."
                             << endl;
                    } else {
                        printCourseInformation(
                            courses,
                            courseNumber
                        );
                    }
                }

                break;
            }

            case 9:
                cout << "Thank you for using this course planner!"
                     << endl;
                break;

            default:
                cout << menuChoice
                     << " is not a valid option."
                     << endl;
                break;
        }
    }

    return 0;
}