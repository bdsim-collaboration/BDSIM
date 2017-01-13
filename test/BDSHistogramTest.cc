#include "BDSBin.hh"
#include "BDSHistogram.hh"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

typedef std::chrono::high_resolution_clock Clock;

int main(int /*argc*/, char** /*argv*/)
{
  std::cout << "Test evenly spaced histogram" << std::endl;
  BDSHistogram1D hist = BDSHistogram1D(0,5.23,10,"histo1", "Test Histogram 1",
				       "S Position", "# of Hits");
  std::cout << "Initial histogram" << std::endl;
  std::cout << hist << std::endl;
  hist.PrintBins();

  /// Test a value, same value twice, another value, overflow, underflow and last bin
  std::vector<double> values = {0.23,0.23,4.892,20,-18, 2.4534};

  for (auto value : values)
    {
      std::cout << "Filling with value: " << value << std::endl;
      hist.Fill(value);
      hist.PrintBins();
      std::cout << std::endl;
    }
  
  std::cout << "Test unevenly spaced histogram" << std::endl;

  std::vector<double> edges = {0,2.0,2.2,2.30001,2.4,5.0};
  BDSHistogram1D hist2 = BDSHistogram1D(edges,"histo2", "Test Histogram 2",
				       "S Position", "# of Hits");
  std::cout << "Initial histogram (2)" << std::endl;
  std::cout << hist2 << std::endl;
  hist2.PrintBins();

  for (auto value : values)
    {
      std::cout << "Filling with value: " << value << std::endl;
      hist2.Fill(value);
      hist2.PrintBins();
      std::cout << std::endl;
    }

  std::cout << "Test random fill on unevenly spaced histogram" << std::endl;  
  
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0,5.0);

  /// generate bin edges randomly within an interval.
  std::vector<double> newBinEdges;
  for (int i = 0; i < 100000; i++)
    {newBinEdges.push_back(distribution(generator));}
  std::sort(newBinEdges.begin(), newBinEdges.end());
  BDSHistogram1D hist3 = BDSHistogram1D(newBinEdges,"histo3", "Test Histogram 3",
				       "S Position", "# of Hits");
  
  int numberOfEntries = 1000000;
  auto t1 = Clock::now();
  for (int i = 0; i < numberOfEntries; i++)
    {hist3.Fill(distribution(generator));}
  auto t2 = Clock::now();

  std::cout << "Time / fill: " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / (double)numberOfEntries << " us" << std::endl;

  std::cout << hist3 << std::endl;
  //hist3.PrintBins();

  return 0;
}
