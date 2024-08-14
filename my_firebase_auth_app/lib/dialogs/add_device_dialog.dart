import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:provider/provider.dart';
import '../services/auth_service.dart';

class AddDeviceDialog extends StatefulWidget {
  const AddDeviceDialog({super.key});

  @override
  _AddDeviceDialogState createState() => _AddDeviceDialogState();
}

class _AddDeviceDialogState extends State<AddDeviceDialog> {
  final _formKey = GlobalKey<FormState>();
  String _deviceName = '';
  String _deviceType = 'Đèn';
  final bool _deviceStatus = false;
  DatabaseReference? _deviceRef;

  final List<String> _deviceTypes = [
    'Đèn',
    'Quạt',
    'Máy lạnh',
    'Máy lọc khí',
    'Công tắt',
  ];

  @override
  Widget build(BuildContext context) {
    final authService = Provider.of<AuthService>(context, listen: false);
    return AlertDialog(
      title: const Text('Thêm phụ kiện'),
      content: Form(
        key: _formKey,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            TextFormField(
              decoration: const InputDecoration(labelText: 'Tên thiết bị'),
              validator: (value) {
                if (value == null || value.isEmpty) {
                  return 'Vui lòng nhập tên thiết bị';
                }
                return null;
              },
              onSaved: (value) {
                _deviceName = value!;
              },
            ),
            DropdownButtonFormField<String>(
              decoration: const InputDecoration(labelText: 'Loại thiết bị'),
              value: _deviceType,
              onChanged: (String? newValue) {
                setState(() {
                  _deviceType = newValue!;
                });
              },
              items: _deviceTypes.map<DropdownMenuItem<String>>((String value) {
                return DropdownMenuItem<String>(
                  value: value,
                  child: Text(value),
                );
              }).toList(),
            ),
          ],
        ),
      ),
      actions: [
        TextButton(
          child: const Text('Hủy'),
          onPressed: () {
            Navigator.of(context).pop();
          },
        ),
        ElevatedButton(
          child: const Text('Thêm'),
          onPressed: () async {
            if (_formKey.currentState!.validate()) {
              _formKey.currentState!.save();
              User? user = authService.getCurrentUser();
              _deviceRef = FirebaseDatabase.instance
                  .ref('users/${user?.uid}/devices')
                  .push();
              await _deviceRef!.set({
                'name': _deviceName,
                'type': _deviceType,
                'status': _deviceStatus,
              });
              Navigator.of(context).pop();
            }
          },
        ),
      ],
    );
  }
}
