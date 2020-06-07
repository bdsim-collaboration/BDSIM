Testing
*******

Unit Tests
==========

* Unit tests test that the code runs with the chosen input and that a particular bit of code is executed
  without problem. They do not necessarily guarantee the correct result. e.g. a quadrupole unit test may
  track a particle through a quadrupole successfully (pass) but doesn't guarantee the action on the particle
  was correct.
* The unit tests make sure we have good code 'coverage' where we have an example that is regularly run
  that uses a certain bit of the code.
* Any **new feature** must have a test (or multiple) defined that exploits and demonstrates it fully.

BDSIM makes use of CTest which uses tests defined in the many CMakeLists.txt as part of the CMake build system.
Every example is also a unit test.

Running
-------

Recommended: ::

  ctest -LE LONG -j4

Generally, from a build directory of BDSIM, the following command can be run: ::

  ctest

This will run all available tests in the build. At the time of writing this is approximately 800 unit tests
and takes around 15 minutes on the developers computer to run with 6 threads.

Various additional options can be used: ::

  ctest -j4

Run with 4 threads.  Note, even if you have 4 cores and hyperthreading, empirically it has been found
that the tests will run more slowly than single threaded if close to the maximum of the computers. It
is recommended to only use the number of cores available as a maximum, i.e. ignore hyperthreading. ::

  ctest -LE LONG -j4

LE = label exclude. Exclude any tests with the 'label' (defined in the test macro) as 'LONG' (our BDSIM defined label). ::

  ctest -R mytest

Run tests matching "mytest" in the name (this can be a regular expression). ::

  ctest -R example-collimation-hits-minke -VV

Run tests matching the name "example-collimation-hits-minke" and show (verbosely) all screen out from the test.
In this case it is therefore prudent to not use :code:`-jN` and only run single threaded so if multiple tests
are matched they don't print out over each other at the same time.


Adding New Unit Tests
---------------------

* An input gmad file should be prepared that does the least possible work to show the desired
  feature.
* Extraneous options and number of events should be avoided.
* The simplest physics list should be used.
* The test should be as short as possible.
* Consider only 1 event, use the default options as much as possible in BDSIM (less input syntax).
* Try to make the test naming hierarchical, e.g. :code:`field-magnetic-quadrupole`. This makes testing
  everything to do with a field easy.
* Only use overlap checking if necessary - this is time consuming and we should only check one geometry
  combination once. This is largely covered already in the test suite.
* Adding data files to the repository should be avoided unless strictly necessary as this
  permanently inflates the size of the git repository. Specific BDSIM data examples are included
  in the :code:`examples/features/data` directory that can be used by any test.
* Remember, do not edit the copy of the input or test in the build directory as this will be overwritten
  then next time the build is run. Prefer to edit the original, then update the build.

Why so pedantic about this? With (currently) around 800 tests, being wasteful in individual tests
quickly adds up.  1s extra on each tests would add up to 13 minutes of extra execution time.

To add a test, add a line in the CMakeLists.txt beside that example. The syntax is: ::

  simple_testing(testname   "--file=inputfile.gmad"  "")

The last argument is any string to match in the print out from the test that should cause it to fail,
e.g. "overlap". :code:`${OVERLAP_CHECK}` defines a pre-made set of overlap keywords. It can be left
empty.

To add a purposively failing test, i.e. a test that the program should complain and exit, is: ::

  simple_fail(testname  "--file=inputfile.gmad" "")

.. note:: Test input files must be committed to the git repository to be picked up by the build
	  system and included in the build directory. This is done as the build system uses git
	  to only copy files that are defined in the repository, therefore avoiding copying the
	  typically polluted example directory when developing the code.

Steps:

1) Define new input file
2) Add to CMakeLists.txt
3) Commit input file(s) and updated CMakeLists.txt
4) Update build (:code:`ccmake .`, then :code:`<c>`, then :code:`<g>`, then :code:`make`)
5) :code:`ctest -R mynewntestname`
6) If test doesn't pass, fix (in source directory, not the build), repeat steps 4 and 5 (3 is not necessary this time)


Extending Test Macros
---------------------

Test macros are small functions that we have defined in our CMake input to not only add a test
to the CMake build system but also set some properties (such as labels). These are defined in: ::

  bdsim/cmake/Modules/bdsim_test_macros.cmake

Please review the already existing ones and extend if necessary.

.. note:: There are different test macros for various executables.
