
import 'midi_win_plugin_platform_interface.dart';

class MidiWinPlugin {
  Future<String?> getPlatformVersion() {
    return MidiWinPluginPlatform.instance.getPlatformVersion();
  }
}
