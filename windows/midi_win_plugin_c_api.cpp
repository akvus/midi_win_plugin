#include "include/midi_win_plugin/midi_win_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "midi_win_plugin.h"

void MidiWinPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  midi_win_plugin::MidiWinPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
