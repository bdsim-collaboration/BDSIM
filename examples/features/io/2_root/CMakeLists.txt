if (USE_ROOT)
  set(TESTING_ARGS --output=root --circular)
  simple_testing(IO_ROOT "--file=fodoRing.gmad" "")
endif()
