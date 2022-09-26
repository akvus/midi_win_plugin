#include "midi_win_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter/encodable_value.h>

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <memory>
#include <sstream>
#include <map>
#include <chrono>
#include <thread>

#include <mmsystem.h>

#define __WINDOWS_MM__
#include "RtMidi.h"


/*
 * TODO: 
 * - connectToDevice
 * - disconnectDevice
 * - onMidiSetupChanged
 * - onMidiDataReceived
 * */

using namespace flutter;

namespace midi_win_plugin {

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
    const MethodCall<EncodableValue> &method_call,
    std::unique_ptr<MethodResult<EncodableValue>> result) {

  const auto *arguments = std::get_if<EncodableMap>(method_call.arguments());

  if (method_call.method_name().compare("getDevices") == 0) {
		EncodableList list = this->getDevices();

    result->Success(EncodableValue(list));
  } 
	else if (method_call.method_name().compare("connectToDevice") == 0) { 
		auto deviceValue = (arguments->find(EncodableValue("device")))->second;
		auto ports = (arguments->find(EncodableValue("ports")))->second;

		auto device = static_cast<EncodableMap>(std::get<EncodableMap>(deviceValue));
		auto idValue = device[EncodableValue("id")];

    auto id = std::get<std::string>(idValue);
		auto portNumber = std::stoi(id);

    this->connectToDevice(portNumber);

    result->Success(EncodableValue(true));
	}
	else if (method_call.method_name().compare("disconnectDevice") == 0) { 
    this->disconnectDevice();

    result->Success(EncodableValue(true));
	}
	else {
    result->NotImplemented();
  }
}

EncodableList MidiWinPlugin::getDevices() {
	EncodableList devices = {};
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

		devices.push_back(this->getDevice(i, portName, true));
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

		devices.push_back(this->getDevice(i, portName, false));
  }

 cleanup:
  delete midiin;
  delete midiout;

	return devices;
}

flutter::EncodableValue MidiWinPlugin::getDevice(int port, std::string portName, bool isIn) {
		EncodableList ports = EncodableList{EncodableValue(
				EncodableMap {
				 {EncodableValue("id"), EncodableValue(port)}
				} 
				)};

		EncodableList inputs = isIn ? ports : EncodableList();
		EncodableList outputs  = isIn ? EncodableList() : ports;
		
		return EncodableValue(EncodableMap {
				{EncodableValue("id"), EncodableValue(port)},
				{EncodableValue("name"), EncodableValue(portName)},
				{EncodableValue("type"), EncodableValue(isIn ? "IN" : "OUT")},
				{EncodableValue("inputs"), EncodableValue(inputs)},
				{EncodableValue("outputs"), EncodableValue(outputs)},
				{EncodableValue("connected"), EncodableValue("false")}
		});
}

bool done;
static void finish(int ignore){ done = true; }

void MidiWinPlugin::connectToDevice(int portNumber) {
	RtMidiIn *midiin = new RtMidiIn();
  std::vector<unsigned char> message;
  int i;
	size_t nBytes;
  double stamp;

  unsigned int nPorts = midiin->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    goto cleanup;
  }

  midiin->openPort( portNumber );
  midiin->ignoreTypes( false, false, false );

  // Install an interrupt handler function.
  done = false;
  (void) signal(SIGINT, finish);

  // Periodically check input queue.
  std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";

  while ( !done ) {
    stamp = midiin->getMessage( &message );
    nBytes = message.size();
    for ( i=0; i<nBytes; i++ )
      std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
    if ( nBytes > 0 )
      std::cout << "stamp = " << stamp << std::endl;

    // Sleep for 10 milliseconds ... platform-dependent.
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

 cleanup:
  delete midiin;
}

void MidiWinPlugin::disconnectDevice() {

}

}  // namespace midi_win_plugin
