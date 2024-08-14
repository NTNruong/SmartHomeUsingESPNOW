import 'package:firebase_core/firebase_core.dart' show FirebaseOptions;
import 'package:flutter/foundation.dart'
    show defaultTargetPlatform, kIsWeb, TargetPlatform;

class DefaultFirebaseOptions {
  static FirebaseOptions get currentPlatform {
    if (kIsWeb) {
      throw UnsupportedError(
        'DefaultFirebaseOptions have not been configured for web - '
        'you can reconfigure this by running the FlutterFire CLI again.',
      );
    }
    switch (defaultTargetPlatform) {
      case TargetPlatform.android:
        return android;
      case TargetPlatform.iOS:
        return ios;
      case TargetPlatform.macOS:
        throw UnsupportedError(
          'DefaultFirebaseOptions have not been configured for macos - '
          'you can reconfigure this by running the FlutterFire CLI again.',
        );
      case TargetPlatform.windows:
        throw UnsupportedError(
          'DefaultFirebaseOptions have not been configured for windows - '
          'you can reconfigure this by running the FlutterFire CLI again.',
        );
      case TargetPlatform.linux:
        throw UnsupportedError(
          'DefaultFirebaseOptions have not been configured for linux - '
          'you can reconfigure this by running the FlutterFire CLI again.',
        );
      default:
        throw UnsupportedError(
          'DefaultFirebaseOptions are not supported for this platform.',
        );
    }
  }

  static const FirebaseOptions android = FirebaseOptions(
    apiKey: 'AIzaSyAWygdFVHzIhP9Qtvf6Pz3MhtgIpW_qLbc',
    appId: '1:893245231007:android:65eb8ce249b3df3f3737aa',
    messagingSenderId: '893245231007',
    projectId: 'fir-demo-20d39',
    databaseURL:
        'https://fir-demo-20d39-default-rtdb.asia-southeast1.firebasedatabase.app',
    storageBucket: 'fir-demo-20d39.appspot.com',
  );

  static const FirebaseOptions ios = FirebaseOptions(
    apiKey: 'AIzaSyBLby-c3VOXDYhEAOTd1h_hdCIa11w3_V8',
    appId: '1:893245231007:ios:a96599ab4ce869a83737aa',
    messagingSenderId: '893245231007',
    projectId: 'fir-demo-20d39',
    databaseURL:
        'https://fir-demo-20d39-default-rtdb.asia-southeast1.firebasedatabase.app',
    storageBucket: 'fir-demo-20d39.appspot.com',
    iosBundleId: 'com.example.myFirebaseAuthApp',
  );
}
