import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';
import 'package:flutter_midi_command_platform_interface/flutter_midi_command_platform_interface.dart';
import 'package:flutter_midi_command_platform_interface/method_channel_midi_command.dart';

class MethodChannelMidiWinPlugin extends MethodChannelMidiCommand {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('midi_win_plugin');

  @override
  Future<List<MidiDevice>?> get devices async {
    final devices =
        await methodChannel.invokeMethod<List<Object?>>('getDevices');

    return devices!.map((e) {
      final deviceData = (e as String).split(';');

      return MidiDevice(deviceData[1], deviceData[2], deviceData[0], false);
    }).toList();
  }
}
