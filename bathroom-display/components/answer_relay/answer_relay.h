#pragma once

#include <string>
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/media_player/media_player.h"

// A media player that plays nothing itself: a URL handed to it fires on_play_url, and it reports
// announcing until finish() is called, so voice_assistant can hand its answer URL to another speaker.
namespace esphome {
namespace answer_relay {

class AnswerRelay : public media_player::MediaPlayer, public Component {
 public:
  media_player::MediaPlayerTraits get_traits() override { return media_player::MediaPlayerTraits(); }

  void add_on_play_callback(std::function<void(std::string)> &&callback) {
    this->play_callback_.add(std::move(callback));
  }

  void finish() {
    if (this->state != media_player::MEDIA_PLAYER_STATE_IDLE) {
      this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
      this->publish_state();
    }
  }

 protected:
  void control(const media_player::MediaPlayerCall &call) override {
    if (call.get_media_url().has_value()) {
      this->state = media_player::MEDIA_PLAYER_STATE_ANNOUNCING;
      this->publish_state();
      this->play_callback_.call(call.get_media_url().value());
      return;
    }
    if (call.get_command().has_value() && call.get_command().value() == media_player::MEDIA_PLAYER_COMMAND_STOP)
      this->finish();
  }

  CallbackManager<void(std::string)> play_callback_;
};

class PlayTrigger : public Trigger<std::string> {
 public:
  explicit PlayTrigger(AnswerRelay *parent) {
    parent->add_on_play_callback([this](std::string url) { this->trigger(url); });
  }
};

}  // namespace answer_relay
}  // namespace esphome
