import 'package:flutter/services.dart';
import 'package:flutter_midi_command_platform_interface/flutter_midi_command_platform_interface.dart';
import 'package:flutter_midi_command_platform_interface/method_channel_midi_command.dart';

// TODO for now it's copied form MEthodChannelMidiCommand
const MethodChannel _methodChannel =
    MethodChannel('plugins.invisiblewrench.com/flutter_midi_command');

class MethodChannelMidiCommandWindows extends MethodChannelMidiCommand {
  @override
  Future<List<MidiDevice>?> get devices async {
    final devices =
        await _methodChannel.invokeMethod<List<Object?>>('getDevices');

    return devices!.map((e) {
      final deviceData = (e as String).split(';');

      return MidiDevice(deviceData[1], deviceData[2], deviceData[0], false);
    }).toList();
  }
}
