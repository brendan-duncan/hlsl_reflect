#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class Test {
public:
  Test(const std::string& name, void (*test)()) {
    Test::addPendingTest(name, test);
  }

  struct TestGroup {
    std::string name;
    int results = 0;
    int saved_results = 0;

    TestGroup(const std::string& name)
      : name(name) { }

    ~TestGroup() {
      if (results == 0) {
        std::cout << "TEST " << name << " PASSED" << std::endl;
      } else {
        std::cout << "TEST " << name << " FAILED" << std::endl;
      }
    }
  };

  static std::vector<TestGroup *> tests;

  static const char* dataPath() {
    return TOSTRING(DATA_PATH);
  }

  static std::string dataPath(const char* path) {
    return std::string(dataPath()) + path;
  }

  static void startTest(const std::string& name) {
    tests.push_back(new TestGroup(name));
    tests.back()->saved_results = Test::getGroupResults();
  }

  static void finishTest() {
    if (!tests.empty()) {
      tests.back()->results = Test::getGroupResults();
      Test::setGroupResults(tests.back()->saved_results);
      delete tests.back();
      tests.pop_back();
      if (tests.empty()) {
        Test::clearGroupResults();
      }
    }
  }

  static int getResults() {
    return _results;
  }

  static void setResults(int value) {
    _results = value;
  }

  static void clearGroupResults() {
    _groupResults = 0;
  }

  static void setGroupResults(int value) {
    _groupResults = value;
  }

  static int getGroupResults() {
    return _groupResults;
  }

  static void pass() {
  }

  static void fail() {
    _groupResults = 1;
    _results = 1;
  }

  static void fail(const char* file, int line, const std::string& message="") {
    std::cout << "TEST FAILED: " << file << ":" << line <<":" << message << std::endl;
    _results = 1;
    _groupResults = 1;
  }

  template<typename T>
  static inline void testTrue(const T& result, const char* file, int line) {
    if (result) {
      Test::pass();
    } else {
      Test::fail(file, line,
          (std::stringstream() << "Expected true Found " << result).str());
    }
  }

  template<typename T>
  static inline void testFalse(const T& result, const char* file, int line) {
    if (!result) {
      Test::pass();
    } else {
      Test::fail(file, line,
          (std::stringstream() << "Expected false Found " << result).str());
    }
  }

  template<typename X, typename Y>
  static inline void testEquals(const X& result, const Y& expected,
                                const char* file, int line) {
    if (result == expected) {
      Test::pass();
    } else {
      Test::fail(file, line,
          (std::stringstream() << "Expected " << result << " == " << expected).str());
    }
  }

  template<typename X, typename Y>
  static inline void testNotEquals(const X& result, const Y& expected,
                                   const char* file, int line) {
    if (result != expected) {
      Test::pass();
    } else {
      Test::fail(file, line,
          (std::stringstream() << "Expected " << result << " != " << expected).str());
    }
  }

  template<typename X>
  static inline void testIsNull(const X* result, const char* file, int line) {
    if (result == nullptr) {
      Test::pass();
    } else {
      Test::fail(file, line,
          (std::stringstream() << "Expected " << result << " == NULL").str());
    }
  }

  template<typename X>
  static inline void testNotNull(const X* result, const char* file, int line) {
    if (result != nullptr) {
      Test::pass();
    } else {
      Test::fail(file, line,
          (std::stringstream() << "Expected " << result << " != NULL").str());
    }
  }

  template<typename X, typename Y>
  static inline void testFloatEquals(const X& result, const Y& expected,
                                     float epsilon,
                                     const char* file, int line) {
    if (fabs(result - expected) <= epsilon) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " == " + expected);
    }
  }

  template<typename X, typename Y>
  static inline void testFloatNotEquals(const X& result, const Y& expected,
                                        float epsilon,
                                        const char* file, int line) {
    if (fabs(result - expected) > epsilon) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " != " + expected);
    }
  }

  static inline void testStringEquals(const char* result, const char* expected,
                                      const char* file, int line) {
    if (strcmp(result, expected) == 0) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " == " + expected);
    }
  }

  static inline void testStringNotEquals(const char* result, const char* expected,
                                         const char* file, int line) {
    if (strcmp(result, expected) != 0) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " != " + expected);
    }
  }

  template<typename X, typename Y>
  static inline void testLess(const X& result, const Y& expected,
                              const char* file, int line) {
    if (result < expected) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " < " + expected);
    }
  }

  template<typename X, typename Y>
  static inline void testLEqual(const X& result, const Y& expected,
                                const char* file, int line) {
    if (result <= expected) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " <= " + expected);
    }
  }

  template<typename X, typename Y>
  static inline void testGreater(const X& result, const Y& expected,
                                 const char* file, int line) {
    if (result > expected) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " > " + expected);
    }
  }

  template<typename X, typename Y>
  static inline void testGEqual(const X& result, const Y& expected,
                                const char* file, int line) {
    if (result >= expected) {
      Test::pass();
    } else {
      Test::fail(file, line,
          std::string("Expected ") + result + " >= " + expected);
    }
  }

  static void addPendingTest(const std::string& name, void (*test)()) {
    PendingTest* pendingTest = new PendingTest();
    pendingTest->name = name;
    pendingTest->test = test;
    _pendingTests.push_back(pendingTest);
  }

  static void runPendingTests() {
    for (auto it = _pendingTests.begin(); it != _pendingTests.end(); ++it) {
      PendingTest* test = *it;
      Test::startTest(test->name);
      test->test();
      Test::finishTest();
      delete test;
    }
    _pendingTests.clear();
  }

private:
  struct PendingTest {
    std::string name;
    void (*test)();
  };

  static int _groupResults;
  static int _results;
  static std::vector<PendingTest*> _pendingTests;
};

#define TEST_DATA_PATH(s) \
  Test::dataPath(s).c_str()


#undef TOSTRING
#undef STRINGIFY

#define TEST(name, test)\
  static Test test_##name(#name, test)

#define TEST_RESULTS() Test::getResults()

#define TEST_START(name) Test::startTest(name); {

#define TEST_END() } Test::finishTest();

#define TEST_TRUE(result) \
  Test::testTrue((result), __FILE__, __LINE__)

#define TEST_FALSE(result) \
  Test::testFalse((result), __FILE__, __LINE__)

#define TEST_IS_NULL(result) \
  Test::testIsNull((result), __FILE__, __LINE__)

#define TEST_NOT_NULL(result) \
  Test::testNotNull((result), __FILE__, __LINE__)

#define TEST_EQUALS(result, expected) \
  Test::testEquals((result), (expected), __FILE__, __LINE__)

#define TEST_NOT_EQUALS(result, expected) \
  Test::testNotEquals((result), (expected), __FILE__, __LINE__)

#define TEST_FLOAT_EQUALS(result, expected, epsilon) \
  Test::testFloatEquals((result), (expected), (epsilon), __FILE__, __LINE__)

#define TEST_FLOAT_NOT_EQUALS(result, expected, epsilon) \
  Test::testFloatNotEquals((result), (expected), (epsilon), __FILE__, __LINE__)

#define TEST_STRING_EQUALS(result, expected) \
  Test::testStringEquals((result), (expected), __FILE__, __LINE__)

#define TEST_STRING_NOT_EQUALS(result, expected) \
  Test::testStringNotEquals((result), (expected), __FILE__, __LINE__)

#define TEST_LESS(result, expected) \
  Test::testLess((result), (expected), __FILE__, __LINE__)

#define TEST_LEQUAL(result, expected) \
  Test::testLEqual((result), (expected), __FILE__, __LINE__)

#define TEST_GREATER(result, expected) \
  Test::testGreater((result), (expected), __FILE__, __LINE__)

#define TEST_GEQUAL(result, expected) \
  Test::testGEqual((result), (expected), __FILE__, __LINE__)
