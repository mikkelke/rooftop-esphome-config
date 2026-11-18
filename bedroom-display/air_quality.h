#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>

namespace bedroom_display {

enum class AirLevel { Unknown, Good, Moderate, Poor };
enum class AirMetric { Co2, Particles, Voc, Humidity };

struct AirQualityReading {
  AirLevel level = AirLevel::Unknown;
  AirMetric metric = AirMetric::Co2;
  float value = 0;
  const char *co2_trend = "";

  const char *level_name() const {
    switch (this->level) {
      case AirLevel::Good: return "Good";
      case AirLevel::Moderate: return "Moderate";
      case AirLevel::Poor: return "Poor";
      default: return "";
    }
  }

  void format_chip(char *text, std::size_t size) const {
    switch (this->metric) {
      case AirMetric::Co2:
        std::snprintf(text, size, "CO2 %.0f%s · open a window", this->value, this->co2_trend);
        break;
      case AirMetric::Particles:
        std::snprintf(text, size, "Particles %.0f · air out", this->value);
        break;
      case AirMetric::Voc:
        std::snprintf(text, size, "VOC %.0f · air out", this->value);
        break;
      case AirMetric::Humidity:
        std::snprintf(text, size, "Dry air %.0f %% · humidifier", this->value);
        break;
    }
  }
};

class AirQuality {
 public:
  AirQualityReading reading;

  void sample(float co2, float particles, float voc, float humidity) {
    this->samples_[this->sample_index_++] = {co2, particles, voc, humidity};
    this->sample_index_ %= this->samples_.size();
    this->sample_count_ = std::min(this->sample_count_ + 1, this->samples_.size());
    if (this->sample_count_ < this->samples_.size())
      return;

    const float co2_median = this->median_(AirMetric::Co2);
    const float particles_median = this->median_(AirMetric::Particles);
    const float voc_median = this->median_(AirMetric::Voc);
    const float humidity_median = this->median_(AirMetric::Humidity);

    this->reading = {};
    this->consider_(AirMetric::Co2, co2_median, level_(co2_median, 1000, 1400));
    this->consider_(AirMetric::Particles, particles_median, level_(particles_median, 15, 35));
    this->consider_(AirMetric::Voc, voc_median, level_(voc_median, 150, 250));
    this->consider_(AirMetric::Humidity, humidity_median,
                    humidity_median < 30 ? AirLevel::Moderate : AirLevel::Good);
    this->reading.co2_trend = this->trend_(co2_median);
  }

 private:
  static constexpr std::size_t MEDIAN_SAMPLES = 60;
  static constexpr std::size_t TREND_SAMPLE_INTERVAL = 15;
  static constexpr std::size_t TREND_SAMPLES = 15 * 60 / TREND_SAMPLE_INTERVAL;
  static constexpr float TREND_DEADBAND_PPM = 50;

  std::array<std::array<float, 4>, MEDIAN_SAMPLES> samples_{};
  std::size_t sample_index_ = 0;
  std::size_t sample_count_ = 0;
  std::array<float, TREND_SAMPLES> co2_history_{};
  std::size_t history_index_ = 0;
  std::size_t history_count_ = 0;
  std::size_t history_elapsed_ = 0;
  float co2_baseline_ = NAN;

  float median_(AirMetric metric) const {
    std::array<float, MEDIAN_SAMPLES> values;
    auto last = values.begin();
    for (const auto &sample : this->samples_) {
      const float value = sample[static_cast<std::size_t>(metric)];
      if (std::isfinite(value))
        *last++ = value;
    }
    const auto count = static_cast<std::size_t>(last - values.begin());
    if (count <= MEDIAN_SAMPLES / 2)
      return NAN;
    const auto middle = values.begin() + count / 2;
    std::nth_element(values.begin(), middle, last);
    if (count % 2 != 0)
      return *middle;
    return (*std::max_element(values.begin(), middle) + *middle) / 2;
  }

  static AirLevel level_(float value, float moderate, float poor) {
    if (value >= poor)
      return AirLevel::Poor;
    if (value >= moderate)
      return AirLevel::Moderate;
    return AirLevel::Good;
  }

  void consider_(AirMetric metric, float value, AirLevel level) {
    if (std::isfinite(value) && level > this->reading.level)
      this->reading = {level, metric, value};
  }

  const char *trend_(float co2) {
    if (this->history_elapsed_ == 0) {
      if (this->history_count_ == this->co2_history_.size()) {
        this->co2_baseline_ = this->co2_history_[this->history_index_];
      } else {
        ++this->history_count_;
      }
      this->co2_history_[this->history_index_++] = co2;
      this->history_index_ %= this->co2_history_.size();
    }
    this->history_elapsed_ = (this->history_elapsed_ + 1) % TREND_SAMPLE_INTERVAL;
    const float change = co2 - this->co2_baseline_;
    if (change >= TREND_DEADBAND_PPM)
      return " ↑";
    if (change <= -TREND_DEADBAND_PPM)
      return " ↓";
    return "";
  }
};

inline AirQuality &air_quality() {
  static AirQuality air;
  return air;
}

}  // namespace bedroom_display
