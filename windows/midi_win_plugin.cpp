#include "midi_win_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter/encodable_value.h>

#include <memory>
#include <sstream>
#include <map>

#include <mmsystem.h>

#define __WINDOWS_MM__
#include "RtMidi.h"

using namespace flutter;

namespace midi_win_plugin {

// static
void MidiWinPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "plugins.invisiblewrench.com/flutter_midi_command",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<MidiWinPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

MidiWinPlugin::MidiWinPlugin() {}

MidiWinPlugin::~MidiWinPlugin() {}

void MidiWinPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getDevices") == 0) {

		flutter::EncodableList list = this->getDevices();

    result->Success(flutter::EncodableValue(list));
  } else {
    result->NotImplemented();
  }
}

flutter::EncodableList MidiWinPlugin::getDevices() {
	flutter::EncodableList devices = {};
	RtMidiIn *midiin = 0;
  RtMidiOut *midiout = 0;

  try {
    midiin = new RtMidiIn();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }

  unsigned int nPorts = midiin->getPortCount();
  std::string portName;
  for ( unsigned int i=0; i<nPorts; i++ ) {
    try {
      portName = midiin->getPortName(i);
    }
    catch ( RtMidiError &error ) {
      error.printMessage();
      goto cleanup;
    }

		int port = (int) i;

		flutter::EncodableList inputs = flutter::EncodableList{flutter::EncodableValue(
				EncodableMap {
				 {EncodableValue("id"), EncodableValue(port)}
				} 
				)};

		devices.push_back(flutter::EncodableValue(flutter::EncodableMap {
				{flutter::EncodableValue("id"), flutter::EncodableValue(port)},
				{flutter::EncodableValue("name"), flutter::EncodableValue(portName)},
				{flutter::EncodableValue("type"), flutter::EncodableValue("IN")},
				{flutter::EncodableValue("inputs"), flutter::EncodableValue(inputs)},
				{flutter::EncodableValue("outputs"), flutter::EncodableValue(flutter::EncodableList())},
				{flutter::EncodableValue("connected"), flutter::EncodableValue("false")}
		}));
  }

  try {
    midiout = new RtMidiOut();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }

  nPorts = midiout->getPortCount();
  for ( unsigned int i=0; i<nPorts; i++ ) {
    try {
      portName = midiout->getPortName(i);
    }
    catch (RtMidiError &error) {
      error.printMessage();
      goto cleanup;
    }


		// TODO DRY
		int port = (int) i;

		flutter::EncodableList outputs = flutter::EncodableList{flutter::EncodableValue(
				EncodableMap {
				 {EncodableValue("id"), EncodableValue(port)}
				} 
				)};

		devices.push_back(flutter::EncodableValue(flutter::EncodableMap {
				{flutter::EncodableValue("id"), flutter::EncodableValue(port)},
				{flutter::EncodableValue("name"), flutter::EncodableValue(portName)},
				{flutter::EncodableValue("type"), flutter::EncodableValue("OUT")},
				{flutter::EncodableValue("inputs"), flutter::EncodableValue(flutter::EncodableList())},
				{flutter::EncodableValue("outputs"), flutter::EncodableValue(outputs)},
				{flutter::EncodableValue("connected"), flutter::EncodableValue("false")}
		}));
  }

 cleanup:
  delete midiin;
  delete midiout;

	return devices;
}

}  // namespace midi_win_plugin
