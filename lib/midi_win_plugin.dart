import 'midi_win_plugin_platform_interface.dart';

class MidiWinPlugin {
  Future<List<String>?> getDevices() {
    return MidiWinPluginPlatform.instance.getDevices();
  }
}
