import 'package:flutter_test/flutter_test.dart';
import 'package:midi_win_plugin/midi_win_plugin.dart';
import 'package:midi_win_plugin/midi_win_plugin_platform_interface.dart';
import 'package:midi_win_plugin/midi_win_plugin_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockMidiWinPluginPlatform
    with MockPlatformInterfaceMixin
    implements MidiWinPluginPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final MidiWinPluginPlatform initialPlatform = MidiWinPluginPlatform.instance;

  test('$MethodChannelMidiWinPlugin is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelMidiWinPlugin>());
  });

  test('getPlatformVersion', () async {
    MidiWinPlugin midiWinPlugin = MidiWinPlugin();
    MockMidiWinPluginPlatform fakePlatform = MockMidiWinPluginPlatform();
    MidiWinPluginPlatform.instance = fakePlatform;

    expect(await midiWinPlugin.getPlatformVersion(), '42');
  });
}
