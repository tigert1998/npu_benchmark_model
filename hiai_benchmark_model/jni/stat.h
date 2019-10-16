#ifndef STAT_H_
#define STAT_H_

#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

template <typename ValueType, typename HighPrecisionValueType = double>
class Stat {
 public:
  void UpdateStat(ValueType v) {
    if (count_ == 0) {
      first_ = v;
    }

    newest_ = v;
    max_ = std::max(v, max_);
    min_ = std::min(v, min_);
    ++count_;
    sum_ += v;
    squared_sum_ += static_cast<HighPrecisionValueType>(v) * v;
  }

  void Reset() { new (this) Stat<ValueType, HighPrecisionValueType>(); }

  bool empty() const { return count_ == 0; }

  ValueType first() const { return first_; }

  ValueType newest() const { return newest_; }

  ValueType max() const { return max_; }

  ValueType min() const { return min_; }

  int64_t count() const { return count_; }

  ValueType sum() const { return sum_; }

  HighPrecisionValueType squared_sum() const { return squared_sum_; }

  bool all_same() const { return (count_ == 0 || min_ == max_); }

  HighPrecisionValueType avg() const {
    return empty() ? std::numeric_limits<ValueType>::quiet_NaN()
                   : static_cast<HighPrecisionValueType>(sum_) / count_;
  }

  ValueType std_deviation() const {
    return all_same() ? 0 : sqrt(squared_sum_ / count_ - avg() * avg());
  }

  void OutputToStream(std::ostream* stream) const {
    if (empty()) {
      *stream << "count=0";
    } else if (all_same()) {
      *stream << "count=" << count_ << " curr=" << newest_;
      if (count_ > 1) *stream << "(all same)";
    } else {
      *stream << "count=" << count_ << " first=" << first_
              << " curr=" << newest_ << " min=" << min_ << " max=" << max_
              << " avg=" << avg() << " std=" << std_deviation();
    }
  }

  friend std::ostream& operator<<(std::ostream& stream,
                                  const Stat<ValueType>& stat) {
    stat.OutputToStream(&stream);
    return stream;
  }

 private:
  ValueType first_ = 0;
  ValueType newest_ = 0;
  ValueType max_ = std::numeric_limits<ValueType>::min();
  ValueType min_ = std::numeric_limits<ValueType>::max();
  int64_t count_ = 0;
  ValueType sum_ = 0;
  HighPrecisionValueType squared_sum_ = 0;
};

#endif  // STAT_H_
