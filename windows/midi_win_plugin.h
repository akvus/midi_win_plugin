#ifndef FLUTTER_PLUGIN_MIDI_WIN_PLUGIN_H_
#define FLUTTER_PLUGIN_MIDI_WIN_PLUGIN_H_


#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/encodable_value.h>

#include <memory>
#include <map>

namespace midi_win_plugin {

class MidiWinPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  MidiWinPlugin();

  virtual ~MidiWinPlugin();

  // Disallow copy and assign.
  MidiWinPlugin(const MidiWinPlugin&) = delete;
  MidiWinPlugin& operator=(const MidiWinPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

	std::vector<flutter::EncodableValue> getDevices();
};

}  // namespace midi_win_plugin

#endif  // FLUTTER_PLUGIN_MIDI_WIN_PLUGIN_H_
