import 'package:flutter_midi_command_platform_interface/flutter_midi_command_platform_interface.dart';
import 'package:midi_win_plugin/midi_win_plugin_method_channel.dart';

export 'package:flutter_midi_command_platform_interface/flutter_midi_command_platform_interface.dart'
    show MidiDevice, MidiPacket, MidiPort;

class MidiWinPlugin {
  MidiWinPlugin() {
    // Sets an instance to be used by MidiCommand()
    MidiCommandPlatform.instance = MethodChannelMidiCommandWindows();
  }
}
