import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'midi_win_plugin_platform_interface.dart';

/// An implementation of [MidiWinPluginPlatform] that uses method channels.
class MethodChannelMidiWinPlugin extends MidiWinPluginPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('midi_win_plugin');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
