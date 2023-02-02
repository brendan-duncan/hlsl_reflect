#include "test.h"

int Test::_results = 0;
int Test::_groupResults = 0;
std::vector<Test::TestGroup *> Test::tests;
std::vector<Test::PendingTest*> Test::_pendingTests;
