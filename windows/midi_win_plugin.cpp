#include "midi_win_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include <mmsystem.h>

#define __WINDOWS_MM__
#include "RtMidi.h"

namespace midi_win_plugin {

// static
void MidiWinPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "midi_win_plugin",
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
		std::vector<flutter::EncodableValue> list = this->getDevices();
    result->Success(list);
  } else {
    result->NotImplemented();
  }
}

std::vector<flutter::EncodableValue> MidiWinPlugin::getDevices() {
	//UINT nMidiDeviceNum;

	//nMidiDeviceNum = midiInGetNumDevs();
	//std::cout << nMidiDeviceNum << " XX\n";
	std::vector<flutter::EncodableValue> deviceNames {};

	RtMidiIn *midiin = 0;
  RtMidiOut *midiout = 0;
  try {
    midiin = new RtMidiIn();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Check inputs.
  unsigned int nPorts = midiin->getPortCount();
  std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
  std::string portName;
  for ( unsigned int i=0; i<nPorts; i++ ) {
    try {
      portName = midiin->getPortName(i);
    }
    catch ( RtMidiError &error ) {
      error.printMessage();
      goto cleanup;
    }
		std::string deviceData = "IN;" + std::to_string(i) + ";" + portName;
		deviceNames.push_back(flutter::EncodableValue(deviceData));
    std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
  }
  // RtMidiOut constructor
  try {
    midiout = new RtMidiOut();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }
  // Check outputs.
  nPorts = midiout->getPortCount();
  std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
  for ( unsigned int i=0; i<nPorts; i++ ) {
    try {
      portName = midiout->getPortName(i);
    }
    catch (RtMidiError &error) {
      error.printMessage();
      goto cleanup;
    }
		std::string deviceData = "OUT;" + std::to_string(i) + ";" + portName;
		deviceNames.push_back(flutter::EncodableValue(deviceData));
    std::cout << "  Output Port #" << i+1 << ": " << portName << '\n';
  }
  std::cout << '\n';

  // Clean up
 cleanup:
  delete midiin;
  delete midiout;

	return deviceNames;
}

}  // namespace midi_win_plugin
