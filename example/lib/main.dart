import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_midi_command/flutter_midi_command.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final _midiCommand = MidiCommand();

  List<MidiDevice> _devices = [];

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  Future<void> initPlatformState() async {
    List<MidiDevice> devices;
    try {
      devices = await _midiCommand.devices ?? [];
    } on PlatformException {
      devices = [];
    }

    if (!mounted) return;

    setState(() {
      _devices = devices;
    });

    //Don't know how to register the event channel in midiCommand
    //_midiCommand.onMidiDataReceived?.listen((event) { print("event = $event"); });

    const eventChannel = EventChannel('plugins.invisiblewrench.com/flutter_midi_command/rx_channel');
    eventChannel.receiveBroadcastStream().listen((dynamic event) {
      print("event = $event");
    });


  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Column(
          children: [
            Text(_devices.length.toString()),
            SizedBox(
              height: 300,
              child: ListView.separated(
                separatorBuilder: (context, index) => SizedBox(height: 20),
                itemBuilder: (context, index) {
                  final item = _devices[index];

                  return Text(
                      "${item.name} || ${item.id} ${item.type} ${item.connected} ${item.inputPorts} ${item.outputPorts}");
                },
                itemCount: _devices.length,
              ),
            ),
            Row(
              children: [
                ElevatedButton(
                    onPressed: () {
                      _midiCommand.connectToDevice(_devices
                          .where((element) => element.type == "IN")
                          .first);


                    },
                    child: Text("Connect")),
                ElevatedButton(
                    onPressed: () {
                      _midiCommand.disconnectDevice(_devices[0]);
                    },
                    child: Text("Disconnect")),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
