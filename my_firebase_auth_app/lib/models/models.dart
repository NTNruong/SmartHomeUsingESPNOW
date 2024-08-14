class Device {
  final String key;
  final String name;
  final String type;
  final bool status;

  Device(this.key, this.name, this.type, this.status);

  factory Device.fromJson(Map<String, dynamic> json, String key) {
    return Device(
      key,
      json['name'] ?? 'Unknown',
      json['type'] ?? 'Unknown',
      json['status'] ?? false, // Ensure this is a boolean
    );
  }
}

class Sensor {
  final String key;
  final String name;
  final double temperature; // Change to double
  final double humidity; // Change to double

  Sensor(this.key, this.name, this.temperature, this.humidity);

  factory Sensor.fromJson(Map<String, dynamic> json, String key) {
    return Sensor(
      key,
      json['name'] ?? 'Unknown',
      (json['temperature'] as num).toDouble(), // Convert to double
      (json['humidity'] as num).toDouble(), // Convert to double
    );
  }
}
