#ifndef FLUTTER_PLUGIN_MIDI_WIN_PLUGIN_H_
#define FLUTTER_PLUGIN_MIDI_WIN_PLUGIN_H_


#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/encodable_value.h>
#include <flutter/event_channel.h>
#include <flutter/event_sink.h>
#include <flutter/event_stream_handler.h>

#include <memory>
#include <map>

namespace midi_win_plugin {

typedef flutter::EventChannel<flutter::EncodableValue> FlEventChannel;
typedef flutter::EventSink<flutter::EncodableValue> FlEventSink;
typedef flutter::MethodCall<flutter::EncodableValue> FlMethodCall;
typedef flutter::MethodResult<flutter::EncodableValue> FlMethodResult;
typedef flutter::MethodChannel<flutter::EncodableValue> FlMethodChannel;
typedef flutter::StreamHandler<flutter::EncodableValue> FlStreamHandler;
typedef flutter::StreamHandlerError<flutter::EncodableValue>
    FlStreamHandlerError;

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
	flutter::EncodableValue getDevice(int port, std::string portName, bool isIn);

	void connectToDevice(int portNumber);
	void disconnectDevice();
};

class MidiMessagesStreamHandler : public FlStreamHandler {
public:
  MidiMessagesStreamHandler();
  virtual ~MidiMessagesStreamHandler();
  void AddMidiMessageEvent(double deltatime, std::vector< unsigned char > *message, size_t nBytes);

protected:
  std::unique_ptr<FlStreamHandlerError>
  OnListenInternal(const flutter::EncodableValue *arguments,
                   std::unique_ptr<FlEventSink> &&sink) override;

  std::unique_ptr<FlStreamHandlerError>
  OnCancelInternal(const flutter::EncodableValue *arguments) override;

private:
  std::unique_ptr<FlEventSink> sink;
};

}  // namespace midi_win_plugin

#endif  // FLUTTER_PLUGIN_MIDI_WIN_PLUGIN_H_
