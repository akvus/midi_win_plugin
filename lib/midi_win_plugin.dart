import 'package:flutter_midi_command_platform_interface/flutter_midi_command_platform_interface.dart';

import 'midi_win_plugin_platform_interface.dart';

export 'package:flutter_midi_command_platform_interface/flutter_midi_command_platform_interface.dart';

class MidiWinPlugin {
  Future<List<MidiDevice>?> getDevices() {
    return MidiWinPluginPlatform.instance.getDevices();
  }
}
