import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import '../models/models.dart';

class EditDeviceDialog extends StatefulWidget {
  final DatabaseReference deviceRef;
  final Device device;

  const EditDeviceDialog(
      {super.key, required this.deviceRef, required this.device});

  @override
  _EditDeviceDialogState createState() => _EditDeviceDialogState();
}

class _EditDeviceDialogState extends State<EditDeviceDialog> {
  final _formKey = GlobalKey<FormState>();
  late String _deviceName;
  late String _deviceType;

  final List<String> _deviceTypes = [
    'Đèn',
    'Quạt',
    'Máy lạnh',
    'Máy lọc khí',
  ];

  @override
  void initState() {
    super.initState();
    _deviceName = widget.device.name;
    _deviceType = widget.device.type;
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: const Text('Chỉnh sửa thiết bị'),
      content: Form(
        key: _formKey,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            TextFormField(
              initialValue: _deviceName,
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
          child: const Text('Lưu'),
          onPressed: () async {
            if (_formKey.currentState!.validate()) {
              _formKey.currentState!.save();
              await widget.deviceRef.update({
                'name': _deviceName,
                'type': _deviceType,
              });
              Navigator.of(context).pop();
            }
          },
        ),
      ],
    );
  }
}
