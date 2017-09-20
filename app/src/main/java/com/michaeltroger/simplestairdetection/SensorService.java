package com.michaeltroger.simplestairdetection;

import android.app.Service;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.MediaPlayer;
import android.os.IBinder;
import android.support.annotation.NonNull;

public class SensorService extends Service implements SensorEventListener{
    private SensorManager mSensorManager;

    private MediaPlayer mMediaplayerMoving;
    private float mEwmaAccelleration = 0;
    private static final float ALPHA_ACCELERATION = 0.5f;
    private static final float THRESHOLD_EWMA_ACCELERATION = 0.1f;
    private boolean mMoving;

    private MediaPlayer mMediaplayerStairs;
    private float mEwmaPressure = 0;
    private static final float ALPHA_PRESSURE = 0.5f;
    private static final float THRESHOLD_EWMA_PRESSURE = 0.2f;
    private float mInitialPressure;
    private boolean mOnStairs;

    public static final String INTENT_ACTION = "com.michaeltroger.simplestairdetection.action";
    public static final String INTENT_MSG = "com.michaeltroger.simplestairdetection.msg";
    public static final String WALKING_MSG = "com.michaeltroger.simplestairdetection.walking";
    public static final String STAIRS_MSG = "com.michaeltroger.simplestairdetection.stairs";

    @Override
    public void onCreate() {
        super.onCreate();

        mSensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);

        final Sensor sensorAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
        final Sensor sensorBarometer = mSensorManager.getDefaultSensor(Sensor.TYPE_PRESSURE);
        mSensorManager.registerListener(this, sensorAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(this, sensorBarometer, SensorManager.SENSOR_DELAY_NORMAL);

        mMediaplayerMoving = MediaPlayer.create(getApplicationContext(), R.raw.robot);
        mMediaplayerStairs = MediaPlayer.create(getApplicationContext(), R.raw.typing);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mSensorManager.unregisterListener(this);
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
        if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
            handleAccelerationChanged(event);
        } else if (event.sensor.getType() == Sensor.TYPE_PRESSURE) {
            handlePressureChanged(event);
        }

    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // accuracy is expected to not change
    }

    void handleAccelerationChanged(final @NonNull SensorEvent event) {
        final float accX = event.values[0];
        final float accY = event.values[1];
        final float accZ = event.values[2];

        final int accVectorLength = (int)Math.round(Math.sqrt(accX * accX + accY * accY + accZ * accZ));

        mEwmaAccelleration = ALPHA_ACCELERATION * accVectorLength + (1 - ALPHA_ACCELERATION) * mEwmaAccelleration;

        if (mEwmaAccelleration > THRESHOLD_EWMA_ACCELERATION) {
            if (!mMoving) {
                handleMovementDetected();
                mMoving = true;
            }
        } else {
            mMoving = false;
        }
    }

    private void handlePressureChanged(final @NonNull SensorEvent event) {
        final float pressure = event.values[0];

        if (mEwmaPressure == 0) {
            mInitialPressure = pressure;
            mEwmaPressure = pressure;
        } else {
            mEwmaPressure = ALPHA_PRESSURE * pressure + (1 - ALPHA_PRESSURE) * mEwmaPressure;
        }
        if (mMoving) {
            if (Math.abs(mInitialPressure - mEwmaPressure) > THRESHOLD_EWMA_PRESSURE) {
                handleStairsDetected();
                mOnStairs = true;

                mInitialPressure = mEwmaPressure;
            } else {
                mOnStairs = false;
            }
        }

    }

    private void handleStairsDetected() {
        mMediaplayerStairs.start();

        final Intent local = new Intent(INTENT_ACTION);
        local.putExtra(INTENT_MSG, STAIRS_MSG);
        this.sendBroadcast(local);
    }

    private void handleMovementDetected() {
        mMediaplayerMoving.start();

        final Intent local = new Intent(INTENT_ACTION);
        local.putExtra(INTENT_MSG, WALKING_MSG);
        this.sendBroadcast(local);
    }

}
