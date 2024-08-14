import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:provider/provider.dart';
import '../services/auth_service.dart';

class AddSensorDialog extends StatefulWidget {
  const AddSensorDialog({super.key});

  @override
  _AddSensorDialogState createState() => _AddSensorDialogState();
}

class _AddSensorDialogState extends State<AddSensorDialog> {
  final _formKey = GlobalKey<FormState>();
  String _sensorName = '';

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: const Text('Thêm Cảm Biến'),
      content: Form(
        key: _formKey,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            TextFormField(
              decoration: const InputDecoration(labelText: 'Tên Cảm Biến'),
              validator: (value) {
                if (value == null || value.isEmpty) {
                  return 'Vui lòng nhập tên cảm biến';
                }
                return null;
              },
              onSaved: (value) {
                _sensorName = value!;
              },
            ),
          ],
        ),
      ),
      actions: [
        TextButton(
          onPressed: () {
            Navigator.of(context).pop();
          },
          child: const Text('Hủy'),
        ),
        ElevatedButton(
          onPressed: _addSensor,
          child: const Text('Thêm'),
        ),
      ],
    );
  }

  Future<void> _addSensor() async {
    if (_formKey.currentState!.validate()) {
      _formKey.currentState!.save();
      final authService = Provider.of<AuthService>(context, listen: false);
      User? user = authService.getCurrentUser();
      DatabaseReference sensorsRef =
          FirebaseDatabase.instance.ref('users/${user?.uid}/sensors');
      String sensorKey = sensorsRef.push().key ?? '';
      await sensorsRef.child(sensorKey).set({
        'name': _sensorName,
        'temperature': 0,
        'humidity': 0,
      });
      Navigator.of(context).pop();
    }
  }
}
