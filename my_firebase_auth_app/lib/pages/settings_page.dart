import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:provider/provider.dart';
import '../services/auth_service.dart';
import '../models/models.dart';
import '../dialogs/edit_device_dialog.dart';
import '../dialogs/edit_sensor_dialog.dart';

class SettingsPage extends StatelessWidget {
  const SettingsPage({super.key});

  @override
  Widget build(BuildContext context) {
    final authService = Provider.of<AuthService>(context, listen: false);
    User? user = authService.getCurrentUser();

    DatabaseReference devicesRef =
        FirebaseDatabase.instance.ref('users/${user?.uid}/devices');
    DatabaseReference sensorsRef =
        FirebaseDatabase.instance.ref('users/${user?.uid}/sensors');

    return Scaffold(
      appBar: AppBar(
        title: const Text('Cài đặt'),
      ),
      body: ListView(
        children: [
          const Padding(
            padding: EdgeInsets.all(16.0),
            child: Text(
              'Thiết Bị',
              style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
          ),
          StreamBuilder(
            stream: devicesRef.onValue,
            builder: (context, snapshot) {
              if (snapshot.hasData &&
                  !snapshot.hasError &&
                  snapshot.data!.snapshot.value != null) {
                Map<dynamic, dynamic> data =
                    snapshot.data!.snapshot.value as Map<dynamic, dynamic>;
                List<Device> devices = data.entries
                    .map((entry) => Device.fromJson(
                        Map<String, dynamic>.from(entry.value), entry.key))
                    .toList();
                return ListView.builder(
                  shrinkWrap: true,
                  physics: const NeverScrollableScrollPhysics(),
                  itemCount: devices.length,
                  itemBuilder: (context, index) {
                    DatabaseReference deviceRef =
                        devicesRef.child(devices[index].key);
                    return ListTile(
                      title: Text(devices[index].name),
                      subtitle: Text('Loại: ${devices[index].type}'),
                      trailing: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          IconButton(
                            icon: const Icon(Icons.edit),
                            onPressed: () {
                              showDialog(
                                context: context,
                                builder: (context) => EditDeviceDialog(
                                  deviceRef: deviceRef,
                                  device: devices[index],
                                ),
                              );
                            },
                          ),
                          IconButton(
                            icon: const Icon(Icons.delete),
                            onPressed: () {
                              deviceRef.remove();
                            },
                          ),
                        ],
                      ),
                    );
                  },
                );
              } else {
                return const Center(child: Text('No devices available'));
              }
            },
          ),
          const Padding(
            padding: EdgeInsets.all(16.0),
            child: Text(
              'Cảm Biến',
              style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
          ),
          StreamBuilder(
            stream: sensorsRef.onValue,
            builder: (context, snapshot) {
              if (snapshot.hasData &&
                  !snapshot.hasError &&
                  snapshot.data!.snapshot.value != null) {
                Map<dynamic, dynamic> data =
                    snapshot.data!.snapshot.value as Map<dynamic, dynamic>;
                List<Sensor> sensors = data.entries
                    .map((entry) => Sensor.fromJson(
                        Map<String, dynamic>.from(entry.value), entry.key))
                    .toList();
                return ListView.builder(
                  shrinkWrap: true,
                  physics: const NeverScrollableScrollPhysics(),
                  itemCount: sensors.length,
                  itemBuilder: (context, index) {
                    DatabaseReference sensorRef =
                        sensorsRef.child(sensors[index].key);
                    return ListTile(
                      title: Text(sensors[index].name),
                      trailing: Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          IconButton(
                            icon: const Icon(Icons.edit),
                            onPressed: () {
                              showDialog(
                                context: context,
                                builder: (context) => EditSensorDialog(
                                  sensorRef: sensorRef,
                                  sensor: sensors[index],
                                ),
                              );
                            },
                          ),
                          IconButton(
                            icon: const Icon(Icons.delete),
                            onPressed: () {
                              sensorRef.remove();
                            },
                          ),
                        ],
                      ),
                    );
                  },
                );
              } else {
                return const Center(child: Text('No sensors available'));
              }
            },
          ),
        ],
      ),
    );
  }
}
