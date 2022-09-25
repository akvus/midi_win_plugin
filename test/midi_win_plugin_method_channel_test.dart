import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:midi_win_plugin/midi_win_plugin_method_channel.dart';

void main() {
  MethodChannelMidiWinPlugin platform = MethodChannelMidiWinPlugin();
  const MethodChannel channel = MethodChannel('midi_win_plugin');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
