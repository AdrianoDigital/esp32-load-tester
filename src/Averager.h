#pragma once

template <class T>
class Averager {
  // Collects a configurable number of samples, and calculates average when
  // complete.
  // Usage:
  //      Averager<long> avg(3);
  //      while(!avg.is_complete()) {
  //        avg.add( get_sample() );
  //      }
  //      long a = avg.average();
 private:
  unsigned int factor;
  unsigned int counter;
  T sum;
  T avg;

 public:
  Averager(unsigned int factor = 20)
      : factor(factor), counter(0), sum(0), avg(0) {};

  bool is_complete() { return counter == factor; };
  void add(T sample) {
    if (!is_complete()) {
      sum += sample;
      counter++;
      if (is_complete()) {
        avg = sum / (T)(factor);
      }
    }
  };
  T average() { return avg; };
  void clear() {
    sum = 0;
    counter = 0;
    avg = 0;
  }
};
