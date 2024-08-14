import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:provider/provider.dart';
import 'package:percent_indicator/percent_indicator.dart';
import '../services/auth_service.dart';
import '../dialogs/add_device_dialog.dart';
import '../dialogs/add_sensor_dialog.dart';
import 'settings_page.dart';
import '../models/models.dart';

class HomePage extends StatelessWidget {
  const HomePage({super.key});

  @override
  Widget build(BuildContext context) {
    final authService = Provider.of<AuthService>(context);

    return Scaffold(
      appBar: AppBar(
        title: const Text('Nhà của tôi  '),
        actions: [
          IconButton(
            icon: const Icon(Icons.settings),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => const SettingsPage()),
              );
            },
          ),
          IconButton(
            icon: const Icon(Icons.logout),
            onPressed: () async {
              _showLogoutConfirmationDialog(context, authService);
            },
          ),
        ],
      ),
      body: Container(
        decoration: const BoxDecoration(
          image: DecorationImage(
            image: AssetImage(
                'assets/background.jpg'), // Path to your background image
            fit: BoxFit.cover,
          ),
        ),
        child: Column(
          children: [
            Padding(
              padding: const EdgeInsets.all(10.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.end,
                children: [
                  FloatingActionButton(
                    heroTag: 'add_device',
                    onPressed: () {
                      showDialog(
                        context: context,
                        builder: (context) => const AddDeviceDialog(),
                      );
                    },
                    tooltip: 'Add Device',
                    child: const Icon(Icons.add),
                  ),
                  const SizedBox(width: 5),
                  // FloatingActionButton(
                  //   heroTag: 'add_sensor',
                  //   onPressed: () {
                  //     showDialog(
                  //       context: context,
                  //       builder: (context) => const AddSensorDialog(),
                  //     );
                  //   },
                  //   tooltip: 'Add Sensor',
                  //   child: const Icon(Icons.add_chart),
                  // ),
                ],
              ),
            ),
            const SensorInfo(),
            const Expanded(
              child: DeviceList(),
            ),
          ],
        ),
      ),
    );
  }

  void _showLogoutConfirmationDialog(
      BuildContext context, AuthService authService) {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Text('Xác nhận đăng xuất'),
          content: const Text('Bạn có chắc chắn muốn đăng xuất?'),
          actions: <Widget>[
            TextButton(
              child: const Text('Hủy'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
            TextButton(
              child: const Text('Đăng xuất'),
              onPressed: () async {
                await authService.signOut();
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }
}

class SensorInfo extends StatelessWidget {
  const SensorInfo({super.key});

  @override
  Widget build(BuildContext context) {
    final authService = Provider.of<AuthService>(context, listen: false);
    User? user = authService.getCurrentUser();
    DatabaseReference sensorsRef =
        FirebaseDatabase.instance.ref('users/${user?.uid}/sensors');

    return StreamBuilder(
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
          return Column(
            children:
                sensors.map((sensor) => SensorDisplay(sensor: sensor)).toList(),
          );
        } else {
          return Center(
            child: ElevatedButton(
              child: const Text('Thêm Cảm Biến'),
              onPressed: () => showDialog(
                context: context,
                builder: (context) => const AddSensorDialog(),
              ),
            ),
          );
        }
      },
    );
  }
}

class SensorDisplay extends StatelessWidget {
  final Sensor sensor;

  const SensorDisplay({required this.sensor, super.key});

  @override
  Widget build(BuildContext context) {
    final double humidity = sensor.humidity;
    final double temperature = sensor.temperature;

    return SizedBox(
      height: MediaQuery.of(context).size.height * 0.4,
      child: Center(
        child: CircularPercentIndicator(
          radius: 130.0,
          lineWidth: 25.0,
          percent: humidity / 100,
          center: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                '$temperature°C',
                style: Theme.of(context).textTheme.headlineMedium,
              ),
              const SizedBox(height: 10),
              Text(
                'Độ ẩm: $humidity%',
                style: Theme.of(context).textTheme.titleMedium,
              ),
            ],
          ),
          progressColor: Colors.blue,
          backgroundColor: Colors.grey[200]!,
          circularStrokeCap: CircularStrokeCap.round,
        ),
      ),
    );
  }
}

class DeviceList extends StatelessWidget {
  const DeviceList({super.key});

  @override
  Widget build(BuildContext context) {
    final authService = Provider.of<AuthService>(context, listen: false);
    User? user = authService.getCurrentUser();
    DatabaseReference devicesRef =
        FirebaseDatabase.instance.ref('users/${user?.uid}/devices');

    return StreamBuilder(
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
          return GridView.builder(
            gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
              crossAxisCount: 2,
              childAspectRatio: 3 / 2,
              crossAxisSpacing: 10,
              mainAxisSpacing: 10,
            ),
            padding: const EdgeInsets.all(10),
            itemCount: devices.length,
            itemBuilder: (context, index) {
              return DeviceTile(device: devices[index]);
            },
          );
        } else {
          return Center(
            child: ElevatedButton(
              child: const Text('Thêm Thiết Bị'),
              onPressed: () => showDialog(
                context: context,
                builder: (context) => const AddDeviceDialog(),
              ),
            ),
          );
        }
      },
    );
  }
}

class DeviceTile extends StatelessWidget {
  final Device device;

  const DeviceTile({required this.device, super.key});

  @override
  Widget build(BuildContext context) {
    final authService = Provider.of<AuthService>(context, listen: false);
    User? user = authService.getCurrentUser();
    DatabaseReference deviceRef = FirebaseDatabase.instance
        .ref('users/${user?.uid}/devices/${device.key}');

    return GestureDetector(
      onTap: () {
        bool newStatus = !device.status;
        deviceRef.update({'status': newStatus});
      },
      child: Container(
        decoration: BoxDecoration(
          color: device.status ? Colors.orangeAccent : Colors.grey,
          borderRadius: BorderRadius.circular(10),
        ),
        child: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                device.name,
                style: const TextStyle(
                  color: Colors.white,
                  fontSize: 18,
                  fontWeight: FontWeight.bold,
                ),
              ),
              const SizedBox(height: 10),
              Text(
                'Loại: ${device.type}',
                style: const TextStyle(
                  color: Colors.white,
                  fontSize: 14,
                ),
              ),
              const SizedBox(height: 5),
              Text(
                'Trạng thái: ${device.status ? 'Bật' : 'Tắt'}',
                style: const TextStyle(
                  color: Colors.white,
                  fontSize: 14,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
