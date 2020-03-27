#include <sstream>

#include "../cute/cute.h"
#include "../cute/ide_listener.h"
#include "../cute/xml_listener.h"
#include "../cute/cute_runner.h"

#include "../../lib/include/pwire-server-lib.h"

void thisIsARunningTest() {
  ASSERT(true);
}

void thisIsALibraryTest() {
  std::ostringstream out { };
  out << "Hello World";
  ASSERT_EQUAL("Hello World", out.str());
}

bool runAllTests(int argc, char const *argv[]) {
  cute::suite s;
  // TODO(ckirchme) add your test here
  s.push_back(CUTE(thisIsALibraryTest));
  s.push_back(CUTE(thisIsARunningTest));
  cute::xml_file_opener xmlfile(argc, argv);
  cute::xml_listener<cute::ide_listener<> >  lis(xmlfile.out);
  bool success = cute::makeRunner(lis, argc, argv)(s, "AllTests");
  return success;
}

int main(int argc, char const *argv[]) {
  return runAllTests(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
