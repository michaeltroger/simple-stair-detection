package com.michaeltroger.simpleactivityrecognition;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.MediaPlayer;
import android.os.IBinder;
import android.os.PowerManager;
import android.util.Log;

public class SensorService extends Service implements SensorEventListener{
    private static final int ONGOING_NOTIFICATION_ID = 1;
    private SensorManager mSensorManager;
    private Sensor mSensor;
    private MediaPlayer mMediaplayer;
    private float mEwma = 0;
    private static final float ALPHA = 0.5f;
    private static final float THRESHOLD = 0.1f;

    private static final String TAG = SensorService.class.getSimpleName();
    private boolean mMoving;

    @Override
    public void onCreate() {
        super.onCreate();

        mSensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
        mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);

        mSensorManager.registerListener(this, mSensor, SensorManager.SENSOR_DELAY_NORMAL);

        mMediaplayer = MediaPlayer.create(getApplicationContext(), R.raw.robot);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        float accX = event.values[0];
        float accY = event.values[1];
        float accZ = event.values[2];

        int accVectorLength = (int)Math.round(Math.sqrt(accX * accX + accY * accY + accZ * accZ));

        mEwma = ALPHA * accVectorLength + (1 - ALPHA) * mEwma;

        Log.d(TAG, "ewma: " + mEwma);
        if (mEwma > THRESHOLD) {
            if (!mMoving && !mMediaplayer.isPlaying()) {
                mMediaplayer.start();
                mMoving = true;
            }
        } else {
            mMoving = false;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {}
}
