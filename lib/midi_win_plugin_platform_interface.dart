import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'midi_win_plugin_method_channel.dart';

abstract class MidiWinPluginPlatform extends PlatformInterface {
  /// Constructs a MidiWinPluginPlatform.
  MidiWinPluginPlatform() : super(token: _token);

  static final Object _token = Object();

  static MidiWinPluginPlatform _instance = MethodChannelMidiWinPlugin();

  /// The default instance of [MidiWinPluginPlatform] to use.
  ///
  /// Defaults to [MethodChannelMidiWinPlugin].
  static MidiWinPluginPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [MidiWinPluginPlatform] when
  /// they register themselves.
  static set instance(MidiWinPluginPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
