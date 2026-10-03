"""Console checks for the course planner. Requires Python 3 and g++."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class PlannerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.build.cleanup)
        cls.program = Path(cls.build.name) / "course_planner"
        subprocess.run(
            [os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra",
             "-pedantic", str(ROOT / "ProjectTwo.cpp"), "-o", str(cls.program)],
            check=True,
        )

    def run_program(self, entries):
        return subprocess.run(
            [str(self.program)], input=entries, text=True,
            capture_output=True, timeout=5, check=True, cwd=ROOT,
        ).stdout

    def test_list_and_lookup(self):
        output = self.run_program("1\ndata/sample_courses.txt\n2\n3\ncs301\n9\n")
        self.assertIn("Course data loaded successfully.", output)
        lines = output.split("Here is a sample schedule:\n", 1)[1].splitlines()[:6]
        self.assertEqual([line.split(",")[0] for line in lines],
                         ["CS101", "CS102", "CS201", "CS202", "CS301", "CS302"])
        self.assertIn("CS301, Software Engineering\nPrerequisites:\n"
                      "CS201, Data Structures\nCS202, Database Fundamentals", output)

    def test_lookup_without_prerequisites_and_missing_course(self):
        output = self.run_program("1\ndata/sample_courses.txt\n3\nCS101\n3\nUNKNOWN\n9\n")
        self.assertIn("CS101, Introduction to Programming\nPrerequisites: None", output)
        self.assertIn("Course UNKNOWN was not found.", output)

    def test_before_loading_and_invalid_menu(self):
        output = self.run_program("2\n3\nabc\n8\n9\n")
        self.assertEqual(output.count("Please load course data first."), 2)
        self.assertIn("Invalid input.", output)
        self.assertIn("8 is not a valid option.", output)

    def test_invalid_files_preserve_loaded_courses(self):
        cases = [
            ("", "contains no courses"),
            ("CS101\n", "Invalid course data"),
            ("CS101,Intro\nCS101,Duplicate\n", "Duplicate course number"),
            ("CS101,Intro,CS101\n", "cannot be its own prerequisite"),
            ("CS101,Intro,CS999\n", "was not found"),
            ("CS101,Intro\nCS102,Next,CS101,CS101\n", "Duplicate prerequisite"),
        ]
        for contents, message in cases:
            with self.subTest(message=message), tempfile.TemporaryDirectory() as folder:
                data = Path(folder) / "courses.txt"
                data.write_text(contents, encoding="utf-8")
                output = self.run_program(
                    "1\ndata/sample_courses.txt\n1\n" + str(data) + "\n3\nCS301\n9\n")
                self.assertIn(message, output)
                self.assertIn("CS301, Software Engineering", output)

    def test_missing_file(self):
        with tempfile.TemporaryDirectory() as folder:
            output = self.run_program("1\n" + str(Path(folder) / "missing.txt") + "\n9\n")
            self.assertIn("could not be opened", output)

    def test_end_of_input(self):
        for entries in ["", "abc\n", "1\n", "1\ndata/sample_courses.txt\n3\n"]:
            with self.subTest(entries=entries):
                self.assertIn("Input ended. Exiting course planner.", self.run_program(entries))


if __name__ == "__main__":
    unittest.main(verbosity=2)
