#include "midi_win_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter/encodable_value.h>
#include <flutter/event_channel.h>
#include <flutter/event_sink.h>
#include <flutter/event_stream_handler.h>

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
 * - onMidiSetupChanged
 * - onMidiDataReceived
 * - clean all that mess up !!!!
 * */

using namespace flutter;

namespace midi_win_plugin {

typedef flutter::EventChannel<flutter::EncodableValue> FlEventChannel;
typedef flutter::EventSink<flutter::EncodableValue> FlEventSink;
typedef flutter::MethodCall<flutter::EncodableValue> FlMethodCall;
typedef flutter::MethodResult<flutter::EncodableValue> FlMethodResult;
typedef flutter::MethodChannel<flutter::EncodableValue> FlMethodChannel;
typedef flutter::StreamHandler<flutter::EncodableValue> FlStreamHandler;
typedef flutter::StreamHandlerError<flutter::EncodableValue> FlStreamHandlerError;

// handler for midi message events
auto msgsStreamHandler = std::make_unique<MidiMessagesStreamHandler>();

// Set up and register plugin
void MidiWinPlugin::RegisterWithRegistrar(PluginRegistrarWindows *registrar) {
  auto plugin = std::make_unique<MidiWinPlugin>();

	// Set up method channel
  auto channel =
      std::make_unique<MethodChannel<EncodableValue>>(
          registrar->messenger(), "plugins.invisiblewrench.com/flutter_midi_command",
          &flutter::StandardMethodCodec::GetInstance());
  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

	// set up event channel
	auto eventChannel = std::make_unique<FlEventChannel>(
      registrar->messenger(), "plugins.invisiblewrench.com/flutter_midi_command/rx_channel",
      &flutter::StandardMethodCodec::GetInstance());
  eventChannel->SetStreamHandler(std::move(msgsStreamHandler));

	// register plugin
	// TODO not sure that move doesn't brake sth here? but it made this compile xD
  registrar->AddPlugin(std::move(plugin));
}

MidiWinPlugin::MidiWinPlugin() {}

MidiWinPlugin::~MidiWinPlugin() {}

void MidiWinPlugin::HandleMethodCall(
    const MethodCall<EncodableValue> &method_call,
    std::unique_ptr<MethodResult<EncodableValue>> result) {

  const auto *arguments = std::get_if<EncodableMap>(method_call.arguments());
	std::cout << "Thread dart: " << std::this_thread::get_id() << std::endl;

  if (method_call.method_name().compare("getDevices") == 0) {
		EncodableList list = this->getDevices();

    result->Success(EncodableValue(list));
  } 
	else if (method_call.method_name().compare("connectToDevice") == 0) { 
    // TODO make some function to get the portNumber
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

////////////// GET DEVICES

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

EncodableValue MidiWinPlugin::getDevice(int port, std::string portName, bool isIn) {
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

////////////// CONNECT TO DEVICE
// Used when a device is connected
RtMidiIn *midiin;

void mycallback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
  size_t nBytes = message->size();

  msgsStreamHandler->AddMidiMessageEvent(deltatime, message, nBytes);
}

void MidiWinPlugin::connectToDevice(int portNumber) {
  midiin = new RtMidiIn();
  midiin->openPort( portNumber );
  midiin->setCallback( &mycallback );
  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );
  std::cout << "\nConnected to MIDI device. Reading MIDI input.\n";
}


///////////////////// DISCONNECT DEVICE

void MidiWinPlugin::disconnectDevice() {
  delete midiin;

  std::cout << "\nDisconnected MIDI device.\n";
}


////////////////// MidiMessagesStreamHandler

MidiMessagesStreamHandler::MidiMessagesStreamHandler() {}

MidiMessagesStreamHandler::~MidiMessagesStreamHandler() {}

void MidiMessagesStreamHandler::AddMidiMessageEvent(double deltatime, std::vector< unsigned char > *message, size_t nBytes) {

  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
  if ( nBytes > 0 )
    std::cout << "stamp = " << deltatime << std::endl;

	std::cout << "Thread callback: " << std::this_thread::get_id() << std::endl;

	// TODO thread issues?
  try
  {
    sink->Success(EncodableValue(3));
  }
  catch (...)
  {
		std::exception_ptr p = std::current_exception();
  }
}

std::unique_ptr<FlStreamHandlerError>
MidiMessagesStreamHandler::OnListenInternal(
    const EncodableValue *arguments,
    std::unique_ptr<FlEventSink> &&events) {

  sink = std::move(events);

  return nullptr;
}

std::unique_ptr<FlStreamHandlerError>
MidiMessagesStreamHandler::OnCancelInternal(
    const EncodableValue *arguments) {
  sink.reset();
  return nullptr;
}





}  // namespace midi_win_plugin
