import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import '../models/models.dart';

class EditSensorDialog extends StatefulWidget {
  final DatabaseReference sensorRef;
  final Sensor sensor;

  const EditSensorDialog(
      {super.key, required this.sensorRef, required this.sensor});

  @override
  _EditSensorDialogState createState() => _EditSensorDialogState();
}

class _EditSensorDialogState extends State<EditSensorDialog> {
  final _formKey = GlobalKey<FormState>();
  late String _sensorName;

  @override
  void initState() {
    super.initState();
    _sensorName = widget.sensor.name;
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: const Text('Chỉnh sửa cảm biến'),
      content: Form(
        key: _formKey,
        child: TextFormField(
          initialValue: _sensorName,
          decoration: const InputDecoration(labelText: 'Tên cảm biến'),
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
              await widget.sensorRef.update({
                'name': _sensorName,
              });
              Navigator.of(context).pop();
            }
          },
        ),
      ],
    );
  }
}
