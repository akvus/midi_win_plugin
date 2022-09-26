import 'package:flutter_midi_command_platform_interface/flutter_midi_command_platform_interface.dart';
import 'package:flutter_midi_command_platform_interface/method_channel_midi_command.dart';

void initWindows() {
  MidiCommandPlatform.instance = MethodChannelMidiCommand();
}
